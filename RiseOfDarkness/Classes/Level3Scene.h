#ifndef LEVEL3_SCENE_H
#define LEVEL3_SCENE_H

#include "GamePlay.h"
#include "cocos2d.h"

using namespace cocos2d;

class Level3Scene : public Layer, GamePlay
{
private:
	Sprite* heartContainer;
public:
	static cocos2d::Scene* CreateScene();

	virtual bool init();

	void update(float deltaTime);

	void AddListener();

	void CreateMonster();

	bool OnTouchBegan(Touch* touch, Event* event);
	bool OnTouchEnded(Touch* touch, Event* event);
	void OnTouchMoved(Touch* touch, Event* event);
	void NormalAttack(Ref* sender, ui::Widget::TouchEventType type);
	void SpecialAttack(Ref* sender, ui::Widget::TouchEventType type);
	void Evade(Ref* sender, ui::Widget::TouchEventType type);
	void Defend(Ref* sender, ui::Widget::TouchEventType type);
	bool onContactBegin(PhysicsContact& contact);
	void OpenInventory(cocos2d::Ref * sender);
	void CreateTreasure();
	void ClickShowInfor(Ref* pSender);

	CREATE_FUNC(Level3Scene);
};

#endif
