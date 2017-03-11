//
//  CardSprite.hpp
//  towOfour8
//
//  Created by Michael-Nine on 2017/3/9.
//
//

#ifndef CardSprite_hpp
#define CardSprite_hpp
#define CARD_BORDER 4

#include <stdio.h>
#include "cocos2d.h"

USING_NS_CC;
class CardSprite : public Sprite {
public:
    static CardSprite* createCardSprite(int num, int size, float x, float y);
    
    int getNumber();
    void setNumber(int num);
    
private:
    int number;
    Label *numberLabel;
    LayerColor *backgroundLayer;
    
    void initCard(int num, int size, float x, float y);
    virtual bool init();
    CREATE_FUNC(CardSprite);
};
#endif /* CardSprite_hpp */
