#ifndef __ELISE_H__
#define __ELISE_H__

#include "Enemy.h"
#include "Bullet.h"
#include <vector>

class Elise : public Enemy
{
private:
	static const int GO_UP = 0;
	static const int GO_DOWN = 1;
	static const int GO_LEFT = 2;
	static const int GO_RIGHT = 3;

	static const int UP_ATTACK = 4;
	static const int DOWN_ATTACK = 5;
	static const int LEFT_ATTACK = 6;
	static const int RIGHT_ATTACK = 7;

	static const int HP = 500;
	static const int DETECT_RANGE = 300;
	static const int ATTACK_RANGE = 200;
	static const int ATTACK_COOLDOWN = 2;
	static const int REVIVE_TIME = 5;
	static const int HP_REVIVE = 40;

	Action* mAction[9];
	Bullet* bullet;
public:
	Elise();
	Elise(Layer* layer, int direction, Vec2 pos, int group);
	~Elise();

	void Update(float deltaTime);

	void SetState(int nextState);

	void Run();

	void Idle();

	void Attack();

	Bullet* GetBullet();

	bool Detect(float detectRange);

	void SetDirection(int dir);
};

#endif // !__ELISE_H__

