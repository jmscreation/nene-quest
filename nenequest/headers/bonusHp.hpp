#ifndef BONUSHP_H
#define BONUSHP_H

#include <SFML/Graphics.hpp>

#include "bonus.hpp"

enum BonusHpType { BONUS_ONIGIRI };

class BonusHp : public Bonus {
   public:
    static const short int ONIGIRI = 1;
    BonusHp(BonusHpType bonusType, sf::Vector2f position, int strength=-1);
    virtual ~BonusHp();
    short int getHealedAmount();

   protected:
   private:
    static const int ONIGIRI_HEALED_AMOUNT = 50;
    short int healed_amount;
};

#endif  // BONUSHP_H
