#include "world.hpp"

std::vector<Entity *> World::entities;
std::vector<Player *> World::players;
std::vector<BonusHp *> World::bonuses_hp;
float World::elapsedTime;

using namespace std;

void World::initialize() {}

std::vector<Entity *> World::getEntities() { return entities; }

std::vector<Entity *> World::getNearEntities(sf::Vector2f position,
                                             float radius) {
    std::vector<Entity *> result;
    for (auto e : entities)
        if (e->distanceToPosition(position) < radius) result.push_back(e);

    return result;
}

void World::clearEntities() {
    for (auto e : entities) {
        delete e;
    }
    entities.clear();
    players.clear();
    bonuses_hp.clear();
}

std::vector<Entity *> World::getCollidingEntities(Entity *entity) {
    std::vector<Entity *> result;
    for (auto e : entities)
        if (e != entity) {
            bool collide = false;
            for (auto h : e->getHitboxes())
                for (auto sh : entity->getHitboxes())
                    if (h->getGlobalBounds().intersects(
                            sh->getGlobalBounds()) &&
                        !collide) {
                        result.push_back(e);
                        collide = true;
                    }
        }

    return result;
}

std::vector<Entity *> World::getCollidingEntitiesOnZAxis(Entity *entity) {
    std::vector<Entity *> result;
    for (auto e : entities)
        if (e != entity) {
            bool collide = false;
            for (auto h : e->getZHitboxes())
                for (auto sh : entity->getZHitboxes())
                    if (h->getGlobalBounds().intersects(
                            sh->getGlobalBounds()) &&
                        !collide) {
                        result.push_back(e);
                        collide = true;
                    }
        }

    return result;
}

std::vector<Entity *> World::testCollidingEntities(Entity *entity,
                                                   sf::Vector2f movement) {
    std::vector<Entity *> result;
    for (auto sh : entity->getHitboxes()) {
        sf::RectangleShape testHitbox = sf::RectangleShape(*sh);
        testHitbox.move(movement);
        for (auto e : entities)
            if (e != entity) {
                bool collide = false;
                for (auto h : e->getHitboxes())
                    if (h->getGlobalBounds().intersects(
                            testHitbox.getGlobalBounds()) &&
                        !collide) {
                        result.push_back(e);
                        collide = true;
                    }
            }
    }

    return result;
}

std::vector<Entity *> World::testCollidingEntitiesOnZAxis(
    Entity *entity, sf::Vector2f movement) {
    std::vector<Entity *> result;
    for (auto sh : entity->getZHitboxes()) {
        sf::RectangleShape testHitbox = sf::RectangleShape(*sh);
        testHitbox.move(movement);
        for (auto e : entities)
            if (e != entity) {
                bool collide = false;
                for (auto h : e->getZHitboxes())
                    if (h->getGlobalBounds().intersects(
                            testHitbox.getGlobalBounds()) &&
                        !collide) {
                        result.push_back(e);
                        collide = true;
                    }
            }
    }

    return result;
}

void World::setElapsedTime(float time) { elapsedTime = time / 1000; }

void World::addEntity(Entity *entity, bool isPlayer) {
    entities.push_back(entity);
    if (isPlayer) players.push_back((Player *)entity);
}

void World::addHpEntity(Entity *entity) {
  entities.push_back(entity);
  bonuses_hp.push_back((BonusHp *) entity);
}

void World::updateEntities() {
    for (unsigned int i = 0; i < entities.size(); i++) {
        entities.at(i)->update(elapsedTime);
        if (entities.at(i)->isDead()) {
            delete (entities.at(i));
            entities.erase(entities.begin() + i);
        }
    }
}

float World::getElapsedTime() { return elapsedTime; }

void World::render(sf::RenderWindow &app) {
    std::sort(entities.begin(), entities.end(), sortUsingFirstZHitbox);

    for (auto e : entities) app.draw(*e);
}

std::vector<Player *> World::getPlayers() { return players; }

bool World::sortUsingFirstZHitbox(Entity *a, Entity *b) {
    if (a->isIgnoringDepthOnRendering()) return true;
    if (b->isIgnoringDepthOnRendering()) return false;

    auto ha = a->getZHitboxes().at(0)->getPosition().y +
              a->getZHitboxes().at(0)->getSize().y;
    auto hb = b->getZHitboxes().at(0)->getPosition().y +
              b->getZHitboxes().at(0)->getSize().y;
    return ha < hb;
}

void World::managePlayersCollidingWithThings() {
    for (auto player : players)
        for (Entity *ent : World::getCollidingEntitiesOnZAxis(player)) {
            switch (ent->getEntityType()) {
                case EntityType::BONUS:
                    World::managePlayerCollidingWithBonus(player, (Bonus *)ent);
                    break;
                case EntityType::ENEMY:
                    World::managePlayerCollidingWithEnemy(player, (Enemy *)ent);
                    break;
                case EntityType::NONE:
                case EntityType::PLAYER:
                case EntityType::SFX:
                case EntityType::SOLID:
                    break;  // do nothing?
            }
        }
}

void World::managePlayerCollidingWithBonus(Player *player, Bonus *bonus) {
    switch (bonus->getBonusType()) {
        case BONUS_HP:
            player->alterHealth(((BonusHp *)bonus)->getHealedAmount(), true);
            bonus->alterHealth(-1, false);
            break;
        case BONUS_WEAPON:
            if (((ItemWeapon *)bonus)->isPickable()) {
                WeaponType playerWeapon = player->getWeapon()->getWeaponType();
                player->equip(
                    new Weapon(((ItemWeapon *)bonus)->getWeaponType()));
                ((ItemWeapon *)bonus)->setWeaponType(playerWeapon);
                ((ItemWeapon *)bonus)->isJustDropped();
            }
            break;
    }
}

