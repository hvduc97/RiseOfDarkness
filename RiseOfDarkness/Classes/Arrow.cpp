#include "Arrow.h"
#include "MainCharacter.h"
USING_NS_CC;

Arrow::Arrow(Sprite* sprite, int BITMASK)
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

Arrow::~Arrow()
{
}

void Arrow::setAlive(bool alive)
{
	mIsAlive = alive;
}

void Arrow::fly(bool isRight)
{
	
}

bool Arrow::isAlive()
{
	return mIsAlive;
}


void Arrow::update(float deltaTime)
{
	SetDistance(GetDistance() + RUN_STEP);
	int direction = this->GetDirection();
	switch (direction)
	{
	case LEFT:
		SetPosition(GetPosition() + Vec2(-RUN_STEP, 0));
		break;
	case RIGHT:
		SetPosition(GetPosition() + Vec2(+RUN_STEP, 0));
		break;
	case UP:
		SetPosition(GetPosition() + Vec2(0, RUN_STEP));
		break;
	case DOWN:
		SetPosition(GetPosition() + Vec2(0, -RUN_STEP));
		break;
	case TOPLEFT:
		SetPosition(GetPosition() + Vec2(-RUN_STEP, RUN_STEP));
		break;
	case TOPRIGHT:
		SetPosition(GetPosition() + Vec2(RUN_STEP, RUN_STEP));
		break;
	case RIGHTBOT:
		SetPosition(GetPosition() + Vec2(RUN_STEP, -RUN_STEP));
		break;
	case LEFTBOT:
		SetPosition(GetPosition() + Vec2(-RUN_STEP, -RUN_STEP));
		break;
	default:
		break;
	}
	//out of range
	if (GetDistance()>=MainCharacter::GetInstance()->GetArrowRange())
	{
		//setAlive(false);
		mSprite->setVisible(false);
		mSprite->setPosition(Vec2(-10, -10));
		SetDistance(0);
	}
}

void Arrow::SetVisible(bool b)
{
	mSprite->setVisible(b);
	SetDistance(0);
	if (!b)
	{
		mSprite->setPosition(Vec2(-1, -1));
	}
}

bool Arrow::IsVisible()
{
	return mSprite->isVisible();
}

void Arrow::SetRotate(int angle)
{
	mSprite->setRotation(angle);
}

void Arrow::SetFlipX(bool b)
{
	mSprite->setFlipX(b);
}

void Arrow::SetDirection(int d)
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
	case TOPLEFT:
		SetRotate(45);
		break;
	case TOPRIGHT:
		SetRotate(45 + 90);
		break;
	case RIGHTBOT:
		SetRotate(180 + 45);
		break;
	case LEFTBOT:
		SetRotate(360 - 45);
		break;
	default:
		SetRotate(0);
		break;
	}
}

void Arrow::SetDistance(int s)
{
	distance = s;
}

int Arrow::GetDistance()
{
	return distance;
}

int Arrow::GetDirection()
{
	return direction;
}

cocos2d::Sprite * Arrow::GetSprite()
{
	return mSprite;
}

void Arrow::SetScale(float scaleX,float scaleY)
{
	mSprite->setScale(scaleX,scaleY);
}

void Arrow::AddToScene(Layer *layer)
{
	mSprite->removeFromParent();
	layer->addChild(mSprite, 7);
}

cocos2d::Vec2 Arrow::GetPosition()
{
	return mSprite->getPosition();
}

void Arrow::SetPosition(Vec2 pos)
{
	mSprite->setPosition(pos);
}
