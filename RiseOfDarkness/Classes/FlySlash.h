#ifndef FLY_SLASH_H
#define FLY_SLASH_H

#include "cocos2d.h"

using namespace cocos2d;

class FlySlash
{
private:
	Sprite* mSprite;
	PhysicsBody* mPhysicsBody;
	int direction;
public:
	FlySlash();
	void AddToLayer(Layer* layer);
	~FlySlash();

	void Update(float deltaTime);
	void SetPosition();
	void SetDirection(int direction);
	Sprite* GetSprite();
	PhysicsBody* GetPhysicsBody();
	void Disappear();
};

#endif