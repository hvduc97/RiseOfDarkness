#include "MainMenuScene.h"
#include "ui/CocosGUI.h"
#include "SelectScene.h"
#include "ResourceManager.h"
#include "SimpleAudioEngine.h"


USING_NS_CC;
using namespace ui;
using namespace CocosDenshion;

Scene* MainMenuScene::CreateScene()
{
	return MainMenuScene::create();
}

bool MainMenuScene::init()
{
	if (!Scene::init())
	{
		return false;
	}

	SimpleAudioEngine::getInstance()->stopBackgroundMusic(true);
	SimpleAudioEngine::getInstance()->playBackgroundMusic("audio/mainMenu/menu.wav", true);

	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	auto bg = ResourceManager::GetInstance()->GetSpriteById(5);//Sprite::create("/res/sprites/bgbg.png");
	bg->removeFromParent();
	bg->setScale(visibleSize.width / bg->getContentSize().width, visibleSize.height / bg->getContentSize().height);
	bg->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
	this->addChild(bg, -1);

	auto button = ResourceManager::GetInstance()->GetButtonById(2);//Button::create("/res/buttons/startt.png");
	button->removeFromParent();
	button->setPosition(visibleSize / 2);
	//button->setScaleX(2);
	button->addTouchEventListener([&](Ref* sender, Widget::TouchEventType type)
	{
		if (type == ui::Widget::TouchEventType::ENDED)
		{
			auto gotoNext = CallFunc::create([]() {
				Director::getInstance()->replaceScene(SelectScene::create());
			});
			runAction(gotoNext);
		}
	});

	this->addChild(button);

	


	return true;
}


