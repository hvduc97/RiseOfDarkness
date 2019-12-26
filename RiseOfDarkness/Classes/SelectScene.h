#ifndef SELECT_SCENE_H
#define SELECT_SCENE_H

#include "cocos2d.h"

using namespace cocos2d;

class SelectScene : public Scene
{
public:
	static Scene* CreateScene();

	virtual bool init();

	CREATE_FUNC(SelectScene);
};

#endif

