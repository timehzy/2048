//
//  CardSprite.cpp
//  towOfour8
//
//  Created by Michael-Nine on 2017/3/9.
//
//

#include "CardSprite.hpp"

CardSprite* CardSprite::createCardSprite(int num, int size, float x, float y){
    CardSprite *card = new CardSprite();
    card->setPosition(x, y);
    card->setContentSize(Size(size, size));
    if (card && card->init()) {
        card->autorelease();
        card->initCard(num, size, x, y);
        return card;
    }
    CC_SAFE_DELETE(card);
    return NULL;
}

bool CardSprite::init(){
    if (!Sprite::init()) {
        return false;
    }
    return true;
}

void CardSprite::initCard(int num, int size, float x, float y){
    number = num;
    backgroundLayer = LayerColor::create(Color4B(198, 91,23, num ? 255 : 0), size - CARD_BORDER, size - CARD_BORDER);
    backgroundLayer->setPosition(0, 0);
    if (number > 0) {
        numberLabel = Label::createWithSystemFont(StringUtils::format("%i", number), "Arial", 28);
    }else{
        numberLabel = Label::createWithSystemFont("", "Arial", 28);
    }
    numberLabel->setPosition(size / 2, size / 2);
    backgroundLayer->addChild(numberLabel);
    addChild(backgroundLayer);
}

int CardSprite::getNumber(){
    return number;
}

void CardSprite::setNumber(int num){
    number = num;
    if (num > 0) {
        numberLabel->setString(StringUtils::format("%i", num));
        backgroundLayer->setOpacity(255);
    }else{
        numberLabel->setString("");
        backgroundLayer->setOpacity(0);
    }
}
