#include "FlySlash.h"
#include "MainCharacter.h"
#include "ResourceManager.h"

FlySlash::FlySlash()
{
	mSprite = ResourceManager::GetInstance()->DuplicateSprite(ResourceManager::GetInstance()->GetSpriteById(30));
	mSprite->setVisible(false);
	mSprite->setScale(0.5);
	mPhysicsBody = PhysicsBody::createBox(mSprite->getContentSize(), PhysicsMaterial(0, 0, 0));
	mPhysicsBody->setRotationEnable(false);
	mPhysicsBody->setCollisionBitmask(MainCharacter::FLY_SLASH_BITMASK);
	mPhysicsBody->setDynamic(false);
	mPhysicsBody->setContactTestBitmask(true);
	mPhysicsBody->setGravityEnable(false);
	mSprite->setPhysicsBody(mPhysicsBody);
	direction = 2;
}

void FlySlash::AddToLayer(Layer* layer)
{
	mSprite->removeFromParent();
	layer->addChild(mSprite, 4);
}

FlySlash::~FlySlash() {}

Sprite* FlySlash::GetSprite()
{
	return mSprite;
}

PhysicsBody* FlySlash::GetPhysicsBody()
{
	return mPhysicsBody;
}

void FlySlash::Update(float deltaTime)
{
	if (mSprite->isVisible())
	{
		switch (direction)
		{
		case 1:
			mSprite->setPosition(mSprite->getPositionX(), mSprite->getPositionY() + 7);
			break;
		case 2:
			mSprite->setPosition(mSprite->getPositionX(), mSprite->getPositionY() - 7);
			break;
		case 3:
			mSprite->setPosition(mSprite->getPositionX() - 7, mSprite->getPositionY());
			break;
		case 4:
			mSprite->setPosition(mSprite->getPositionX() + 7, mSprite->getPositionY());
			break;
		case 5:
			mSprite->setPosition(mSprite->getPositionX() - 4.949747, mSprite->getPositionY() + 4.949747);
			break;
		case 6:
			mSprite->setPosition(mSprite->getPositionX() - 4.949747, mSprite->getPositionY() - 4.949747);
			break;
		case 7:
			mSprite->setPosition(mSprite->getPositionX() + 4.949747, mSprite->getPositionY() + 4.949747);
			break;
		case 8:
			mSprite->setPosition(mSprite->getPositionX() + 4.949747, mSprite->getPositionY() - 4.949747);
			break;
		}
	}
}

void FlySlash::SetPosition()
{
	if (!mSprite->isVisible())
	{
		mSprite->setPosition(MainCharacter::GetInstance()->GetSprite()->getPosition());
		mSprite->setVisible(true);
		switch (direction)
		{
		case 1:
			mSprite->setRotation(-90);
			break;
		case 2:
			mSprite->setRotation(90);
			break;
		case 3:
			mSprite->setRotation(180);
			break;
		case 4:
			mSprite->setRotation(0);
			break;
		case 5:
			mSprite->setRotation(-135);
			break;
		case 6:
			mSprite->setRotation(135);
			break;
		case 7:
			mSprite->setRotation(-45);
			break;
		case 8:
			mSprite->setRotation(45);
			break;
		}
	}
}

void FlySlash::SetDirection(int direction)
{
	this->direction = direction;
}

void FlySlash::Disappear()
{
	mSprite->setVisible(false);
}