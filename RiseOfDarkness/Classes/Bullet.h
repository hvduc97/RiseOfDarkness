#ifndef __BULLET_H__
#define __BULLET_H__

#include "cocos2d.h"

class Bullet
{
private:
	int RUN_STEP;
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

	static const int RANGE = 250;
	Bullet(cocos2d::Sprite *sprite, int BITMASK);
	~Bullet();
	void SetStep(int step);
	int GetStep();
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
	void SetScale(float, float);
	void AddToScene(cocos2d::Layer*);
	cocos2d::Vec2 GetPosition();
	void SetPosition(cocos2d::Vec2 pos);
};
#endif // !__BULLET_H__