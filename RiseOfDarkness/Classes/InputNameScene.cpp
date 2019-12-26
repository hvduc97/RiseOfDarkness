#include "InputNameScene.h"
#include <String.h>
#include <iostream>
#include "HomeScene.h"
#include "Level2Scene.h"
#include "ResourceManager.h"
#include "MainCharacter.h"
#include "Level1Scene.h"
#include "Level3Scene.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;
using namespace CocosDenshion;

Scene* InputNameScene::CreateScene()
{
	auto scene = Scene::create();
	auto layer = InputNameScene::create();
	scene->addChild(layer);
	return scene;
}

bool InputNameScene::init()
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

	auto text = ResourceManager::GetInstance()->GetSpriteById(8);//Sprite::create("/res/sprites/inputname.png");
	text->removeFromParent();
	text->setAnchorPoint(Vec2(0, 0));
	text->setPosition(Vec2(visibleSize.width / 4 + origin.x, visibleSize.height / 6 + origin.y));
	this->addChild(text);

	auto label = ResourceManager::GetInstance()->GetLabelById(0);//Label::createWithTTF("Enter your name ", "fonts/Marker Felt.ttf", 40);
	label->removeFromParent();
	label->setString("Enter your name ");
	label->setBMFontSize(40);
	label->setPosition(Vec2(origin.x + visibleSize.width / 2,
			origin.y + visibleSize.height / 1.5-10));
	label->enableOutline(Color4B::RED, 1);
	this->addChild(label, 1);

	note = ResourceManager::GetInstance()->GetSpriteById(18);
	note->removeFromParent();
	addChild(note, 5);
	note->setPosition(visibleSize / 2);
	note->setVisible(false);

	warning = ResourceManager::GetInstance()->GetLabelById(1);
	warning->removeFromParent();
	warning->setPosition(visibleSize / 2);
	warning->setString("Please enter your name!");
	addChild(warning, 6);
	warning->setVisible(false);

	close = ResourceManager::GetInstance()->GetButtonById(22);
	close->removeFromParent();
	close->setScale(0.2);
	addChild(close, 6);
	close->setPosition(visibleSize / 2 + note->getBoundingBox().size / 2);
	close->setVisible(false);
	close->addTouchEventListener([&](Ref* sender, ui::Widget::TouchEventType type)
	{
		if (type == ui::Widget::TouchEventType::ENDED)
		{
			note->setVisible(false);
			warning->setVisible(false);
			close->setVisible(false);
		}
	});
	
	textField = ui::TextField::create("enter here.. ", "fonts/Marker Felt.ttf", 30);
	
	textField->setPasswordEnabled(false);
	textField->setMaxLength(50);
	textField->setPosition(Vec2(visibleSize.width / 2, visibleSize.height/1.5-70));
	textField->setColor(Color3B(112, 40, 4));
	/*textField->addClickEventListener([](Ref* event) {
		log("TextField: %s", textField.getString().c_str);
	});*/
	
    textField->addTouchEventListener([&](Ref* sender, ui::Widget::TouchEventType type) {
	});
	
	this->addChild(textField,2);

	auto buttonfinish = ResourceManager::GetInstance()->GetButtonById(9);//ui::Button::create("/res/buttons/finish.png");
	buttonfinish->removeFromParent();
	buttonfinish->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 3 + origin.y));
	//buttonfinish->setScaleX(2);

	buttonfinish->addTouchEventListener([&](Ref* sender, ui:: Widget::TouchEventType type)
	{
		if (type == ui::Widget::TouchEventType::ENDED)
		{
			MainCharacter::GetInstance()->Init(GetTextField()->getString());
			if (MainCharacter::GetInstance()->GetName() != "")
			{
				auto gotoNext = CallFunc::create([]() {
					Director::getInstance()->replaceScene(HomeScene::CreateScene());
				});
				runAction(gotoNext);
			}
			else
			{
				note->setVisible(true);
				close->setVisible(true);
				warning->setVisible(true);
			}
		}
	});

	this->addChild(buttonfinish);
	return true;
}

ui::TextField* InputNameScene::GetTextField()
{
	return textField;
}