#include "Fire.h"
#include "MainCharacter.h"
#include "ResourceManager.h"

Fire::Fire(Sprite* sprite, int BITMASK)
{
	mSprite = sprite;
	//physics body
	mPhysicsBody = PhysicsBody::createBox(Size(mSprite->getContentSize()), PhysicsMaterial(1.0f, 0.0f, 1.0f));
	mPhysicsBody->setRotationEnable(false);
	mPhysicsBody->setGravityEnable(false);
	mPhysicsBody->setDynamic(false);
	mPhysicsBody->setCollisionBitmask(BITMASK);
	mPhysicsBody->setContactTestBitmask(true);
	mSprite->setPhysicsBody(mPhysicsBody);
	setAlive(false);

	mAction = ResourceManager::GetInstance()->GetActionById(56)->clone();
	CC_SAFE_RETAIN(mAction);
}

Fire::~Fire()
{
	mAction->autorelease();
}

void Fire::setAlive(bool alive)
{
	mIsAlive = alive;
}

void Fire::fly(bool isRight)
{

}

bool Fire::isAlive()
{
	return mIsAlive;
}


void Fire::update(float deltaTime)
{
	SetDistance(GetDistance() + RUN_STEP);
	SetPosition(GetPosition() + Vec2(0, -RUN_STEP));
	//out of range
	if (GetDistance() >= RANGE)
	{
		//setAlive(false);
		mSprite->setVisible(false);
		mSprite->setPosition(Vec2(-10, -10));
		SetDistance(0);
	}
}

void Fire::SetVisible(bool b)
{
	mSprite->setVisible(b);
	SetDistance(0);
	if (!b)
	{
		mSprite->setPosition(Vec2(-1, -1));
	}
}

bool Fire::IsVisible()
{
	return mSprite->isVisible();
}

void Fire::SetDistance(int s)
{
	distance = s;
}

int Fire::GetDistance()
{
	return distance;
}



Sprite * Fire::GetSprite()
{
	return mSprite;
}

void Fire::SetScale(float scaleX, float scaleY)
{
	mSprite->setScale(scaleX, scaleY);
}

void Fire::AddToScene(Layer *layer)
{
	mSprite->removeFromParent();
	layer->addChild(mSprite, 7);
}

Vec2 Fire::GetPosition()
{
	return mSprite->getPosition();
}

void Fire::SetPosition(Vec2 pos)
{
	mSprite->setPosition(pos);
}

void Fire::RunAction()
{
	if (mSprite->getNumberOfRunningActions() == 0)
	{
		mSprite->runAction(mAction);
	}
}

void Fire::StopAction()
{
	if (mSprite->getNumberOfRunningActions() != 0)
	{
		mSprite->stopAction(mAction);
	}
}
