#ifndef ENEMY_H
#define ENEMY_H

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "Arrow.h"
#include "Bullet.h"
#include "Item.h"
#include "Fire.h"

using namespace cocos2d;

class Enemy
{
protected:
	Sprite* mSprite, *hpBar;
	ui::LoadingBar* hpLoadingBar;
	PhysicsBody* mPhysicsBody;
	float currentHP, maxHP;
	int direction, firstDirection;
	Vec2 pos;
	float countingTime, coolDownAttack;
	int currentState;
	int preventRun;
	int speed;
	Item* item;
public:
	Enemy();
	virtual ~Enemy();

	virtual void SetState(int nextState) = 0;
	virtual void Update(float deltaTime) = 0;
	virtual float GetPercentHP();
	virtual Sprite* GetSprite();
	virtual PhysicsBody* GetPhysicsBody();
	virtual void GetDamage(int damage);
	virtual bool IsAlive();
	virtual int GetDirection();
	virtual void AutoRevive(int HP);
	virtual void SetPreventRun();
	virtual void ReverseDirection();
	virtual void SetDirection(int dir);
	virtual bool Detect(float detectRange);
	virtual bool Target(float attackRange);
	virtual Arrow* GetArrow();
	virtual Bullet* GetBullet();
	virtual Item* GetItem();
	virtual Fire* GetFire();
};

#endif