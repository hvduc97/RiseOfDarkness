#ifndef SLASH_H
#define SLASH_H

#include "cocos2d.h"

using namespace cocos2d;

class Slash
{
private:
	Sprite* mSprite;
	PhysicsBody* mPhysicsBody;
public:
	Slash();
	~Slash();

	void AddToLayer(Layer* layer);
	Sprite* GetSprite();
	PhysicsBody* GetPhysicsBody();
};

#endif