#ifndef GAME_PLAY_H
#define GAME_PLAY_H

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "Enemy.h"
#include "SneakyJoystickSkinnedBase.h"

using namespace cocos2d;

class GamePlay
{
protected:
	Sprite* mainCharacter;
	TMXTiledMap* tileMap, *upperTileMap;
	Camera* camera, *anchorCamera;
	PhysicsBody* body;
	std::vector<ui::Button*> m_buttons;
	std::vector<Sprite*> m_sprites;
	Label* mName, *gameover, *clear, *gold, *paused, *amountHP, *amountMP, *health, *attack, *armor, *speedBoot, *amountEnemy, *arrowAttack;
	ui::LoadingBar *hpLoadingBar, *mpLoadingBar;
	std::vector<Enemy*> m_enemies;
	int currentStage;
	SneakyJoystickSkinnedBase* joystickBase;
	SneakyJoystick* leftJoystick;
	Sprite* shader;
	float activeRunRange;
	ui::Layout* tab;
	ui::Button* nextStage;
public:
	GamePlay();
	virtual ~GamePlay();
	virtual void update(float deltaTime) = 0;
	virtual void UpdateInfoBar();

	virtual void CreateAllButton(Layer* layer);
	virtual void CreatePhysicsWorld(const char* obstacle, const char* mc, Layer* layer);
	virtual void SetCamera(Vec2 pos);
	virtual void AddListener() = 0;

	virtual void OpenInventory(cocos2d::Ref * sender);
	virtual void ShowInventoryGrid();

	virtual bool CheckClear();
	virtual void EnemyUpdate(float deltaTime);

	virtual void CreateJoystick(Layer* layer);
	virtual void UpdateJoystick();

	virtual void InitChest(Layer* layer);
	virtual void ShowInfor();
	virtual int GetAliveEnemies();
};

#endif