void World::managePlayerCollidingWithEnemy(Player *player, Enemy *enemy) {
    player->isHit(enemy->getAttackDamage());
}

void World::scroll() {
    // Move all entities
    for (Entity *entity : entities) {
        if (entity->getEntityType() != PLAYER) {
            entity->move(Vector2f(-SCROLL_SPEED * World::getElapsedTime(), 0));
            if (entity->getZHitboxes().at(0)->getGlobalBounds().left +
                    entity->getZHitboxes().at(0)->getGlobalBounds().width <
                0)
                entity->alterHealth(-1, false);
            // Check if a player is pushed
            else if (entity->getEntityType() == SOLID) {
                for (Player *player : players)
                    while (player->collideWith(entity)) {
                        if (player->getHitboxes()
                                .at(0)
                                ->getGlobalBounds()
                                .left > 0)
                            player->move(sf::Vector2f(-1, 0));
                        else {
                            entity->alterHealth(-1, false);
                            player->isHit(SCROLL_DAMAGE);
                            break;
                        }
                    }
            }
        }
    }
}


// Collision detection
void World::checkCollision(float elapsedTime, Vector2u windowSize){

    for(unsigned int i = 0; i < players.size(); i++){

      /*for(Arrow* arrow : players.at(i)->getArrows()){
            for(unsigned int j = 0; j < enemies.size(); j++){
                if(enemies.at(j)->detectHit(arrow)){
                    enemies.at(j)->take_damage(Arrow::ARROW_DAMAGE);
                    arrow->kill();
                }
            }
            for(unsigned int j = 0; j < breakable_objects.size(); j++){
                if(arrow->detectHit(breakable_objects.at(j))){
                    arrow->kill();

                    if(Bonus* tmp = breakable_objects.at(j)->getDrops()){
                        Vector2f dropPosition =
                        Vector2f(breakable_objects.at(j)->getHitbox().getGlobalBounds().left
                                                        ,
                                                        breakable_objects.at(j)->getHitbox().getGlobalBounds().top
                                                        +
                                                        breakable_objects.at(j)->getHitbox().getGlobalBounds().height
                                                        -100);
                        dropItem(tmp, dropPosition);
                    }

                    delete(breakable_objects.at(j));
                    breakable_objects.erase(breakable_objects.begin()+j);

                }
            }
        }
        if(!players.at(i)->isJumping()){

            //Collisions with enemies
            if(player_invulnerability_timer <= 0){
                for(unsigned int j = 0; j < enemies.size(); j++){
                    if(enemies.at(j)->detectHit(players.at(i))){
                        players.at(i)->getLife()->decrease(enemies.at(j)->getAttackDamage());
                        player_invulnerability_timer = 200;
                        break;
                    }
                    if(enemies.at(j)->getEnemyType() == Enemy_Dragon){
                       vector<Flame*> flames =
                       ((Dragon*)enemies.at(j))->getFlames(); for(Flame* flame
                       : flames){
                            if(players.at(i)->detectHit(flame)){
                               players.at(i)->getLife()->decrease(Flame::FLAMES_DAMAGE);
                               player_invulnerability_timer = 200;
                               break;
                            }
                       }
                    }

                }
            }
            else{
                player_invulnerability_timer -= elapsedTime;
            }
      */
            //Collisions with bonus hp
            for(unsigned int j = 0; j < bonuses_hp.size(); j++){

                //Check for collisions between the player and the item
                if(players.at(i)->collideWith(bonuses_hp.at(j))){

		  players.at(i)->alterHealth(bonuses_hp.at(j)->getHealedAmount(), true);
                    delete(bonuses_hp.at(j));
                    bonuses_hp.erase(bonuses_hp.begin()+j);
                }
            }
	    
            //Collisions with weapon items
            /*for(unsigned int j = 0; j < item_weapons.size(); j++){

                //Check for collisions between the player and the item
                if(players.at(i)->getLastDroppedItem() == NULL ||
                !players.at(i)->detectHit(players.at(i)->getLastDroppedItem())){
                    players.at(i)->setLastDroppedItem(NULL);
                    if(item_weapons.at(j)->detectHit(players.at(i))){

                        WeaponType weaponType =
                        item_weapons.at(j)->getWeaponType();
                        if(players.at(i)->getWeapon()->getWeaponType() !=
                        weaponType){

                            //Drop old weapon
                            ItemWeapon* tmp = new
                            ItemWeapon(players.at(i)->getWeapon()->getWeaponType(),
                            item_weapons.at(j)->getPosition());

                            /* setDropped used to prevent the player from
                            interacting with the item again when he's still
                            standing on it */
	    /*item_weapons.push_back(tmp);
                            players.at(i)->setLastDroppedItem(tmp);

                            /*If an other player is standing on the item,
			      change it's last dropped item*/
	    /*		    for(Player* var : players)
                                if(var->getLastDroppedItem() ==
                                item_weapons.at(j))
                                    var->setLastDroppedItem(tmp);

                            delete(item_weapons.at(j));
                            item_weapons.erase(item_weapons.begin()+j);

                            //Equip new weapon
                            players.at(i)->equip(new Weapon(weaponType));

                        }
                    }
                }
            }*/
    }
    //    }
}
