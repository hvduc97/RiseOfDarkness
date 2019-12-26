#ifndef _ARROW_H_
#define _ARROW_H_

#include "cocos2d.h"

class Arrow
{
private:
	
	const int RUN_STEP = 10;

	cocos2d::Sprite *mSprite;
	cocos2d::PhysicsBody* mPhysicsBody;
	bool mIsAlive;
	int direction;
	int distance;
	bool mIsMoveLeftToRight;
	bool mIsMoveBottomToTop;
public:
	static const int LEFT = 0;
	static const int RIGHT = 1;
	static const int UP = 2;
	static const int DOWN = 3;
	static const int TOPLEFT = 4;
	static const int TOPRIGHT = 5;
	static const int RIGHTBOT = 6;
	static const int LEFTBOT = 7;
	static const int RANGE = 250;
	Arrow(cocos2d::Sprite *sprite, int BITMASK);
	~Arrow();
	void setAlive(bool alive);
	void fly(bool isRight);
	bool isAlive();
	void update(float);
	void SetVisible(bool);
	bool IsVisible();
	void SetRotate(int angle);
	void SetFlipX(bool b);
	void SetDirection(int);
	void SetDistance(int);
	int GetDistance();
	int GetDirection();
	cocos2d::Sprite* GetSprite();
	void SetScale(float,float);
	void AddToScene(cocos2d::Layer*);
	cocos2d::Vec2 GetPosition();
	void SetPosition(cocos2d::Vec2 pos);
};
#endif