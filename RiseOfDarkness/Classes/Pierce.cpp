#include "Pierce.h"
#include "MainCharacter.h"
#include "ResourceManager.h"

Pierce::Pierce()
{
	mSprite = ResourceManager::GetInstance()->DuplicateSprite(ResourceManager::GetInstance()->GetSpriteById(14));
	mSprite->setVisible(false);
	mPhysicsBody = PhysicsBody::createBox(mSprite->getContentSize(), PhysicsMaterial(0, 0, 0));
	mPhysicsBody->setRotationEnable(false);
	mPhysicsBody->setCollisionBitmask(MainCharacter::PIERCE_BITMASK);
	mPhysicsBody->setDynamic(false);
	mPhysicsBody->setContactTestBitmask(true);
	mPhysicsBody->setGravityEnable(false);
	mSprite->setPhysicsBody(mPhysicsBody);
}

void Pierce::AddToLayer(Layer* layer)
{
	layer->addChild(mSprite, 1);
}

Pierce::~Pierce() {}

Sprite* Pierce::GetSprite()
{
	return mSprite;
}

PhysicsBody* Pierce::GetPhysicsBody()
{
	return mPhysicsBody;
}