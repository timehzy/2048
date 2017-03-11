#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
USING_NS_CC;

typedef enum : int {
    NewCardDirectionUp,
    NewCardDirectionRight,
    NewCardDirectionDown,
    NewCardDirectionLeft
} NewCardDirection;

typedef enum : int {
    SwipeUp,
    SwipeRight,
    SwipeDown,
    SwipeLeft,
    SwipeNone
} SwipeDirection;

class CardSprite;
class ScoreBoard;
class HelloWorld : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();
    virtual bool init()override;
    CREATE_FUNC(HelloWorld);
    bool onTouchBegan(Touch *touch, Event *event)override;
    void onTouchMoved(Touch *touch, Event *event)override;
    void onTouchEnded(Touch *touch, Event *event)override;
    void onTouchCancelled(Touch *touch, Event *event)override;
private:
    CardSprite *cards[4][4];
    ScoreBoard *curSb;
    ScoreBoard *highestSb;
    LayerColor *cardsBg;
    SwipeDirection swipeD;
    bool moveLineFlag[4] = {false, false, false, false};
    float swipeDistance;
    bool swipeShouldCancel;
    bool makeNewNumber = false;
    Point startPosition;
    bool doLeft();
    bool doDown();
    bool doRight();
    bool doUp();
    void refreshScoreBoard(int score);
    void endOrCancel();
    void restart(Ref *sender);
    void createCards();
};

#endif // __HELLOWORLD_SCENE_H__
