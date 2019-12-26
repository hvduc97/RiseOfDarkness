#ifndef _FIRE_H_
#define _FIRE_H_

#include "cocos2d.h"

USING_NS_CC;

class Fire
{
private:
	const int RUN_STEP = 2;

	Sprite *mSprite;
	PhysicsBody* mPhysicsBody;
	Action *mAction;
	bool mIsAlive;
	int distance;
public:
	static const int RANGE = 100;
	Fire(Sprite *sprite, int BITMASK);
	~Fire();
	void setAlive(bool alive);
	void fly(bool isRight);
	bool isAlive();
	void update(float);
	void SetVisible(bool);
	bool IsVisible();
	void SetDistance(int);
	int GetDistance();
	Sprite* GetSprite();
	void SetScale(float, float);
	void AddToScene(cocos2d::Layer*);
	cocos2d::Vec2 GetPosition();
	void SetPosition(cocos2d::Vec2 pos);
	void RunAction();
	void StopAction();
};
#endif