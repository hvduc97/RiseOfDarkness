
#ifndef __MAP_SCENE_H__
#define __MAP_SCENE_H__

#include "cocos2d.h"

class MapScene : public cocos2d::Layer
{
public:
    static cocos2d::Scene* CreateScene();

    virtual bool init();
	void LoadLevel(cocos2d::Ref *pSender,int level);
	void CreateMap();
	void btnBackCallback(cocos2d::Ref *pSender);
	void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *unused_event);
    CREATE_FUNC(MapScene);
private:
	cocos2d::TMXTiledMap *mTileMap;
	cocos2d::Sprite* player;
	cocos2d::TMXLayer *meta;
	cocos2d::TMXLayer *collectMap;
	int lv;
};

#endif
