#include "Nokken.h"
#include "ResourceManager.h"
#include "MainCharacter.h"
#include <math.h>
#include "SimpleAudioEngine.h"

using namespace CocosDenshion;

Nokken::Nokken()
{
}

Nokken::Nokken(Layer * layer, Vec2 pos, int group)
{
	mSprite = ResourceManager::GetInstance()->DuplicateSprite(ResourceManager::GetInstance()->GetSpriteById(31));
	mSprite->setScale(1.5f);
	Size box;
	box.width = mSprite->getContentSize().width / 1.2;
	box.height = mSprite->getContentSize().height / 2.2;
	mPhysicsBody = PhysicsBody::createBox(box, PhysicsMaterial(0, 0, 0), Vec2(0, box.height / 6));
	mPhysicsBody->setRotationEnable(false);
	mPhysicsBody->setCollisionBitmask(MainCharacter::NOKKEN_MONSTER_BITMASK);
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
	
	mAction[IDLE] = ResourceManager::GetInstance()->GetActionById(52)->clone();
	mAction[ATTACK] = ResourceManager::GetInstance()->GetActionById(53)->clone();

	CC_SAFE_RETAIN(mAction[IDLE]);
	CC_SAFE_RETAIN(mAction[ATTACK]);

	this->pos = pos;
	currentState = IDLE;

	mSprite->setPosition(pos);

	maxHP = HP;
	currentHP = maxHP;

	countingTime = 0;
	coolDownAttack = 0;

	auto sprite = ResourceManager::GetInstance()->DuplicateSprite(ResourceManager::GetInstance()->GetSpriteById(32));
	sprite->setScale(0.5f);
	bullet = new Bullet(sprite, MainCharacter::BULLET_BITMASK);
	bullet->SetStep(6);
	bullet->SetVisible(false);
	bullet->GetSprite()->getPhysicsBody()->setGroup(group);
	bullet->SetDirection(3);
	bullet->SetRotate(0);
	bullet->SetDistance(0);
	layer->addChild(sprite, 7);

	item = new Item(MainCharacter::GetInstance()->GetInventory()->database->items[MainCharacter::GetInstance()->GetInventory()->GetIndexByID(24)]);
	item->GetIcon()->setVisible(false);
	auto itemPhysics = PhysicsBody::createBox(item->GetIcon()->getContentSize(), PhysicsMaterial(0, 0, 0));
	itemPhysics->setRotationEnable(false);
	itemPhysics->setGravityEnable(false);
	itemPhysics->setDynamic(false);
	itemPhysics->setGroup(group);
	itemPhysics->setContactTestBitmask(false);
	itemPhysics->setCollisionBitmask(MainCharacter::MUSHROOM_ITEM_BITMASK);
	item->GetIcon()->setPhysicsBody(itemPhysics);
	layer->addChild(item->GetIcon(), 7);
}

Nokken::~Nokken()
{
	mAction[IDLE]->autorelease();
	mAction[ATTACK]->autorelease();

	hpBar->autorelease();
	hpLoadingBar->autorelease();
}

void Nokken::Update(float deltaTime)
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
		if (bullet->IsVisible())
		{
			bullet->update(deltaTime);
		}
	}
	else
	{
		MainCharacter::GetInstance()->AddGold(MainCharacter::NOKKEN_MONSTER_GOLD);
		bullet->SetVisible(false);
		item->GetIcon()->setPosition(mSprite->getPosition());
		item->GetIcon()->setVisible(true);
		item->GetIcon()->getPhysicsBody()->setContactTestBitmask(true);
		SimpleAudioEngine::getInstance()->playEffect("audio/inventorySoundEffects/sellBuyItem.wav", false);
	}
}

void Nokken::SetState(int nextState)
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

void Nokken::Idle()
{
	if (mSprite->getNumberOfRunningActions() == 0)
	{
		SetState(IDLE);
	}
}

void Nokken::Attack()
{
	if (!bullet->IsVisible())
	{
		bullet->SetPosition(mSprite->getPosition());
		bullet->SetVisible(true);
		SetState(ATTACK);
	}
}

Bullet * Nokken::GetBullet()
{
	return bullet;
}

bool Nokken::Detect(float detectRange)
{
	auto mcPos = MainCharacter::GetInstance()->GetSprite()->getPosition();
	auto ePos = mSprite->getPosition();

	float dis = std::sqrt((mcPos.x - ePos.x)*(mcPos.x - ePos.x) + (mcPos.y - ePos.y)*(mcPos.y - ePos.y));

	if (dis <= detectRange)
	{
		return true;
	}

	return false;
}
