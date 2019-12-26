#include "Monster.h"
#include "ResourceManager.h"
#include "MainCharacter.h"
#include <math.h>
#include "SimpleAudioEngine.h"

using namespace CocosDenshion;

Monster::Monster() {}

Monster::Monster(Layer* layer, int direction, Vec2 pos, int group)
{
	mSprite = ResourceManager::GetInstance()->DuplicateSprite(ResourceManager::GetInstance()->GetSpriteById(23));
	mSprite->setScale(1.7f);
	Size box;
	box.width = mSprite->getContentSize().width / 1.2;
	box.height = mSprite->getContentSize().height / 2.2;
	mPhysicsBody = PhysicsBody::createBox(box, PhysicsMaterial(0, 0, 0), Vec2(0, box.height / 6));
	mPhysicsBody->setRotationEnable(false);
	mPhysicsBody->setCollisionBitmask(MainCharacter::ROPE_MONSTER_BITMASK);
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

	mAction[BACK_IDLE] = ResourceManager::GetInstance()->GetActionById(43)->clone();
	mAction[FRONT_IDLE] = ResourceManager::GetInstance()->GetActionById(44)->clone();
	mAction[RIGHT_IDLE] = ResourceManager::GetInstance()->GetActionById(45)->clone();
	mAction[GO_UP] = ResourceManager::GetInstance()->GetActionById(46)->clone();
	mAction[GO_DOWN] = ResourceManager::GetInstance()->GetActionById(47)->clone();
	mAction[GO_RIGHT] = ResourceManager::GetInstance()->GetActionById(48)->clone();
	mAction[FRONT_ATTACK] = ResourceManager::GetInstance()->GetActionById(49)->clone();
	mAction[BACK_ATTACK] = ResourceManager::GetInstance()->GetActionById(50)->clone();
	mAction[RIGHT_ATTACK] = ResourceManager::GetInstance()->GetActionById(51)->clone();

	CC_SAFE_RETAIN(mAction[FRONT_IDLE]);
	CC_SAFE_RETAIN(mAction[BACK_IDLE]);
	CC_SAFE_RETAIN(mAction[RIGHT_IDLE]);
	CC_SAFE_RETAIN(mAction[GO_DOWN]);
	CC_SAFE_RETAIN(mAction[GO_UP]);
	CC_SAFE_RETAIN(mAction[GO_RIGHT]);
	CC_SAFE_RETAIN(mAction[FRONT_ATTACK]);
	CC_SAFE_RETAIN(mAction[BACK_ATTACK]);
	CC_SAFE_RETAIN(mAction[RIGHT_ATTACK]);

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

	auto sprite = ResourceManager::GetInstance()->DuplicateSprite(ResourceManager::GetInstance()->GetSpriteById(24));
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

Monster::~Monster()
{
	mAction[FRONT_IDLE]->autorelease();
	mAction[BACK_IDLE]->autorelease();
	mAction[RIGHT_IDLE]->autorelease();
	mAction[GO_DOWN]->autorelease();
	mAction[GO_UP]->autorelease();
	mAction[GO_RIGHT]->autorelease();
	mAction[FRONT_ATTACK]->autorelease();
	mAction[BACK_ATTACK]->autorelease();
	mAction[RIGHT_ATTACK]->autorelease();

	hpBar->autorelease();
	hpLoadingBar->autorelease();
}

void Monster::Update(float deltaTime)
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
		if (bullet->IsVisible())
		{
			bullet->update(deltaTime);
		}
	}
	else
	{
		MainCharacter::GetInstance()->AddGold(MainCharacter::ROPE_MONSTER_GOLD);
		bullet->SetVisible(false);
		item->GetIcon()->setPosition(mSprite->getPosition());
		item->GetIcon()->setVisible(true);
		item->GetIcon()->getPhysicsBody()->setContactTestBitmask(true);
		SimpleAudioEngine::getInstance()->playEffect("audio/inventorySoundEffects/sellBuyItem.wav", false);
	}
}

void Monster::SetState(int nextState)
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
		case RIGHT_IDLE:
			currentState = nextState;
			mSprite->runAction(mAction[nextState]);
			break;
		case GO_UP:
		case GO_DOWN:
		case GO_RIGHT:
			currentState = nextState;
			mSprite->stopAllActions();
			mSprite->runAction(mAction[nextState]);
			break;
		case FRONT_ATTACK:
		case BACK_ATTACK:
		case RIGHT_ATTACK:
			currentState = nextState;
			mSprite->stopAllActions();
			mSprite->runAction(mAction[nextState]);
		}
	}
}

void Monster::Idle()
{
	if (mSprite->getNumberOfRunningActions() == 0)
	{

		switch (direction)
		{
		case 1:
			SetState(BACK_IDLE);
			break;
		case 2:
			SetState(FRONT_IDLE);
			break;
		case 3:
			mSprite->setFlippedX(true);
			SetState(RIGHT_IDLE);
			break;
		case 4:
			mSprite->setFlippedX(false);
			SetState(RIGHT_IDLE);
			break;
		}
	}
}

void Monster::Run()
{
	if (currentState != RIGHT_ATTACK || currentState != FRONT_ATTACK || currentState != BACK_ATTACK)
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
				SetState(GO_RIGHT);
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

void Monster::Attack()
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
			SetState(BACK_ATTACK);
			break;
		case 2:
			bullet->SetDirection(3);
			SetState(FRONT_ATTACK);
			break;
		case 3:
			bullet->SetDirection(0);
			SetState(RIGHT_ATTACK);
			break;
		case 4:
			bullet->SetDirection(1);
			SetState(RIGHT_ATTACK);
		}
	}
}

Bullet* Monster::GetBullet()
{
	return bullet;
}

bool Monster::Detect(float detectRange)
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

void Monster::SetDirection(int dir)
{
	if (direction != dir)
	{
		if (dir == 3)
		{
			mSprite->setFlipX(true);
		}
		else if (direction == 3)
		{
			mSprite->setFlipX(false);
		}
		direction = dir;
	}
	if (preventRun != dir)
	{
		preventRun = 0;
	}
}