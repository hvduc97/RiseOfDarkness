#include "Bullet.h"
#include "MainCharacter.h"
USING_NS_CC;

Bullet::Bullet(Sprite* sprite, int BITMASK)
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
}

Bullet::~Bullet()
{
}

void Bullet::SetStep(int step)
{
	this->RUN_STEP = step;
}

int Bullet::GetStep()
{
	return this->RUN_STEP;
}

void Bullet::setAlive(bool alive)
{
	mIsAlive = alive;
}

void Bullet::fly(bool isRight)
{

}

bool Bullet::isAlive()
{
	return mIsAlive;
}


void Bullet::update(float deltaTime)
{
	SetDistance(GetDistance() + RUN_STEP);
	int direction = this->GetDirection();
	if (direction == LEFT)
	{
		SetPosition(GetPosition() + Vec2(-RUN_STEP, 0));
	}
	else
	{
		if (direction == UP)
		{
			//SetRotate(90);
			SetPosition(GetPosition() + Vec2(0, RUN_STEP));
		}
		else
		{
			if (direction == DOWN)
			{
				//SetRotate(360 - 90);
				SetPosition(GetPosition() + Vec2(0, -RUN_STEP));
			}
			else
			{
				//SetFlipX(true);
				SetPosition(GetPosition() + Vec2(+RUN_STEP, 0));
			}
		}
	}

	//out of range
	if (GetDistance() >= RANGE)
	{
		//setAlive(false);
		mSprite->setVisible(false);
		mSprite->setPosition(Vec2(-10, -10));
		SetDistance(0);
	}
}

void Bullet::SetVisible(bool b)
{
	mSprite->setVisible(b);
	SetDistance(0);
	if (!b)
	{
		mSprite->setPosition(Vec2(-1, -1));
	}
}

bool Bullet::IsVisible()
{
	return mSprite->isVisible();
}

void Bullet::SetRotate(int angle)
{
	mSprite->setRotation(angle);
}

void Bullet::SetFlipX(bool b)
{
	mSprite->setFlipX(b);
}

void Bullet::SetDirection(int d)
{
	direction = d;
	switch (d)
	{
	case UP:
		SetRotate(90);
		break;
	case DOWN:
		SetRotate(360 - 90);
		break;
	case RIGHT:
		SetRotate(180);
		break;
	default:
		SetRotate(0);
		break;
	}
}

void Bullet::SetDistance(int s)
{
	distance = s;
}

int Bullet::GetDistance()
{
	return distance;
}

int Bullet::GetDirection()
{
	return direction;
}

cocos2d::Sprite * Bullet::GetSprite()
{
	return mSprite;
}

void Bullet::SetScale(float scaleX, float scaleY)
{
	mSprite->setScale(scaleX, scaleY);
}

void Bullet::AddToScene(Layer *layer)
{
	mSprite->removeFromParent();
	layer->addChild(mSprite, 7);
}

cocos2d::Vec2 Bullet::GetPosition()
{
	return mSprite->getPosition();
}

void Bullet::SetPosition(Vec2 pos)
{
	mSprite->setPosition(pos);
}
