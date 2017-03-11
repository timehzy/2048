//
//  ScoreBoard.cpp
//  towOfour8
//
//  Created by Michael-Nine on 2017/3/9.
//
//

#include "ScoreBoard.hpp"

ScoreBoard* ScoreBoard::createScoreBoard(ScoreBoardType type, int score, cocos2d::Size size, Point position){
    ScoreBoard *sb = new ScoreBoard();
    if (sb && sb->init()) {
        sb->autorelease();
        sb->initScoreBoard(type, score, size, position);
        return sb;
    }
    CC_SAFE_DELETE(sb);
    return NULL;
}

bool ScoreBoard::init(){
    if (Layer::init()) {
        return true;
    }
    return false;
}

void ScoreBoard::initScoreBoard(ScoreBoardType type, int score, cocos2d::Size size, Point position){
    background = LayerColor::create(Color4B(55, 85, 136, 255), size.width, size.height);
    background->setPosition(position);
    addChild(background);
    if (type == ScoreBoardTypeCurrent) {
        auto *titleLabel = Label::createWithSystemFont("score", "Arial", 26);
        titleLabel->setPosition(size.width / 2, size.height - 20);
        background->addChild(titleLabel);
    }else if (type == ScoreBoardTypeHighest){
        auto *titleLabel = Label::createWithSystemFont("highest", "Arial", 26);
        titleLabel->setPosition(size.width / 2, size.height - 20);
        background->addChild(titleLabel);
    }
    scoreLabel = Label::createWithSystemFont(StringUtils::format("%d", score), "Arial", 36);
    scoreLabel->setPosition(size.width / 2, 30);
    background->addChild(scoreLabel);
}

void ScoreBoard::setScore(int num){
    scoreLabel->setString(StringUtils::format("%d", num));
}

int ScoreBoard::getScore(){
    return atoi(scoreLabel->getString().c_str());
}
