#ifndef SPEAR_MOBLIN_H
#define SPEAR_MOBLIN_H	

#include "Enemy.h"
#include "Pierce.h"

class SpearMoblin : public Enemy
{
private:
	static const int FRONT_IDLE = 0;
	static const int BACK_IDLE = 1;
	static const int LEFT_IDLE = 2;
	static const int GO_UP = 3;
	static const int GO_DOWN = 4;
	static const int GO_LEFT = 5;
	static const int FRONT_ATTACK = 6;
	static const int BACK_ATTACK = 7;
	static const int LEFT_ATTACK = 8;

	static const int HP = 350;
	static const int DETECT_RANGE = 80;
	static const int ATTACK_RANGE = 30;
	static const int ATTACK_COOLDOWN = 1;
	static const int REVIVE_TIME = 7;
	static const int HP_REVIVE = 20;

	Action* mAction[9];
	Pierce* pierce;
public:
	SpearMoblin();
	SpearMoblin(Layer* layer, int direction, Vec2 pos, int group);
	~SpearMoblin();

	void Update(float deltaTime);

	void SetState(int nextState);

	void Run();

	void Idle();

	void Attack();
};

#endif
