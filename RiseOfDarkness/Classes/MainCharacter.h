#ifndef MAIN_CHARACTER_H
#define MAIN_CHARACTER_H

#include "cocos2d.h"
#include "Slash.h"
#include <vector>
#include "Arrow.h"
#include "Item.h"
#include "Inventory.h"
#include "FlySlash.h"

using namespace cocos2d;

class MainCharacter
{
public: 
	// BIT MASK
	static const int MAIN_CHARACTER_BITMASK = 100;
	static const int OBSTACLE_BITMASK = 101;
	static const int SLASH_BITMASK = 102;
	static const int SPEARMOBLIN_BITMASK = 103;
	static const int PIERCE_BITMASK = 104;
	static const int NORMAL_ARROW_BITMASK = 105;
	static const int BOWMOBLIN_BITMASK = 106;
	static const int BOWMOBLIN_ARROW_BITMASK = 107;
	static const int BULLET_BITMASK = 108;
	static const int ROPE_MONSTER_BITMASK = 109;
	static const int HEART_CONTAINER_BITMASK = 110;
	static const int FLY_SLASH_BITMASK = 111;
	static const int RIVER_BITMASK = 112;
	static const int NOKKEN_MONSTER_BITMASK = 113;
	static const int MAOKAI_MONSTER_BITMASK = 118;
	static const int ELISE_MONSTER_BITMASK = 119;
	static const int WARWICK_MONSTER_BITMASK = 120;
	static const int FIRE_BITMASK = 121;
	static const int SCREEN_BORDER_BITMASK = 122;

	static const int ARROW1_ITEM_BITMASK = 114;
	static const int APPLE_ITEM_BITMASK = 115;
	static const int MUSHROOM_ITEM_BITMASK = 116;
	static const int MEAT_ITEM_BITMASK = 117;

	// POTION
	static const int HP = 50;
	static const int MP = 20;

	// STATUS
	static const int SPEED = 1;
	static const int ATTACK = 20;
	static const int DEFEND = 10;

	// DAMAGE ALL CHARACTER
	static const int SPEARMOBLIN_DAMAGE = 50;
	static const int NORMAL_ARROW = 10;
	static const int BOWMOBLIN_DAMAGE = 45;
	static const int ROPE_MONSTER_DAMAGE = 35;
	static const int BULLET_DAMAGE = 35;
	static const int FIRE_DAMAGE = 70;
	static const int WARWICK_DAMAGE = 75;

	// GOLD RECEIVE FROM KILL ENEMY
	static const int BOWMOBLIN_GOLD = 50;
	static const int SPEARMOBLIN_GOLD = 50;
	static const int ROPE_MONSTER_GOLD = 40;
	static const int NOKKEN_MONSTER_GOLD = 20;
	static const int MAOKAI_MONSTER_GOLD = 35;
	static const int ELISE_MONSTER_GOLD = 45;
	static const int WARWICK_MONSTER_GOLD = 30;

	static const int FRONT_IDLE = 0;
	static const int BACK_IDLE = 1;
	static const int LEFT_IDLE = 2;
	static const int GO_DOWN = 3;
	static const int GO_UP = 4;
	static const int GO_LEFT = 5;
	static const int ROLL_FRONT = 6;
	static const int ROLL_BACK = 7;
	static const int ROLL_LEFT = 8;
	static const int FRONT_SLASH = 9;
	static const int BACK_SLASH = 10;
	static const int LEFT_SLASH = 11;
	static const int FRONT_ARCHERY = 12;
	static const int BACK_ARCHERY = 13;
	static const int LEFT_ARCHERY = 14;
	static const int FRONT_SHIELD = 15;
	static const int BACK_SHIELD = 16;
	static const int LEFT_SHIELD = 17;
	static const int FRONT_GET_DAMAGE = 18;
	static const int BACK_GET_DAMAGE = 19;
	static const int LEFT_GET_DAMAGE = 20;
	static const int DEAD = 21;

private:
	int stageLevel;
	int speed;
	int attack;
	int arrowAttack;
	int defend;
	int arrowRange;
	int heartContainer;
	std::string mName;
	float maxHP, currentHP, maxMP, currentMP;
	int gold;
	std::vector<Arrow*>mArrows;
	std::vector<int> mAllItems;
	std::vector<Item*> mItems;
	Inventory *inventory;
	std::vector<bool> heartContainerCollect;
public:
	int GetArrowAttack();
	void TakeArrowAttack(int id);
	int GetArrowRange();
	void TakeBow(int id);
	int GetSpeed();
	int GetAttack();
	int GetMaxHP();
	int GetCurrentHP();
	int GetMaxMP();
	int GetCurrentMP();
	int GetDefend();
	std::string GetName();
	int GetStageLevel();
	Inventory *GetInventory();
	int GetGold();
	void AddGold(int numb);
	void SubGold(int numb);
	int GetHeartContainer();
	void TakeHeartContainer();
	void EquipedItem(int index, Item item);
	std::vector<Item*> GetEquipedItem();
	bool TakePotion(int index);
	void TakeSword(int index);
	void TakeArmor(int index);
	void TakeBoots(int index);
	bool GetCheckHeartCollect(int stage);
	void SetHeartCollected(int stage);

public:
	static MainCharacter* GetInstance();
	void Init(std::string name);
	void Refresh();
	Sprite* GetSprite();
	PhysicsBody* GetPhysicsBody();
	int GetCurrentState();
	void Update(float deltaTime);
	void SetState(int nextStage);
	int GetDirection();
	void SetDirection(int direction);
	void Idle();
	void NormalAttack();
	void SpecialAttack();
	void Defend();
	void Evade();
	void Run(float percentSpeed);
	void StopRun();
	void Fire(int);
	void SetListArrow(std::vector<Arrow*>);
	std::vector<Arrow*> GetListArrow();
	void StopDefend();
	float GetPercentHP();
	float GetPercentMP();
	void AutoRevive(float deltaTime);
	void AddToLayer(Layer* layer);
	void GetDamage(int damage);
	bool IsAlive();
	void SetPreventRun(int prevent);
	void IncreaseStage();
	float GetPace();
	FlySlash* GetFlySlash();
private:
	static MainCharacter* m_instance;
	Sprite* mSprite;
	Action* mAction[22];
	PhysicsBody* mPhysicsBody;
	int currentState;
	int direction;
	int preventRun;	
	float countingTime;
	Slash* slash;
	FlySlash* flySlash;
	MainCharacter();
	~MainCharacter();
	void CreateMainCharacter();
	float pace;
};

#endif