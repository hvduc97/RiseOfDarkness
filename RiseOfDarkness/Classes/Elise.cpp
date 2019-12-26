#include "Elise.h"
#include "ResourceManager.h"
#include "MainCharacter.h"
#include <math.h>
#include "SimpleAudioEngine.h"

using namespace CocosDenshion;

Elise::Elise() {}

Elise::Elise(Layer* layer, int direction, Vec2 pos, int group)
{
	mSprite = ResourceManager::GetInstance()->DuplicateSprite(ResourceManager::GetInstance()->GetSpriteById(35));
	mSprite->setScale(1.0f);
	Size box;
	box.width = mSprite->getContentSize().width / 1.2;
	box.height = mSprite->getContentSize().height / 2.2;
	mPhysicsBody = PhysicsBody::createBox(box, PhysicsMaterial(0, 0, 0), Vec2(0, box.height / 6));
	mPhysicsBody->setRotationEnable(false);
	mPhysicsBody->setCollisionBitmask(MainCharacter::ELISE_MONSTER_BITMASK);
	mPhysicsBody->setContactTestBitmask(true);
	mPhysicsBody->setDynamic(false);
	mPhysicsBody->setGravityEnable(false);
	mSprite->setPhysicsBody(mPhysicsBody);
	layer->addChild(mSprite);


	hpBar = ResourceManager::GetInstance()->DuplicateSprite((ResourceManager::GetInstance()->GetSpriteById(12)));
	layer->addChild(hpBar);
	hpLoadingBar = ui::LoadingBar::create("res/loadingbar/color_hp.png");
	layer->addChild(hpLoadingBar);
	hpBar->setScale(0.2);
	hpLoadingBar->setScale(0.2);
	hpBar->retain();
	hpLoadingBar->retain();

	mAction[UP_ATTACK] = ResourceManager::GetInstance()->GetActionById(61)->clone();
	mAction[DOWN_ATTACK] = ResourceManager::GetInstance()->GetActionById(62)->clone();
	mAction[LEFT_ATTACK] = ResourceManager::GetInstance()->GetActionById(63)->clone();
	mAction[RIGHT_ATTACK] = ResourceManager::GetInstance()->GetActionById(64)->clone();

	mAction[GO_UP] = ResourceManager::GetInstance()->GetActionById(57)->clone();
	mAction[GO_DOWN] = ResourceManager::GetInstance()->GetActionById(58)->clone();
	mAction[GO_LEFT] = ResourceManager::GetInstance()->GetActionById(59)->clone();
	mAction[GO_RIGHT] = ResourceManager::GetInstance()->GetActionById(60)->clone();

	CC_SAFE_RETAIN(mAction[UP_ATTACK]);
	CC_SAFE_RETAIN(mAction[DOWN_ATTACK]);
	CC_SAFE_RETAIN(mAction[LEFT_ATTACK]);
	CC_SAFE_RETAIN(mAction[RIGHT_ATTACK]);

	CC_SAFE_RETAIN(mAction[GO_UP]);
	CC_SAFE_RETAIN(mAction[GO_DOWN]);
	CC_SAFE_RETAIN(mAction[GO_LEFT]);
	CC_SAFE_RETAIN(mAction[GO_RIGHT]);

	firstDirection = direction;
	this->direction = direction;
	this->pos = pos;
	currentState = DOWN_ATTACK;

	mSprite->setPosition(pos);

	maxHP = HP;
	currentHP = maxHP;
	speed = 1;

	countingTime = 0;
	coolDownAttack = 0;
	preventRun = 0;

	auto sprite = ResourceManager::GetInstance()->DuplicateSprite(ResourceManager::GetInstance()->GetSpriteById(38));
	sprite->setScale(0.7f);
	bullet = new Bullet(sprite, MainCharacter::BULLET_BITMASK);
	bullet->SetStep(7);
	bullet->SetVisible(false);
	bullet->GetSprite()->getPhysicsBody()->setGroup(group);
	bullet->SetDistance(0);
	layer->addChild(sprite, 7);

	item = new Item(MainCharacter::GetInstance()->GetInventory()->database->items[MainCharacter::GetInstance()->GetInventory()->GetIndexByID(25)]);
	item->GetIcon()->setVisible(false);
	auto itemPhysics = PhysicsBody::createBox(item->GetIcon()->getContentSize(), PhysicsMaterial(0, 0, 0));
	itemPhysics->setRotationEnable(false);
	itemPhysics->setGravityEnable(false);
	itemPhysics->setDynamic(false);
	itemPhysics->setGroup(group);
	itemPhysics->setContactTestBitmask(false);
	itemPhysics->setCollisionBitmask(MainCharacter::MEAT_ITEM_BITMASK);
	item->GetIcon()->setPhysicsBody(itemPhysics);
	layer->addChild(item->GetIcon(), 7);
}

Elise::~Elise()
{
	mAction[UP_ATTACK]->autorelease();
	mAction[DOWN_ATTACK]->autorelease();
	mAction[LEFT_ATTACK]->autorelease();
	mAction[RIGHT_ATTACK]->autorelease();

	mAction[GO_DOWN]->autorelease();
	mAction[GO_UP]->autorelease();
	mAction[GO_LEFT]->autorelease();
	mAction[GO_RIGHT]->autorelease();

	hpBar->autorelease();
	hpLoadingBar->autorelease();
}

