#include "Maokai.h"
#include "ResourceManager.h"
#include "MainCharacter.h"
#include <math.h>
#include "SimpleAudioEngine.h"

using namespace CocosDenshion;
Maokai::Maokai()
{
}

Maokai::Maokai(Layer * layer, Vec2 pos, int group)
{
	mSprite = ResourceManager::GetInstance()->DuplicateSprite(ResourceManager::GetInstance()->GetSpriteById(34));
	mSprite->setScale(0.5f);
	Size box;
	box.width = mSprite->getContentSize().width / 2.5f;
	box.height = mSprite->getContentSize().height / 2.0f;
	mPhysicsBody = PhysicsBody::createBox(box, PhysicsMaterial(0, 0, 0), Vec2(0, - box.height / 6));
	mPhysicsBody->setRotationEnable(false);
	mPhysicsBody->setCollisionBitmask(MainCharacter::MAOKAI_MONSTER_BITMASK);
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

	hpBar->setVisible(false);
	hpLoadingBar->setVisible(false);

	mAction[IDLE] = ResourceManager::GetInstance()->GetActionById(54)->clone();
	mAction[ATTACK] = ResourceManager::GetInstance()->GetActionById(55)->clone();

	CC_SAFE_RETAIN(mAction[IDLE]);
	CC_SAFE_RETAIN(mAction[ATTACK]);

	this->pos = pos;
	currentState = IDLE;

	mSprite->setPosition(pos);

	maxHP = HP;
	currentHP = maxHP;

	countingTime = 0;
	coolDownAttack = 0;

	auto sprite = ResourceManager::GetInstance()->DuplicateSprite(ResourceManager::GetInstance()->GetSpriteById(37));
	sprite->setScale(0.4f);
	fire = new Fire(sprite, MainCharacter::FIRE_BITMASK);
	fire->SetVisible(false);
	fire->GetSprite()->getPhysicsBody()->setGroup(group);
	fire->SetDistance(0);
	layer->addChild(sprite, 7);

	item = new Item(MainCharacter::GetInstance()->GetInventory()->database->items[MainCharacter::GetInstance()->GetInventory()->GetIndexByID(23)]);
	item->GetIcon()->setVisible(false);
	auto itemPhysics = PhysicsBody::createBox(item->GetIcon()->getContentSize(), PhysicsMaterial(0, 0, 0));
	itemPhysics->setRotationEnable(false);
	itemPhysics->setGravityEnable(false);
	itemPhysics->setDynamic(false);
	itemPhysics->setGroup(group);
	itemPhysics->setContactTestBitmask(false);
	itemPhysics->setCollisionBitmask(MainCharacter::APPLE_ITEM_BITMASK);
	item->GetIcon()->setPhysicsBody(itemPhysics);
	layer->addChild(item->GetIcon(), 7);
}

Maokai::~Maokai()
{
	mAction[IDLE]->autorelease();
	mAction[ATTACK]->autorelease();

	hpBar->autorelease();
	hpLoadingBar->autorelease();
}

void Maokai::Update(float deltaTime)
{
	if (IsAlive())
	{
		hpBar->setPosition(Vec2(mSprite->getPositionX(), mSprite->getPositionY() + 20));
		hpLoadingBar->setPosition(Vec2(mSprite->getPositionX(), mSprite->getPositionY() + 20));

		hpLoadingBar->setPercent(GetPercentHP());
		Idle();
		if (Detect(DETECT_RANGE))
		{
			if (coolDownAttack >= ATTACK_COOLDOWN)
			{
				coolDownAttack = 0;
				Attack();
			}
		}
		coolDownAttack += deltaTime;
		countingTime += deltaTime;
		if (countingTime >= REVIVE_TIME)
		{
			AutoRevive(HP_REVIVE);
		}
		if (fire->IsVisible())
		{
			fire->RunAction();
			fire->update(deltaTime);
		}
		else
		{
			fire->StopAction();
		}
	}
	else
	{
		MainCharacter::GetInstance()->AddGold(MainCharacter::MAOKAI_MONSTER_GOLD);
		fire->SetVisible(false);
		item->GetIcon()->setPosition(mSprite->getPosition());
		item->GetIcon()->setVisible(true);
		item->GetIcon()->getPhysicsBody()->setContactTestBitmask(true);
		SimpleAudioEngine::getInstance()->playEffect("audio/inventorySoundEffects/sellBuyItem.wav", false);
	}
}

void Maokai::SetState(int nextState)
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
		case IDLE:
			currentState = nextState;
			mSprite->runAction(mAction[nextState]);
			break;
		case ATTACK:
			currentState = nextState;
			mSprite->stopAllActions();
			mSprite->runAction(mAction[nextState]);
			break;
		default:
			break;
		}
	}
}

void Maokai::Idle()
{
	if (mSprite->getNumberOfRunningActions() == 0)
	{
		SetState(IDLE);
	}
}

void Maokai::Attack()
{
	SimpleAudioEngine::getInstance()->playEffect("audio/maokai/fire.wav", false);
	if (!fire->IsVisible())
	{
		fire->SetPosition(mSprite->getPosition());
		fire->SetVisible(true);
		SetState(ATTACK);
	}
}

Fire * Maokai::GetFire()
{
	return fire;
}

bool Maokai::Detect(float detectRange)
{
	auto mcPos = MainCharacter::GetInstance()->GetSprite()->getPosition();
	auto ePos = mSprite->getPosition();

	float dis = std::sqrt((mcPos.x - ePos.x)*(mcPos.x - ePos.x) + (mcPos.y - ePos.y)*(mcPos.y - ePos.y));

	if (dis <= detectRange)
	{
		hpBar->setVisible(true);
		hpLoadingBar->setVisible(true);
		return true;
	}

	return false;
}
