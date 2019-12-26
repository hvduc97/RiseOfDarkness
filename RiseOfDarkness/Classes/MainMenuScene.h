#ifndef MAIN_MENU_SCENE_H
#define MAIN_MENU_SCENE_H

#include "cocos2d.h"

using namespace cocos2d;

class MainMenuScene : public Scene
{
public:
	static cocos2d::Scene* CreateScene();
	virtual bool init();

	CREATE_FUNC(MainMenuScene);
};

#endif


