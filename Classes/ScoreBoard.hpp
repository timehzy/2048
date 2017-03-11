//
//  ScoreBoard.hpp
//  towOfour8
//
//  Created by Michael-Nine on 2017/3/9.
//
//

#ifndef ScoreBoard_hpp
#define ScoreBoard_hpp

#include <stdio.h>
#include "cocos2d.h"
USING_NS_CC;

typedef enum : int {
    ScoreBoardTypeCurrent,
    ScoreBoardTypeHighest,
} ScoreBoardType;

class ScoreBoard : public Layer {
public:
    static ScoreBoard* createScoreBoard(ScoreBoardType type, int score, Size size, Point position);
    void setScore(int num);
    int getScore();
private:
    virtual bool init();
    CREATE_FUNC(ScoreBoard);
    void initScoreBoard(ScoreBoardType type, int score, Size size, Point position);
    LayerColor *background;
    Label *scoreLabel;
};
#endif /* ScoreBoard_hpp */
