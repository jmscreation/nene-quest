#include "game.hpp"

#include <stdlib.h>
#include <time.h>
#include <iostream>

#include "configManager.hpp"
#include "configParameter.hpp"
#include "configParser.hpp"
#include "world.hpp"
#include "screens.hpp"

using namespace std;
using namespace sf;

bool Game::debugMode;

Game::Game() {
    // Final game objects (should only include Game, Players, LevelManagers and
    // other UI events)
    this->manager = new LevelManager(this);
    this->parser = new LevelParser();
    this->parser->setLevelManager(this->manager);
    this->parser->initialize();
    this->parser->setLevelFilesPath("levels");
    debugMode = true;

    // Config loader
    this->configManager = new ConfigManager();
    this->configParser = new ConfigParser();
    this->configParser->setConfigManager(this->configManager);
    this->configParser->initialize();
}

int Game::run(RenderWindow& app) {

    Event event;
    Background background = Background(app.getSize());
    this->manager->setBackground(&background);
    std::cout << "??";
    // Load settings
    this->configParser->parseFile("config.ini");
    // Bind keys
    Keyboard::Key kbDebugMode;
    Keyboard::Key kbPause;

    Keyboard::Key kbPlayerOneLeft;
    Keyboard::Key kbPlayerOneRight;
    Keyboard::Key kbPlayerOneUp;
    Keyboard::Key kbPlayerOneDown;
    Keyboard::Key kbPlayerOneAttack;
    Keyboard::Key kbPlayerOneJump;

    Keyboard::Key kbPlayerTwoLeft;
    Keyboard::Key kbPlayerTwoRight;
    Keyboard::Key kbPlayerTwoUp;
    Keyboard::Key kbPlayerTwoDown;
    Keyboard::Key kbPlayerTwoAttack;
    Keyboard::Key kbPlayerTwoJump;

    this->configManager->getParameter("toggleDebugMode")
        ->getParameter(&kbDebugMode);
    this->configManager->getParameter("togglePause")->getParameter(&kbPause);

    this->configManager->getParameter("playerOneMoveLeft")
        ->getParameter(&kbPlayerOneLeft);
    this->configManager->getParameter("playerOneMoveRight")
        ->getParameter(&kbPlayerOneRight);
    this->configManager->getParameter("playerOneMoveUp")
        ->getParameter(&kbPlayerOneUp);
    this->configManager->getParameter("playerOneMoveDown")
        ->getParameter(&kbPlayerOneDown);
    this->configManager->getParameter("playerOneAttack")
        ->getParameter(&kbPlayerOneAttack);
    this->configManager->getParameter("playerOneJump")
        ->getParameter(&kbPlayerOneJump);

    this->configManager->getParameter("playerTwoMoveLeft")
        ->getParameter(&kbPlayerTwoLeft);
    this->configManager->getParameter("playerTwoMoveRight")
        ->getParameter(&kbPlayerTwoRight);
    this->configManager->getParameter("playerTwoMoveUp")
        ->getParameter(&kbPlayerTwoUp);
    this->configManager->getParameter("playerTwoMoveDown")
        ->getParameter(&kbPlayerTwoDown);
    this->configManager->getParameter("playerTwoAttack")
        ->getParameter(&kbPlayerTwoAttack);
    this->configManager->getParameter("playerTwoJump")
        ->getParameter(&kbPlayerTwoJump);

    // Binding doesn't work for me for whatever reason...
    /*kbPlayerOneLeft = Keyboard::Left;
    kbPlayerOneRight = Keyboard::Right;
    kbPlayerOneUp = Keyboard::Up;
    kbPlayerOneDown = Keyboard::Down;
    kbPlayerOneAttack = Keyboard::M;
    kbPlayerOneJump = Keyboard::L;

    kbPlayerTwoLeft = Keyboard::Q;
    kbPlayerTwoRight = Keyboard::D;
    kbPlayerTwoUp = Keyboard::Z;
    kbPlayerTwoDown = Keyboard::S;
    kbPlayerTwoAttack = Keyboard::E;
    kbPlayerTwoJump = Keyboard::Space;*/

    // Load Level
    this->parser->parseFile("level0.nnq");
    // Testing objects////////////
    /*bridge = new BridgePit(300, app.getSize().y - background.getSkyHeight(),
                           app.getSize().y);
    World::addEntity(bridge);
    Boar *boar1 =
        new Boar(Vector2f(app.getSize().x - 1010, app.getSize().y / 2));
    World::addEntity(boar1);
    Dragon *dragon = new Dragon(Vector2f(1000, 400));
    World::addEntity(dragon);
    BonusHp *onigiri = new BonusHp(BONUS_ONIGIRI, Vector2f(900, 900));
    ItemWeapon *sword = new ItemWeapon(Sword, Vector2f(800, 600));
    World::addEntity(onigiri);
    World::addEntity(sword);
    World::addEntity(new ItemWeapon(Axe, Vector2f(300, 630)));
    BreakableObject *barrel = new BreakableObject(Chest, Vector2f(520, 630));
    World::addEntity(barrel);
    World::addEntity(new BreakableObject(Barrel, Vector2f(700, 430)));
    */
    // Arrow arrow = Arrow(Vector2f(100, 700), 700);
    BonusHp *onigiri = new BonusHp(BONUS_ONIGIRI, Vector2f(900, 900));
    World::addEntity(onigiri);
    std::cout << "Test2";
    /////////////////////////////

    Clock clock;
    // ---------------- Main Loop ----------------
    while (true) {
        float elapsedTime = clock.restart().asMilliseconds();

        while (app.pollEvent(event)) {
            if (event.type == Event::Closed) return (-1);
	    if (event.type == Event::KeyPressed && event.key.code == Keyboard::Key::Escape){
	      cleanScreen();
	      return TITLE_SCREEN;
	    }
            manageInputs(event, PlayerID::PLAYER1, kbPlayerOneUp,
                         kbPlayerOneDown, kbPlayerOneLeft, kbPlayerOneRight,
                         kbPlayerOneJump);

            if (World::getPlayers().size() == 2)
                manageInputs(event, PlayerID::PLAYER2, kbPlayerTwoUp,
                             kbPlayerTwoDown, kbPlayerTwoLeft, kbPlayerTwoRight,
                             kbPlayerTwoJump);

            manageMetaInputs(event, kbDebugMode);
        }

        World::setElapsedTime(elapsedTime);
        World::updateEntities();
        //World::scroll();
        World::managePlayersCollidingWithThings();

        // scroll(elapsedTime, app.getSize());

        this->manager->update();
        background.update();
        // arrow.update(elapsedTime);
	//World::checkCollision(elapsedTime, app.getSize());
        app.clear(Color::White);
        app.draw(background);
        // app.draw(*bridge);
        World::render(app);
        // drawWithDepth(&app);
        // app.draw(arrow);
        app.display();
    }

    return (-1);
}

