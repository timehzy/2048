#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#include "CardSprite.hpp"
#include "time.h"
#include "ScoreBoard.hpp"
#include "iostream"

USING_NS_CC;
using namespace std;

Scene* HelloWorld::createScene(){
    auto scene = Scene::create();
    auto layer = HelloWorld::create();
    scene->addChild(layer);
    return scene;
}

bool HelloWorld::init(){
    if ( !Layer::init() )
    {
        return false;
    }
    
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    swipeD = SwipeNone;
    
    auto *background = LayerColor::create(Color4B(255, 252, 235, 255));
    addChild(background);
    
    curSb = ScoreBoard::createScoreBoard(ScoreBoardTypeCurrent, 0, Size(visibleSize.width / 3 - 20, 100), Point(20, visibleSize.height - 120));
    background->addChild(curSb);
    
    int highScore = UserDefault::getInstance()->getIntegerForKey("HighestScore");
    highestSb = ScoreBoard::createScoreBoard(ScoreBoardTypeHighest, highScore, Size(visibleSize.width / 3 - 20, 100), Point(curSb->getPosition().x + visibleSize.width / 3 + 20, visibleSize.height - 120));
    background->addChild(highestSb);
    
    auto *reStartButton = MenuItemLabel::create(Label::createWithSystemFont("restart", "Arial", 38), CC_CALLBACK_1(HelloWorld::restart, this));
    reStartButton->setColor(Color3B::BLACK);
    auto *item = Menu::createWithItem(reStartButton);
    item->setPosition(visibleSize.width - 80, visibleSize.height - 80);
    background->addChild(item);
    
    cardsBg = LayerColor::create(Color4B(162, 201, 237, 255), visibleSize.width - 20, visibleSize.width - 20);
    cardsBg->setPosition(10, (visibleSize.height - visibleSize.width - 20 - 100) / 2);
    background->addChild(cardsBg);
    createCards();
    
    auto *downMask = LayerColor::create(Color4B(255, 252, 235, 255), visibleSize.width, visibleSize.width / 4);
    downMask->setPosition(0, cardsBg->getPosition().y - visibleSize.width / 4);
    background->addChild(downMask);
    
    auto *leftMask = LayerColor::create(Color4B(255, 252, 235, 255), 10, visibleSize.width);
    leftMask->setPosition(0, cardsBg->getPosition().y);
    background->addChild(leftMask);
    
    auto *rightMask = LayerColor::create(Color4B(255, 252, 235, 255), 10, visibleSize.width);
    rightMask->setPosition(visibleSize.width - 10, cardsBg->getPosition().y);
    background->addChild(rightMask);

    auto *upMask = LayerColor::create(Color4B(255, 252, 235, 255), visibleSize.width, visibleSize.width / 4);
    upMask->setPosition(0, cardsBg->getPosition().y + cardsBg->getContentSize().width);
    background->addChild(upMask);
    
    auto *listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(HelloWorld::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(HelloWorld::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(HelloWorld::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(HelloWorld::onTouchCancelled, this);
    auto *dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    
    return true;
}

void HelloWorld::restart(cocos2d::Ref *sender){
    for (int i = 0; i<4; i++) {
        for (int j = 0; j<4; j++) {
            cards[i][j]->removeFromParent();
        }
    }
    createCards();
    curSb->setScore(0);
}

void HelloWorld::createCards(){
    for (int i = 0; i<4; i++) {
        for (int j = 0; j<4; j++) {
            cards[i][j] = CardSprite::createCardSprite(arc4random() % 4 > 2 ? 2 : 0, cardsBg->getContentSize().width / 4, cardsBg->getContentSize().width / 4 * i + CARD_BORDER / 2, cardsBg->getContentSize().width / 4 * j + 2);
            cardsBg->addChild(cards[i][j], 1);
        }
    }
}

bool HelloWorld::doLeft(){
    bool showNewNum = false;
    int score = 0;

    //计算合并后的卡片数字
    for (int i=0; i<3; i++) {
        for (int j=0; j<4; j++) {
            if (cards[i][j]->getNumber() == 0 || (cards[i][j]->getNumber() == cards[i+1][j]->getNumber())) {
                if (cards[i][j]->getNumber() == cards[i+1][j]->getNumber()) {
                    score += cards[i][j]->getNumber();
                }
                cards[i][j]->setNumber(cards[i+1][j]->getNumber() + cards[i][j]->getNumber());
                cards[i+1][j]->setNumber(0);
                showNewNum = true;
            }
        }
    }
    
    CardSprite *tempCards[4];
    //将卡片的数字全部右移一个单位
    for (int j=0; j<4; j++) {
        for (int i=3; i>=1; i--) {
            if (!moveLineFlag[j]) {
                break;
            }
            cards[i][j]->setNumber(cards[i-1][j]->getNumber());
        }
        //将要移动到最后的第一列缓存
        tempCards[j] = cards[0][j];
        //最左边一排数字归零
        if (moveLineFlag[j]) {
            cards[0][j]->setNumber(0);
        }
    }
    
    //最左边的一列将成为新卡片进入屏幕，随机取一个计算新生成的卡片数字，并设置值
    int num = arc4random() % 4;
    while (!moveLineFlag[num]) {
        num = arc4random() % 4;
    }
    cards[0][num]->setNumber((arc4random() % 2 + 1) * 2);
    
    //将最左边的一列移动到最右边的屏幕外面，并动画进入的屏幕
    for (int i=0; i<4; i++) {
        for (int j=0; j<4; j++) {
            if (!moveLineFlag[i]) {
                break;
            }
            if (j == 3) {
                cards[j][i] = tempCards[i];
                cards[j][i]->setPosition(cardsBg->getContentSize().width + 3, cards[j][i]->getPosition().y);
                cards[j][i]->runAction(MoveTo::create(.25, Vec2(cardsBg->getContentSize().width / 4 * 3 + 2, cards[j][i]->getPosition().y)));
            }else{
                cards[j][i] = cards[j+1][i];
            }
        }
    }
    refreshScoreBoard(score);
    return showNewNum;
}



bool HelloWorld::doDown(){
    bool showNewNum = false;
    int score = 0;

    for (int i=0; i<4; i++) {
        for (int j=0; j<3; j++) {
            if (cards[i][j]->getNumber() == 0 || (cards[i][j]->getNumber() == cards[i][j+1]->getNumber())) {
                if (cards[i][j]->getNumber() == cards[i][j+1]->getNumber()) {
                    score += cards[i][j]->getNumber();
                }
                cards[i][j]->setNumber(cards[i][j+1]->getNumber() + cards[i][j]->getNumber());
                cards[i][j+1]->setNumber(0);
                showNewNum = true;
            }
        }
    }
    
    CardSprite *tempCards[4];
    for (int i=0; i<4; i++) {
        for (int j=3; j>=1; j--) {
            if (!moveLineFlag[i]) {
                break;
            }
            cards[i][j]->setNumber(cards[i][j-1]->getNumber());
        }
        //将要移动到最后的第一行缓存
        tempCards[i] = cards[i][0];
        //最下边一排数字归零
        if (moveLineFlag[i]) {
            cards[i][0]->setNumber(0);
        }
    }
    
    //最下边的一行将成为新卡片进入屏幕，随机取一个计算新生成的卡片数字，并设置值
    int num = arc4random() % 4;
    while (!moveLineFlag[num]) {
        num = arc4random() % 4;
    }
    cards[num][0]->setNumber((arc4random() % 2 + 1) * 2);
    
    for (int i=0; i<4; i++) {
        for (int j=0; j<4; j++) {
            if (!moveLineFlag[i]) {
                break;
            }
            if (j==3) {
                cards[i][j] = tempCards[i];
                cards[i][j]->setPosition(cards[i][j]->getPosition().x, cardsBg->getContentSize().width + 3);
                cards[i][j]->runAction(MoveTo::create(.25, Vec2(cards[i][j]->getPosition().x, cardsBg->getContentSize().width / 4 * 3 + 2)));
            }else{
                cards[i][j] = cards[i][j+1];
            }
        }
    }
    refreshScoreBoard(score);
    return showNewNum;
}

bool HelloWorld::doRight(){
    bool showNewNum = false;
    int score = 0;

    for (int i=3; i>=1; i--) {
        for (int j=3; j>=0; j--) {
            if (cards[i][j]->getNumber() == 0 || (cards[i][j]->getNumber() == cards[i-1][j]->getNumber())) {
                if (cards[i][j]->getNumber() == cards[i-1][j]->getNumber()) {
                    score += cards[i][j]->getNumber();
                }
                cards[i][j]->setNumber(cards[i-1][j]->getNumber() + cards[i][j]->getNumber());
                cards[i-1][j]->setNumber(0);
                showNewNum = true;
            }
        }
    }
    CardSprite *tempCards[4];
    for (int j=0; j<4; j++) {
        for (int i=0; i<3; i++) {
            if (!moveLineFlag[j]) {
                break;
            }
            cards[i][j]->setNumber(cards[i+1][j]->getNumber());
        }
        //将要移动到最后的第一列缓存
        tempCards[j] = cards[3][j];
        //最右边一列数字归零
        if (moveLineFlag[j]) {
            cards[3][j]->setNumber(0);
        }
    }
    
    //最右边的一行将成为新卡片进入屏幕，随机取一个计算新生成的卡片数字，并设置值
    int num = arc4random() % 4;
    while (!moveLineFlag[num]) {
        num = arc4random() % 4;
    }
    cards[3][num]->setNumber((arc4random() % 2 + 1) * 2);
    
    for (int j=0; j<4; j++) {
        for (int i=3; i>=0; i--) {
            if (!moveLineFlag[j]) {
                break;
            }
            if (i==0) {
                cards[i][j] = tempCards[j];
                cards[i][j]->setPosition(-cardsBg->getContentSize().width / 4, cards[i][j]->getPosition().y);
                cards[i][j]->runAction(MoveTo::create(.25, Vec2(2, cards[i][j]->getPosition().y)));
            }else{
                cards[i][j] = cards[i-1][j];
            }
        }
    }
    
    refreshScoreBoard(score);
    return showNewNum;
}

bool HelloWorld::doUp(){
    bool showNewNum = false;
    int score = 0;

    for (int i=3; i>=0; i--) {
        for (int j=3; j>=1; j--) {
            if (cards[i][j]->getNumber() == 0 || (cards[i][j]->getNumber() == cards[i][j-1]->getNumber())) {
                if (cards[i][j]->getNumber() == cards[i][j-1]->getNumber()) {
                    score += cards[i][j]->getNumber();
                }
                cards[i][j]->setNumber(cards[i][j-1]->getNumber() + cards[i][j]->getNumber());
                cards[i][j-1]->setNumber(0);
                showNewNum = true;
            }
        }
    }
    
    CardSprite *tempCards[4];
    for (int i=0; i<4; i++) {
        for (int j=0; j<3; j++) {
            if (!moveLineFlag[i]) {
                break;
            }
            cards[i][j]->setNumber(cards[i][j+1]->getNumber());
        }
        //将要移动到最后的第一列缓存
        tempCards[i] = cards[i][3];
        //最右边一列数字归零
        if (moveLineFlag[i]) {
            cards[i][3]->setNumber(0);
        }
    }
    
    //最右边的一行将成为新卡片进入屏幕，随机取一个计算新生成的卡片数字，并设置值
    int num = arc4random() % 4;
    while (!moveLineFlag[num]) {
        num = arc4random() % 4;
    }
    cards[num][3]->setNumber((arc4random() % 2 + 1) * 2);
    
    for (int i=0; i<4; i++) {
        for (int j=3; j>=0; j--) {
            if (!moveLineFlag[i]) {
                break;
            }
            if (j==0) {
                cards[i][j] = tempCards[i];
                cards[i][j]->setPosition(cards[i][j]->getPosition().x, -cardsBg->getContentSize().width / 4);
                cards[i][j]->runAction(MoveTo::create(.25, Vec2(cards[i][j]->getPosition().x, 2)));
            }else{
                cards[i][j] = cards[i][j-1];
            }
        }
    }

    refreshScoreBoard(score);
    return showNewNum;
}

bool HelloWorld::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event){
    startPosition = touch->getLocation();
    return true;
}

void HelloWorld::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event){
    Point curLoc = touch->getLocation();
    float dx = curLoc.x - startPosition.x;
    float dy = curLoc.y - startPosition.y;
    float longestDistance = Director::getInstance()->getVisibleSize().width / 4;
    //判断这次滑动的方向
    if (swipeD == SwipeNone) {
        if (abs(dx) > abs(dy)) {
            if (dx > 0) {
                swipeD = SwipeRight;
                
            }else if (dx < 0){
                swipeD = SwipeLeft;
            }
            swipeDistance = dx;
        }else{
            if (dy > 0) {
                swipeD = SwipeUp;
            }else{
                swipeD = SwipeDown;
            }
            swipeDistance = dy;
        }
    }
    
    //本次滑动是否应该还原，即如果滑动方向向右，那么touch->getDelta().x > 0才算成功，否则滑动应该还原
    //移动卡片
    switch (swipeD) {
        case SwipeUp:
            swipeShouldCancel = touch->getDelta().y <= 0;
            if ((abs(dy) <= (longestDistance - 5)) && (dy >= 0)) {
                for (int i=0; i<4;i++) {
                    bool get = false;
                    for (int j=3; j>=1; j--) {
                        if ((cards[i][j]->getNumber() == cards[i][j-1]->getNumber() && cards[i][j]->getNumber() != 0) || (cards[i][j]->getNumber() == 0 && cards[i][j-1]->getNumber() != 0) || get) {
                            cards[i][j-1]->setLocalZOrder(10);
                            cards[i][j-1]->setPosition(cards[i][j-1]->getPosition().x, cards[i][j-1]->getPosition().y+touch->getDelta().y);
                            get = true;
                            moveLineFlag[i] = true;
                        }
                    }
                }
            }
            break;
        case SwipeDown:
            swipeShouldCancel = touch->getDelta().y >= 0;
            if ((abs(dy) <= (longestDistance - 5)) && (dy <= 0)) {
                for (int i=0; i<4; i++) {
                    bool get = false;
                    for (int j=0; j<3; j++) {
                        if ((cards[i][j]->getNumber() == cards[i][j+1]->getNumber() && cards[i][j]->getNumber() != 0) || (cards[i][j]->getNumber() == 0 && cards[i][j+1]->getNumber() != 0) || get) {
                            cards[i][j+1]->setLocalZOrder(10);
                            cards[i][j+1]->setPosition(cards[i][j+1]->getPosition().x, cards[i][j+1]->getPosition().y+touch->getDelta().y);
                            get = true;
                            moveLineFlag[i] = true;
                        }
                    }
                }
            }
            break;
        case SwipeLeft:
            swipeShouldCancel = touch->getDelta().x >= 0;
            if (abs(dx) <= longestDistance - 3 && dx <= 0) {
                for (int j=0; j<4; j++) {
                    bool get = false;
                    for (int i=0; i<3; i++) {
                        if ((cards[i][j]->getNumber() == cards[i+1][j]->getNumber() && cards[i][j]->getNumber() != 0) || (cards[i][j]->getNumber() == 0 && cards[i+1][j]->getNumber() != 0) || get) {
                            cards[i+1][j]->setLocalZOrder(10);
                            cards[i+1][j]->setPosition(cards[i+1][j]->getPosition().x+touch->getDelta().x, cards[i+1][j]->getPosition().y);
                            get = true;
                            moveLineFlag[j] = true;
                        }
                    }
                }
            }
            break;
        case SwipeRight:
            swipeShouldCancel = touch->getDelta().x <= 0;
            if (abs(dx) <= longestDistance && dx >= 0) {
                for (int j=0; j<4; j++) {
                    bool get = false;
                    for (int i=3; i>=1; i--) {
                        if ((cards[i][j]->getNumber() == cards[i-1][j]->getNumber() && cards[i][j]->getNumber() != 0) || (cards[i][j]->getNumber() == 0 && cards[i-1][j]->getNumber() != 0) || get) {
                            cards[i-1][j]->setLocalZOrder(10);
                            cards[i-1][j]->setPosition(cards[i-1][j]->getPosition().x+touch->getDelta().x, cards[i-1][j]->getPosition().y);
                            get = true;
                            moveLineFlag[j] = true;
                        }
                    }
                }
            }
            break;
        case SwipeNone:
            break;
    }
}

void HelloWorld::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event){
    endOrCancel();
}

