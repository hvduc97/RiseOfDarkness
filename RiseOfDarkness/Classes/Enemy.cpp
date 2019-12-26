#include "Enemy.h"
#include "MainCharacter.h"

Enemy::Enemy() {}

Enemy::~Enemy() {}

float Enemy::GetPercentHP()
{
	return currentHP / maxHP*100.0;
}

Sprite* Enemy::GetSprite()
{
	return mSprite;
}

PhysicsBody* Enemy::GetPhysicsBody()
{
	return mPhysicsBody;
}

bool Enemy::IsAlive()
{
	if (currentHP <= 0)
	{
		mPhysicsBody->setContactTestBitmask(false);
		mSprite->setVisible(false);
		hpBar->setVisible(false);
		hpLoadingBar->setVisible(false);
		return false;
	}
	return true;
}

void Enemy::GetDamage(int damage)
{
	currentHP -= damage;
	countingTime = 0;
}

int Enemy::GetDirection()
{
	return direction;
}

void Enemy::AutoRevive(int HP)
{
	if (currentHP < maxHP)
	{
		currentHP += HP;
		if (currentHP > maxHP)
		{
			currentHP = maxHP;
		}
	}
}

void Enemy::SetPreventRun()
{
	switch (direction)
	{
	case 1:
		mSprite->setPositionY(mSprite->getPositionY() - speed);
		preventRun = 1;
		break;
	case 2:
		mSprite->setPositionY(mSprite->getPositionY() + speed);
		preventRun = 2;
		break;
	case 3:
		mSprite->setPositionX(mSprite->getPositionX() + speed);
		preventRun = 3;
		break;
	case 4:
		mSprite->setPositionX(mSprite->getPositionX() - speed);
		preventRun = 4;
	}
}

void Enemy::ReverseDirection()
{
	switch (direction)
	{
	case 1:
		SetDirection(2);
		break;
	case 2:
		SetDirection(1);
		break;
	case 3:
		SetDirection(4);
		break;
	case 4:
		SetDirection(3);
	}
}

void Enemy::SetDirection(int dir)
{
	if (direction != dir)
	{
		if (dir == 4)
		{
			mSprite->setFlipX(true);
		}
		else if (direction == 4)
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

bool Enemy::Detect(float detectRange)
{
	auto mcPos = MainCharacter::GetInstance()->GetSprite()->getPosition();
	auto ePos = mSprite->getPosition();

	float dis = std::sqrt((mcPos.x - ePos.x)*(mcPos.x - ePos.x) + (mcPos.y - ePos.y)*(mcPos.y - ePos.y));

	if (dis <= detectRange)
	{
		if (countingTime > 1)
		{
			countingTime = 0;
			if (std::abs(mcPos.x - ePos.x) < std::abs(mcPos.y - ePos.y))
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

bool Enemy::Target(float attackRange)
{
	auto mcPos = MainCharacter::GetInstance()->GetSprite()->getPosition();
	auto ePos = mSprite->getPosition();

	float dis = std::sqrt((mcPos.x - ePos.x)*(mcPos.x - ePos.x) + (mcPos.y - ePos.y)*(mcPos.y - ePos.y));

	if (dis <= attackRange)
	{
		if (ePos.x <= mcPos.x + 3 && ePos.x >= mcPos.x - 3)
		{
			if (ePos.y < mcPos.y)
			{
				SetDirection(1);
			}
			else
			{
				SetDirection(2);
			}
		}
		else if (ePos.y <= mcPos.y + 3 && ePos.y >= mcPos.y - 3)
		{
			if (ePos.x > mcPos.x)
			{
				SetDirection(3);
			}
			else
			{
				SetDirection(4);
			}
		}
		else
		{
			return false;
		}
		return true;
	}
	return false;
}

Arrow* Enemy::GetArrow()
{
	return nullptr;
}

Bullet* Enemy::GetBullet() 
{
	return nullptr;
}

Item* Enemy::GetItem()
{
	return item;
}

Fire* Enemy::GetFire()
{
	return nullptr;
}