void Elise::Update(float deltaTime)
{
	if (IsAlive())
	{
		hpLoadingBar->setPercent(GetPercentHP());
		Idle();
		if (Target(ATTACK_RANGE))
		{
			if (coolDownAttack >= ATTACK_COOLDOWN)
			{
				coolDownAttack = 0;
				Attack();
			}
		}
		else if (Detect(DETECT_RANGE))
		{
			speed = 3;
			Run();
		}
		else
		{
			speed = 1;
			Run();
		}
		coolDownAttack += deltaTime;
		countingTime += deltaTime;
		if (countingTime >= REVIVE_TIME)
		{
			AutoRevive(HP_REVIVE);
		}
		if (bullet->IsVisible())
		{
			bullet->update(deltaTime);
		}
	}
	else
	{
		MainCharacter::GetInstance()->AddGold(MainCharacter::ELISE_MONSTER_GOLD);
		bullet->SetVisible(false);
		item->GetIcon()->setPosition(mSprite->getPosition());
		item->GetIcon()->setVisible(true);
		item->GetIcon()->getPhysicsBody()->setContactTestBitmask(true);
		SimpleAudioEngine::getInstance()->playEffect("audio/inventorySoundEffects/sellBuyItem.wav", false);
	}

}

void Elise::SetState(int nextState)
{
	if (currentState == nextState && mSprite->getNumberOfRunningActions() == 0)
	{
		mSprite->runAction(mAction[nextState]);
	}
	else if (currentState == nextState)
	{
		return;
	}
	else
	{
		switch (nextState)
		{
		case GO_UP:
		case GO_DOWN:
		case GO_LEFT:
			currentState = nextState;
			mSprite->stopAllActions();
			mSprite->runAction(mAction[nextState]);
			break;
		case GO_RIGHT:
			currentState = nextState;
			mSprite->stopAllActions();
			mSprite->runAction(mAction[nextState]);
			break;
		case DOWN_ATTACK:
		case UP_ATTACK:
		case LEFT_ATTACK:
			currentState = nextState;
			mSprite->stopAllActions();
			mSprite->runAction(mAction[nextState]);
			break;
		case RIGHT_ATTACK:
			currentState = nextState;
			mSprite->stopAllActions();
			mSprite->runAction(mAction[nextState]);
			break;
		default:
			break;
		}
	}
}

void Elise::Idle()
{
	if (mSprite->getNumberOfRunningActions() == 0)
	{

		switch (direction)
		{
		case 1:
			SetState(UP_ATTACK);
			break;
		case 2:
			SetState(DOWN_ATTACK);
			break;
		case 3:
			SetState(LEFT_ATTACK);
			break;
		case 4:
			SetState(RIGHT_ATTACK);
			break;
		}
	}
}

void Elise::Run()
{
	if (currentState != RIGHT_ATTACK || currentState != LEFT_ATTACK || currentState != DOWN_ATTACK || currentState != UP_ATTACK)
	{
		hpBar->setPosition(Vec2(mSprite->getPositionX(), mSprite->getPositionY() + 20));
		hpLoadingBar->setPosition(Vec2(mSprite->getPositionX(), mSprite->getPositionY() + 20));
		switch (direction)
		{
		case 1:
			if (preventRun != 1)
			{
				mSprite->setPositionY(mSprite->getPositionY() + speed);
				SetState(GO_UP);
			}
			break;
		case 2:
			if (preventRun != 2)
			{
				mSprite->setPositionY(mSprite->getPositionY() - speed);
				SetState(GO_DOWN);
			}
			break;
		case 3:
			if (preventRun != 3)
			{
				mSprite->setPositionX(mSprite->getPositionX() - speed);
				SetState(GO_LEFT);
			}
			break;
		case 4:
			if (preventRun != 4)
			{
				mSprite->setPositionX(mSprite->getPositionX() + speed);
				SetState(GO_RIGHT);
			}
		}
	}
}

void Elise::Attack()
{
	if (!bullet->IsVisible())
	{
		bullet->SetRotate(180);
		bullet->SetPosition(mSprite->getPosition());
		bullet->SetVisible(true);
		switch (direction)
		{
		case 1:
			bullet->SetDirection(2);
			SetState(UP_ATTACK);
			break;
		case 2:
			bullet->SetDirection(3);
			SetState(DOWN_ATTACK);
			break;
		case 3:
			bullet->SetDirection(0);
			SetState(LEFT_ATTACK);
			break;
		case 4:
			bullet->SetDirection(1);
			SetState(RIGHT_ATTACK);
		}
	}
}

Bullet* Elise::GetBullet()
{
	return bullet;
}

bool Elise::Detect(float detectRange)
{
	auto mcPos = MainCharacter::GetInstance()->GetSprite()->getPosition();
	auto ePos = mSprite->getPosition();

	float dis = std::sqrt((mcPos.x - ePos.x)*(mcPos.x - ePos.x) + (mcPos.y - ePos.y)*(mcPos.y - ePos.y));

	if (dis <= detectRange)
	{
		if (countingTime > 1)
		{
			countingTime = 0;
			if (std::abs(mcPos.x - ePos.x) > std::abs(mcPos.y - ePos.y))
			{
				if (mcPos.y < ePos.y)
				{
					SetDirection(2);
				}
				else
				{
					SetDirection(1);
				}
			}
			else
			{
				if (mcPos.x < ePos.x)
				{
					SetDirection(3);
				}
				else
				{
					SetDirection(4);
				}
			}
		}
		return true;
	}
	return false;
}

void Elise::SetDirection(int dir)
{
	if (direction != dir)
	{
		direction = dir;
	}
	if (preventRun != dir)
	{
		preventRun = 0;
	}
}