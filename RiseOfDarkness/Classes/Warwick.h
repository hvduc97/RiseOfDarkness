#ifndef WARWICK_H
#define WARWICK_H	

#include "Enemy.h"

class Warwick : public Enemy
{
private:
	static const int GO_UP = 0;
	static const int GO_DOWN = 1;
	static const int GO_LEFT = 2;
	static const int FRONT_ATTACK = 3;
	static const int BACK_ATTACK = 4;
	static const int LEFT_ATTACK = 5;

	static const int HP = 500;
	static const int DETECT_RANGE = 80;
	static const int ATTACK_RANGE = 20;
	static const int ATTACK_COOLDOWN = 1;
	static const int REVIVE_TIME = 5;
	static const int HP_REVIVE = 30;

	Action* mAction[9];
public:
	Warwick();
	Warwick(Layer* layer, int direction, Vec2 pos, int group);
	~Warwick();

	void Update(float deltaTime);

	void SetState(int nextState);

	void Run();

	void Idle();

	void Attack();
};

#endif