void HelloWorld::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event){
    endOrCancel();
}

#pragma mark - private
void HelloWorld::endOrCancel(){
    if (swipeShouldCancel) {
        for (int i=0; i<4; i++) {
            for (int j=0; j<4; j++) {
                cards[i][j]->setPosition(cardsBg->getContentSize().width / 4 * i + CARD_BORDER / 2, cardsBg->getContentSize().width / 4 * j + 2);
                cards[i][j]->setLocalZOrder(1);
            }
        }
    }else{
        if (swipeDistance > 0) {
            for (int i=0; i<4; i++) {
                for (int j=0; j<4; j++) {
                    if (!moveLineFlag[i]) {
                        break;
                    }
                    if (swipeD == SwipeUp) {
                        cards[i][j]->setPosition(cardsBg->getContentSize().width / 4 * i + CARD_BORDER / 2, cardsBg->getContentSize().width / 4 * (j+1) + 2);
                        cards[i][j]->setLocalZOrder(1);
                    }else if (swipeD == SwipeRight) {
                        cards[j][i]->setPosition(cardsBg->getContentSize().width / 4 * (j+1) + CARD_BORDER / 2, cardsBg->getContentSize().width / 4 * i + 2);
                        cards[j][i]->setLocalZOrder(1);
                    }
                }
            }
        }else if (swipeDistance < 0){
            for (int i=0; i<4; i++) {
                for (int j=0; j<4; j++) {
                    if (!moveLineFlag[i]) {
                        break;
                    }
                    if (swipeD == SwipeDown) {
                        cards[i][j]->setPosition(cardsBg->getContentSize().width / 4 * i + CARD_BORDER / 2, cardsBg->getContentSize().width / 4 * (j-1) + 2);
                        cards[i][j]->setLocalZOrder(1);
                    }else if (swipeD == SwipeLeft){
                        cards[j][i]->setPosition(cardsBg->getContentSize().width / 4 * (j-1) + CARD_BORDER / 2, cardsBg->getContentSize().width / 4 * i + 2);
                        cards[j][i]->setLocalZOrder(1);
                    }
                }
            }
        }
        
        bool shouldMove = false;
        for (int i=0; i<4; i++) {
            if (moveLineFlag[i]) {
                shouldMove = moveLineFlag[i];
                break;
            }
        }
        if (shouldMove) {
            switch (swipeD) {
                case SwipeUp:
                    doUp();
                    break;
                case SwipeDown:
                    doDown();
                    break;
                case SwipeRight:
                    doRight();
                    break;
                case SwipeLeft:
                    doLeft();
                    break;
                case SwipeNone:
                    break;
            }
        }
    }
    swipeD = SwipeNone;
    for (int i=0; i<4; i++) {
        moveLineFlag[i] = false;
    }
}

void HelloWorld::refreshScoreBoard(int score){
    if (score == 0) {
        return;
    }
    curSb->setScore(curSb->getScore()+score);
    if (highestSb->getScore() < curSb->getScore()) {
        highestSb->setScore(curSb->getScore());
        UserDefault::getInstance()->setIntegerForKey("HighestScore", highestSb->getScore());
    }
}
