#ifndef LOADING_SCENE_H
#define LOADING_SCENE_H

#include "cocos2d.h"

using namespace cocos2d;

class LoadingScene : public cocos2d::Scene
{
public:
	static cocos2d::Scene* CreateScene();

	virtual bool init();
	CREATE_FUNC(LoadingScene);
};

#endif