// Temporary solution for input management
void Game::manageInputs(sf::Event e, PlayerID id, Keyboard::Key up,
                        Keyboard::Key down, Keyboard::Key left,
                        Keyboard::Key right, Keyboard::Key attack) {
  std::vector<Player *> players = World::getPlayers();
    if (e.type == Event::KeyPressed) {
        if (e.key.code == up)
            players.at(id)->moving_up = true;
        else if (e.key.code == down)
            players.at(id)->moving_down = true;
        else if (e.key.code == right)
            players.at(id)->moving_right = true;
        else if (e.key.code == left)
            players.at(id)->moving_left = true;
        else if (e.key.code == attack) {
            players.at(id)->attack();
            /*if (!this->players.at(id)->isShooting())
                    this->players.at(id)->fireArrow();*/
        }
    } else if (e.type == Event::KeyReleased) {
        if (e.key.code == up)
            players.at(id)->moving_up = false;
        else if (e.key.code == down)
            players.at(id)->moving_down = false;
        else if (e.key.code == right)
            players.at(id)->moving_right = false;
        else if (e.key.code == left)
            players.at(id)->moving_left = false;
        else if (e.key.code == attack)
            players.at(id)->setShootingState(false);
    }
}

void Game::manageMetaInputs(sf::Event e, Keyboard::Key toggleDebug) {
    if (e.type == Event::KeyPressed) {
        if (e.key.code == toggleDebug) debugMode = !debugMode;
    }
}

void Game::cleanScreen() {
    // no need to delete elements of the following vectors as they are
    // membre of World::getEntities()
    /*this->players.clear();
    this->bonuses_hp.clear();
    this->item_weapons.clear();
    this->breakable_objects.clear();*/
    World::clearEntities();
}

// Unused
// bool Game::playerIsColliding(Player* p){
//    for(unsigned int j = 0; j < breakable_objects.size(); j++){
//        if(p->detectHit(breakable_objects.at(j))){
//            return true;
//        }
//    }
//    return false;
//
// }
//
// BreakableObject* Game::getCollidingObject(Player* p){
//    for(BreakableObject* obj : breakable_objects)
//        if(p->detectHit(obj))
//            return obj;
//
//    return NULL;
// }

// Move to Render class
void Game::drawWithDepth(RenderWindow *app) {
    for (auto e : World::getEntities()) app->draw(*e);
}

bool Game::cmp(Entity *a, Entity *d) {
    if (a->getDrawDepth() >= d->getDrawDepth()) return false;
    return true;
}

// Function calls for levelManager
void Game::addInstance(Entity *e) { World::addEntity(e); }

void Game::addPlayerInstance(Player *player) {
    // this->players.push_back(player);
    World::addEntity(player, true);
}

bool Game::IsDebugMode() { return debugMode; }
