#ifndef PIERCE_H
#define PIERCE_H

#include "cocos2d.h"

using namespace cocos2d;

class Pierce
{
private:
	Sprite* mSprite;
	PhysicsBody* mPhysicsBody;
public:
	Pierce();
	void AddToLayer(Layer* layer);
	~Pierce();

	Sprite* GetSprite();
	PhysicsBody* GetPhysicsBody();
};

#endif