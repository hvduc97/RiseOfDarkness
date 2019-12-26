#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include "cocos2d.h"
#include "ui/CocosGUI.h"

using namespace cocos2d;

class ResourceManager
{
private:
	static ResourceManager* m_instance;
	std::string m_dataFolderPath;
	std::map<int, Sprite*> m_sprites;
	std::map<int, ui::Button*>m_buttons;
	std::map<int, Label*>m_labels;
	std::map<int, Action*> m_actions;
	std::map<int, TMXTiledMap*> m_tileMap;
	std::map<int, ui::LoadingBar*> m_loadingbar;
	ResourceManager();
	~ResourceManager();
public:
	static ResourceManager* GetInstance();
	void Init(const std::string &path);
	Sprite* GetSpriteById(int id);
	ui::Button* GetButtonById(int id);
	Label* GetLabelById(int id);
	Action* GetActionById(int id);
	TMXTiledMap* GetTileMapById(int id);
	ui::LoadingBar* GetLoadingbar(int id);
	Sprite* DuplicateSprite(Sprite* sprite);
	Action* GetAction(std::string &dir, int count, float deltaTime);
};

#endif