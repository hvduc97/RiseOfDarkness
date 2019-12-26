#ifndef __MONSTER_H__
#define __MONSTER_H__

#include "Enemy.h"
#include "Bullet.h"
#include <vector>

class Monster : public Enemy
{
private:
	static const int FRONT_IDLE = 0;
	static const int BACK_IDLE = 1;
	static const int RIGHT_IDLE = 2;
	static const int GO_UP = 3;
	static const int GO_DOWN = 4;
	static const int GO_RIGHT = 5;
	static const int FRONT_ATTACK = 6;
	static const int BACK_ATTACK = 7;
	static const int RIGHT_ATTACK = 8;

	static const int HP = 150;
	static const int DETECT_RANGE = 300;
	static const int ATTACK_RANGE = 200;
	static const int ATTACK_COOLDOWN = 3;
	static const int REVIVE_TIME = 10;
	static const int HP_REVIVE = 10;

	Action* mAction[9];
	Bullet* bullet;
public:
	Monster();
	Monster(Layer* layer, int direction, Vec2 pos, int group);
	~Monster();

	void Update(float deltaTime);

	void SetState(int nextState);

	void Run();

	void Idle();

	void Attack();

	Bullet* GetBullet();

	bool Detect(float detectRange);

	void SetDirection(int dir);
};

#endif // !__MONSTER_H__

