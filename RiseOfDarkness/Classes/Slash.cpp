#include "Slash.h"
#include "MainCharacter.h"
#include "ResourceManager.h"

Slash::Slash()
{
	mSprite = ResourceManager::GetInstance()->DuplicateSprite(ResourceManager::GetInstance()->GetSpriteById(14));
	mSprite->setVisible(false);
	mSprite->retain();
	mPhysicsBody = PhysicsBody::createBox(Size(mSprite->getContentSize().width, mSprite->getContentSize().height + 20), PhysicsMaterial(0, 0, 0));
	mPhysicsBody->setRotationEnable(false);
	mPhysicsBody->setCollisionBitmask(MainCharacter::SLASH_BITMASK);
	mPhysicsBody->setDynamic(false);
	mPhysicsBody->setContactTestBitmask(true);
	mPhysicsBody->setGravityEnable(false);
	mSprite->setPhysicsBody(mPhysicsBody);
}

Slash::~Slash() {}

void Slash::AddToLayer(Layer* layer)
{
	mSprite->removeFromParent();
	layer->addChild(mSprite, 1);

}

Sprite* Slash::GetSprite()
{
	return mSprite;
}

PhysicsBody* Slash::GetPhysicsBody()
{
	return mPhysicsBody;
}