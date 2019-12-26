#include "SpearMoblin.h"
#include "ResourceManager.h"
#include "MainCharacter.h"
#include <math.h>
#include "SimpleAudioEngine.h"

using namespace CocosDenshion;
using namespace std;

SpearMoblin::SpearMoblin(){}

SpearMoblin::SpearMoblin(Layer* layer, int direction, Vec2 pos, int group)
{
	mSprite = ResourceManager::GetInstance()->DuplicateSprite(ResourceManager::GetInstance()->GetSpriteById(15));
	mSprite->setScale(1.5);
	Size box;
	box.width = mSprite->getContentSize().width / 1.2;
	box.height = mSprite->getContentSize().height / 2.2;
	mPhysicsBody = PhysicsBody::createBox(box, PhysicsMaterial(0, 0, 0), Vec2(0, box.height / 6));
	mPhysicsBody->setRotationEnable(false);
	mPhysicsBody->setCollisionBitmask(MainCharacter::SPEARMOBLIN_BITMASK);
	mPhysicsBody->setContactTestBitmask(true);
	mPhysicsBody->setDynamic(false);
	mPhysicsBody->setGroup(group);
	mPhysicsBody->setGravityEnable(false);
	mSprite->setPhysicsBody(mPhysicsBody);
	layer->addChild(mSprite);

	pierce = new Pierce();
	pierce->AddToLayer(layer);

	hpBar = ResourceManager::GetInstance()->DuplicateSprite((ResourceManager::GetInstance()->GetSpriteById(12)));
	layer->addChild(hpBar);
	hpLoadingBar = ui::LoadingBar::create("res/loadingbar/color_hp.png");
	layer->addChild(hpLoadingBar);
	hpBar->setScale(0.2);
	hpLoadingBar->setScale(0.2);
	hpBar->retain();
	hpLoadingBar->retain();

	mAction[FRONT_IDLE] = ResourceManager::GetInstance()->GetActionById(22)->clone();
	mAction[BACK_IDLE] = ResourceManager::GetInstance()->GetActionById(23)->clone();
	mAction[LEFT_IDLE] = ResourceManager::GetInstance()->GetActionById(24)->clone();
	mAction[GO_DOWN] = ResourceManager::GetInstance()->GetActionById(26)->clone();
	mAction[GO_UP] = ResourceManager::GetInstance()->GetActionById(25)->clone();
	mAction[GO_LEFT] = ResourceManager::GetInstance()->GetActionById(27)->clone();
	mAction[FRONT_ATTACK] = ResourceManager::GetInstance()->GetActionById(28)->clone();
	mAction[BACK_ATTACK] = ResourceManager::GetInstance()->GetActionById(29)->clone();
	mAction[LEFT_ATTACK] = ResourceManager::GetInstance()->GetActionById(30)->clone();

	CC_SAFE_RETAIN(mAction[FRONT_IDLE]);
	CC_SAFE_RETAIN(mAction[BACK_IDLE]);
	CC_SAFE_RETAIN(mAction[LEFT_IDLE]);
	CC_SAFE_RETAIN(mAction[GO_DOWN]);
	CC_SAFE_RETAIN(mAction[GO_UP]);
	CC_SAFE_RETAIN(mAction[GO_LEFT]);
	CC_SAFE_RETAIN(mAction[FRONT_ATTACK]);
	CC_SAFE_RETAIN(mAction[BACK_ATTACK]);
	CC_SAFE_RETAIN(mAction[LEFT_ATTACK]);

	firstDirection = direction;
	this->direction = direction;
	this->pos = pos;
	currentState = FRONT_IDLE;

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

SpearMoblin::~SpearMoblin()
{
	mAction[FRONT_IDLE]->autorelease();
	mAction[BACK_IDLE]->autorelease();
	mAction[LEFT_IDLE]->autorelease();
	mAction[GO_DOWN]->autorelease();
	mAction[GO_UP]->autorelease();
	mAction[GO_LEFT]->autorelease();
	mAction[FRONT_ATTACK]->autorelease();
	mAction[BACK_ATTACK]->autorelease();
	mAction[LEFT_ATTACK]->autorelease();

	hpBar->autorelease();
	hpLoadingBar->autorelease();
}

void SpearMoblin::Update(float deltaTime)
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
			speed = 2;
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
		MainCharacter::GetInstance()->AddGold(MainCharacter::SPEARMOBLIN_GOLD);
		item->GetIcon()->setPosition(mSprite->getPosition());
		item->GetIcon()->setVisible(true);
		item->GetIcon()->getPhysicsBody()->setContactTestBitmask(true);
		SimpleAudioEngine::getInstance()->playEffect("audio/inventorySoundEffects/sellBuyItem.wav", false);
	}
}

void SpearMoblin::SetState(int nextState)
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
		case FRONT_IDLE:
		case BACK_IDLE:
		case LEFT_IDLE:
			currentState = nextState;
			mSprite->runAction(mAction[nextState]);
			break;
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

void SpearMoblin::Idle()
{
	if (mSprite->getNumberOfRunningActions() == 0)
	{
		pierce->GetSprite()->setPosition(Vec2(-1, -1));
		switch (direction)
		{
		case 1:
			SetState(BACK_IDLE);
			break;
		case 2:
			SetState(FRONT_IDLE);
			break;
		default:
			SetState(LEFT_IDLE);
		}
	}
}

void SpearMoblin::Run()
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
				SetState(GO_LEFT);
			}
		}
	}
}

void SpearMoblin::Attack()
{
	SimpleAudioEngine::getInstance()->playEffect("audio/spearMoblin/spear.mp3", false);
	switch (direction)
	{
	case 1:
		pierce->GetSprite()->setRotation(90);
		pierce->GetSprite()->setPosition(mSprite->getPositionX(), mSprite->getPositionY() + 15);
		SetState(BACK_ATTACK);
		break;
	case 2:
		pierce->GetSprite()->setRotation(90);
		pierce->GetSprite()->setPosition(mSprite->getPositionX(), mSprite->getPositionY() - 15);
		SetState(FRONT_ATTACK);
		break;
	case 3:
		pierce->GetSprite()->setRotation(0);
		pierce->GetSprite()->setPosition(mSprite->getPositionX() - 20, mSprite->getPositionY() - 10);
		SetState(LEFT_ATTACK);
		break;
	case 4:
		pierce->GetSprite()->setRotation(0);
		pierce->GetSprite()->setPosition(mSprite->getPositionX() + 20, mSprite->getPositionY() - 10);
		SetState(LEFT_ATTACK);
	}
}


