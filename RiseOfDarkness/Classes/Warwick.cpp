#include "Warwick.h"
#include "ResourceManager.h"
#include "MainCharacter.h"
#include <math.h>
#include "SimpleAudioEngine.h"

using namespace CocosDenshion;

Warwick::Warwick() {}

Warwick::Warwick(Layer* layer, int direction, Vec2 pos, int group)
{
	mSprite = ResourceManager::GetInstance()->DuplicateSprite(ResourceManager::GetInstance()->GetSpriteById(36));
	mSprite->setScale(1.0f);
	Size box;
	box.width = mSprite->getContentSize().width / 1.2f;
	box.height = mSprite->getContentSize().height / 1.6f;
	mPhysicsBody = PhysicsBody::createBox(box, PhysicsMaterial(0, 0, 0), Vec2(0, -5));
	mPhysicsBody->setRotationEnable(false);
	mPhysicsBody->setCollisionBitmask(MainCharacter::WARWICK_MONSTER_BITMASK);
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

	mAction[GO_UP] = ResourceManager::GetInstance()->GetActionById(65)->clone();
	mAction[GO_DOWN] = ResourceManager::GetInstance()->GetActionById(66)->clone();
	mAction[GO_LEFT] = ResourceManager::GetInstance()->GetActionById(67)->clone();
	mAction[BACK_ATTACK] = ResourceManager::GetInstance()->GetActionById(68)->clone();
	mAction[FRONT_ATTACK] = ResourceManager::GetInstance()->GetActionById(69)->clone();
	mAction[LEFT_ATTACK] = ResourceManager::GetInstance()->GetActionById(70)->clone();

	CC_SAFE_RETAIN(mAction[GO_DOWN]);
	CC_SAFE_RETAIN(mAction[GO_UP]);
	CC_SAFE_RETAIN(mAction[GO_LEFT]);
	CC_SAFE_RETAIN(mAction[FRONT_ATTACK]);
	CC_SAFE_RETAIN(mAction[BACK_ATTACK]);
	CC_SAFE_RETAIN(mAction[LEFT_ATTACK]);

	firstDirection = direction;
	this->direction = direction;
	this->pos = pos;
	currentState = FRONT_ATTACK;

	mSprite->setPosition(pos);

	maxHP = HP;
	currentHP = maxHP;
	speed = 1;

	countingTime = 0;
	coolDownAttack = 0;
	preventRun = 0;

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

Warwick::~Warwick()
{
	mAction[GO_DOWN]->autorelease();
	mAction[GO_UP]->autorelease();
	mAction[GO_LEFT]->autorelease();
	mAction[FRONT_ATTACK]->autorelease();
	mAction[BACK_ATTACK]->autorelease();
	mAction[LEFT_ATTACK]->autorelease();

	hpBar->autorelease();
	hpLoadingBar->autorelease();
}

void Warwick::Update(float deltaTime)
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
	}
	else
	{
		MainCharacter::GetInstance()->AddGold(MainCharacter::WARWICK_MONSTER_GOLD);
		item->GetIcon()->setPosition(mSprite->getPosition());
		item->GetIcon()->setVisible(true);
		item->GetIcon()->getPhysicsBody()->setContactTestBitmask(true);
		SimpleAudioEngine::getInstance()->playEffect("audio/inventorySoundEffects/sellBuyItem.wav", false);
	}
}

void Warwick::SetState(int nextState)
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
		case FRONT_ATTACK:
		case BACK_ATTACK:
		case LEFT_ATTACK:
			currentState = nextState;
			mSprite->stopAllActions();
			mSprite->runAction(mAction[nextState]);
		}
	}
}

void Warwick::Idle()
{
	if (mSprite->getNumberOfRunningActions() == 0)
	{
		switch (direction)
		{
		case 1:
			SetState(BACK_ATTACK);
			break;
		case 2:
			SetState(FRONT_ATTACK);
			break;
		default:
			SetState(LEFT_ATTACK);
		}
	}
}

void Warwick::Run()
{
	if (currentState != LEFT_ATTACK || currentState != FRONT_ATTACK || currentState != BACK_ATTACK)
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
				mPhysicsBody->setPositionOffset(Vec2(-1, 0));
			}
			break;
		case 2:
			if (preventRun != 2)
			{
				mSprite->setPositionY(mSprite->getPositionY() - speed);
				SetState(GO_DOWN);
				mPhysicsBody->setPositionOffset(Vec2(-1, 0));
			}
			break;
		case 3:
			if (preventRun != 3)
			{
				mSprite->setPositionX(mSprite->getPositionX() - speed);
				SetState(GO_LEFT);
				mPhysicsBody->setPositionOffset(Vec2(0, -4));
			}
			break;
		case 4:
			if (preventRun != 4)
			{
				mSprite->setPositionX(mSprite->getPositionX() + speed);
				SetState(GO_LEFT);
				mPhysicsBody->setPositionOffset(Vec2(16, -4));
			}
		}
	}
}

void Warwick::Attack()
{
	switch (direction)
	{
	case 1:
		mSprite->setPositionY(mSprite->getPositionY() - 15);
		SetState(BACK_ATTACK);
		break;
	case 2:
		mSprite->setPositionY(mSprite->getPositionY() + 15);
		SetState(FRONT_ATTACK);
		break;
	case 3:
		mSprite->setPositionX(mSprite->getPositionX() + 15);
		SetState(LEFT_ATTACK);
		break;
	case 4:
		mSprite->setPositionX(mSprite->getPositionX() - 15);
		SetState(LEFT_ATTACK);
	}
}


