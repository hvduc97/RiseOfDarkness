#include "SelectScene.h"
#include "ui/CocosGUI.h"
#include "ResourceManager.h"
#include "InputNameScene.h"

USING_NS_CC;
using namespace ui;

Scene* SelectScene::CreateScene()
{
	return SelectScene::create();
}

bool SelectScene::init()
{
	if (!Scene::init())
	{
		return false;
	}


	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	auto bg = ResourceManager::GetInstance()->GetSpriteById(7);//Sprite::create("/res/sprites/selectbg.jpg");
	bg->removeFromParent();
	bg->setScale(visibleSize.width / bg->getContentSize().width, visibleSize.height / bg->getContentSize().height);
	bg->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
	this->addChild(bg, -1);
	//===============
	auto button = ResourceManager::GetInstance()->GetButtonById(3)->clone();//Button::create("/res/buttons/chose.png");
	button->setPosition(Vec2(visibleSize.width / 4 + origin.x, visibleSize.height / 1.5 + origin.y));
	button->setScale(0.75);
	this->addChild(button,0);

	auto buttonnewgame = ResourceManager::GetInstance()->GetButtonById(6)->clone();//Button::create("/res/buttons/newgame .png"); 
	buttonnewgame->setPosition(Vec2(visibleSize.width / 4 + origin.x, visibleSize.height / 1.3 + origin.y));
	buttonnewgame->setScale(0.65);
	buttonnewgame->addTouchEventListener([&](Ref* sender, Widget::TouchEventType type)
	{
		if (type == ui::Widget::TouchEventType::ENDED)
		{
			auto gotoNext = CallFunc::create([]() {
				Director::getInstance()->replaceScene(InputNameScene::create());
			});
			runAction(gotoNext);
		}
	});
	this->addChild(buttonnewgame,1);

	auto btcont = ResourceManager::GetInstance()->GetButtonById(4)->clone();//Button::create("/res/buttons/continue.png");
	btcont->setPosition(Vec2(visibleSize.width / 4 + origin.x, visibleSize.height / 1.6 + origin.y));
	btcont->setScale(0.65);
	this->addChild(btcont, 1);
	btcont->setEnabled(false);
	//===================
	
	auto button2 = ResourceManager::GetInstance()->GetButtonById(3)->clone(); //Button::create("/res/buttons/chose.png");
	button2->setPosition(Vec2(visibleSize.width / 4.35+button2->getContentSize().width + origin.x, visibleSize.height / 1.5 + origin.y));
	button2->setScale(0.75);
	this->addChild(button2);

	auto buttonnewgame2 = ResourceManager::GetInstance()->GetButtonById(6)->clone(); //Button::create("/res/buttons/newgame .png");
	buttonnewgame2->setPosition(Vec2(visibleSize.width / 4.35 + button2->getContentSize().width + origin.x, visibleSize.height / 1.3 + origin.y));
	buttonnewgame2->setScale(0.65);
	
	buttonnewgame2->addTouchEventListener([&](Ref* sender, Widget::TouchEventType type)
	{
		if (type == ui::Widget::TouchEventType::ENDED)
		{
			auto gotoNext = CallFunc::create([]() {
				Director::getInstance()->replaceScene(InputNameScene::create());
			});
			runAction(gotoNext);
		}
	});
	
	this->addChild(buttonnewgame2, 1);

	auto btcont2 = ResourceManager::GetInstance()->GetButtonById(4)->clone(); //Button::create("/res/buttons/continue.png");
	btcont2->setPosition(Vec2(visibleSize.width / 4.35 + button2->getContentSize().width + origin.x, visibleSize.height / 1.6 + origin.y));
	btcont2->setScale(0.65);
	this->addChild(btcont2, 1);
	btcont2->setEnabled(false);

	//========================
	auto button3 = ResourceManager::GetInstance()->GetButtonById(3)->clone(); //Button::create("/res/buttons/chose.png");
	button3->setPosition(Vec2(visibleSize.width / 5 + button2->getContentSize().width*2 + origin.x, visibleSize.height / 1.5 + origin.y));
	button3->setScale(0.75);
	this->addChild(button3);

	auto buttonnewgame3 = ResourceManager::GetInstance()->GetButtonById(6)->clone(); //Button::create("/res/buttons/newgame .png");
	buttonnewgame3->setPosition(Vec2(visibleSize.width / 5 + button2->getContentSize().width * 2 + origin.x, visibleSize.height / 1.3 + origin.y));
	buttonnewgame3->setScale(0.65);
	buttonnewgame3->addTouchEventListener([&](Ref* sender, Widget::TouchEventType type)
	{
		if (type == ui::Widget::TouchEventType::ENDED)
		{
			auto gotoNext = CallFunc::create([]() {
				Director::getInstance()->replaceScene(InputNameScene::create());
			});
			runAction(gotoNext);
		}
	});

	this->addChild(buttonnewgame3, 1);

	auto btcont3 = ResourceManager::GetInstance()->GetButtonById(4)->clone(); //Button::create("/res/buttons/continue.png");
	btcont3->setPosition(Vec2(visibleSize.width / 5 + button2->getContentSize().width * 2, visibleSize.height / 1.6 + origin.y));
	btcont3->setScale(0.65);
	this->addChild(btcont3, 1);
	btcont3->setEnabled(false);
	//========================
	auto buttondelete = ResourceManager::GetInstance()->GetButtonById(5)->clone(); //Button::create("/res/buttons/delete.png");
	buttondelete->setPosition(Vec2(visibleSize.width / 4+ origin.x, visibleSize.height / 2.75 + origin.y));
	buttondelete->setScale(0.75);
	this->addChild(buttondelete);
	buttondelete->setEnabled(false);

	auto buttondelete2 = ResourceManager::GetInstance()->GetButtonById(5)->clone(); //Button::create("/res/buttons/delete.png");
	buttondelete2->setPosition(Vec2(visibleSize.width / 4.35 + button2->getContentSize().width + origin.x, visibleSize.height / 2.75 + origin.y));
	buttondelete2->setScale(0.75);
	this->addChild(buttondelete2);
	buttondelete2->setEnabled(false);
	
	auto buttondelete3 = ResourceManager::GetInstance()->GetButtonById(5)->clone(); //Button::create("/res/buttons/delete.png");
	buttondelete3->setPosition(Vec2(visibleSize.width / 5 + button2->getContentSize().width*2 + origin.x, visibleSize.height / 2.75 + origin.y));
	buttondelete3->setScale(0.75);
	this->addChild(buttondelete3);
	buttondelete3->setEnabled(false);

	//=============
	//auto text = ResourceManager::GetInstance()->DuplicateSprite(ResourceManager::GetInstance()->GetSpriteById(4));//Sprite::create("/res/sprites/h.png");
	//text->setAnchorPoint(Vec2(0, 0));
	//text->setPosition(Vec2(visibleSize.width / 8 + origin.x, visibleSize.height / 30+ origin.y));
	//text->setScaleX(1.5);
	//text->setScaleY(0.4);
	//this->addChild(text);
	return true;
}



