#ifndef __NOKKEN_H__
#define __NOKKEN_H__

#include "Enemy.h"
#include "cocos2d.h"
#include "Bullet.h"

class Nokken : public Enemy
{
private:
	static const int IDLE = 0;
	static const int ATTACK = 1;

	static const int HP = 200;
	static const int DETECT_RANGE = 150;
	static const int ATTACK_RANGE = 150;
	static const int ATTACK_COOLDOWN = 2;
	static const int REVIVE_TIME = 10;
	static const int HP_REVIVE = 10;

	Action* mAction[2];
	Bullet* bullet;
public:
	Nokken();
	Nokken(Layer* layer, Vec2 pos, int group);
	~Nokken();

	void Update(float deltaTime);

	void SetState(int nextState);

	void Idle();

	void Attack();

	Bullet* GetBullet();

	bool Detect(float detectRange);
};

#endif // !__NOKKEN_H__
