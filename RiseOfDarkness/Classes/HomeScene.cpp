#include "HomeScene.h"
#include "MapScene.h"
#include "MainCharacter.h"
#include "ResourceManager.h"
#include "SimpleAudioEngine.h"

using namespace CocosDenshion;
USING_NS_CC;


Scene* HomeScene::CreateScene()
{
	auto scene = Scene::createWithPhysics();
    //scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);

	auto layer = HomeScene::create();

	scene->addChild(layer);

	return scene;
}

bool HomeScene::init()
{
	if (!Layer::init())
	{
		return false;
	}

	SimpleAudioEngine::getInstance()->stopBackgroundMusic(true);
	SimpleAudioEngine::getInstance()->playBackgroundMusic("audio/homeScene/home.mp3", true);

	MainCharacter::GetInstance()->Refresh();

	tileMap = ResourceManager::GetInstance()->GetTileMapById(0);
	upperTileMap = ResourceManager::GetInstance()->GetTileMapById(1);
	
	auto layer = Layer::create();
	
	CreatePhysicsWorld("obstacles", "mc", this);

	CreateAllButton(this);
	
	AddListener();

	CreateNPC();

	InitChest(this);

	scheduleUpdate();


	return true;
}

void HomeScene::update(float deltaTime)
{
	UpdateInfoBar();

	MainCharacter::GetInstance()->Update(deltaTime);

	SetCamera(mainCharacter->getPosition());

	RunActionNPC();

	UpdateJoystick();

	gold->setString(std::to_string(MainCharacter::GetInstance()->GetGold()));
}

void HomeScene::AddListener()
{
	auto touchListener = EventListenerTouchOneByOne::create();
	touchListener->onTouchBegan = CC_CALLBACK_2(HomeScene::OnTouchBegan, this);
	touchListener->onTouchEnded = CC_CALLBACK_2(HomeScene::OnTouchEnded, this);
	touchListener->onTouchMoved = CC_CALLBACK_2(HomeScene::OnTouchMoved, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);

	m_buttons[1]->addClickEventListener([&](Ref* event) {
		auto gotoMapScene = CallFunc::create([] {
			Director::getInstance()->replaceScene(MapScene::CreateScene());
		});
		runAction(gotoMapScene);
	});

	auto contactListener = EventListenerPhysicsContact::create();
	contactListener->onContactBegin = CC_CALLBACK_1(HomeScene::onContactBegin, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);

	m_buttons[2]->addClickEventListener(CC_CALLBACK_1(HomeScene::OpenInventory, this));
	
	m_buttons[0]->addClickEventListener(CC_CALLBACK_1(HomeScene::ClickShowInfor, this));
}

bool HomeScene::OnTouchBegan(Touch* touch, Event* event)
{
	
	return true;
}

bool HomeScene::OnTouchEnded(Touch* touch, Event* event)
{
	
	return true;
}

void HomeScene::OnTouchMoved(Touch* touch, Event* event)
{
	
}

bool HomeScene::onContactBegin(PhysicsContact& contact)
{
	PhysicsBody* a = contact.getShapeA()->getBody();
	PhysicsBody* b = contact.getShapeB()->getBody();

	if ((a->getCollisionBitmask() == MainCharacter::MAIN_CHARACTER_BITMASK && b->getCollisionBitmask() == MainCharacter::OBSTACLE_BITMASK)
		|| (a->getCollisionBitmask() == MainCharacter::OBSTACLE_BITMASK && b->getCollisionBitmask() == MainCharacter::MAIN_CHARACTER_BITMASK))
	{
		if (MainCharacter::GetInstance()->GetCurrentState() == MainCharacter::ROLL_BACK || MainCharacter::GetInstance()->GetCurrentState() == MainCharacter::ROLL_LEFT ||
			MainCharacter::GetInstance()->GetCurrentState() == MainCharacter::ROLL_FRONT)
		{
			mainCharacter->stopAllActions();
		}
		if (MainCharacter::GetInstance()->GetDirection() == 1)
		{
			mainCharacter->setPositionY(mainCharacter->getPositionY() - MainCharacter::GetInstance()->GetSpeed());
			MainCharacter::GetInstance()->SetPreventRun(1);
		}
		else if (MainCharacter::GetInstance()->GetDirection() == 2)
		{
			mainCharacter->setPositionY(mainCharacter->getPositionY() + MainCharacter::GetInstance()->GetSpeed());
			MainCharacter::GetInstance()->SetPreventRun(2);
		}
		else if (MainCharacter::GetInstance()->GetDirection() == 3)
		{
			mainCharacter->setPositionX(mainCharacter->getPositionX() + MainCharacter::GetInstance()->GetSpeed());
			MainCharacter::GetInstance()->SetPreventRun(3);
		}
		else if (MainCharacter::GetInstance()->GetDirection() == 4)
		{
			mainCharacter->setPositionX(mainCharacter->getPositionX() - MainCharacter::GetInstance()->GetSpeed());
			MainCharacter::GetInstance()->SetPreventRun(4);
		}
		else if (MainCharacter::GetInstance()->GetDirection() == 5)
		{
			mainCharacter->setPosition(Vec2(mainCharacter->getPositionX() + MainCharacter::GetInstance()->GetPace(),
				mainCharacter->getPositionY() - MainCharacter::GetInstance()->GetPace()));
			MainCharacter::GetInstance()->SetPreventRun(5);
		}
		else if (MainCharacter::GetInstance()->GetDirection() == 6)
		{
			mainCharacter->setPosition(Vec2(mainCharacter->getPositionX() + MainCharacter::GetInstance()->GetPace(),
				mainCharacter->getPositionY() + MainCharacter::GetInstance()->GetPace()));
			MainCharacter::GetInstance()->SetPreventRun(6);
		}
		else if (MainCharacter::GetInstance()->GetDirection() == 7)
		{
			mainCharacter->setPosition(Vec2(mainCharacter->getPositionX() - MainCharacter::GetInstance()->GetPace(),
				mainCharacter->getPositionY() - MainCharacter::GetInstance()->GetPace()));
			MainCharacter::GetInstance()->SetPreventRun(7);
		}
		else if (MainCharacter::GetInstance()->GetDirection() == 8)
		{
			mainCharacter->setPosition(Vec2(mainCharacter->getPositionX() - MainCharacter::GetInstance()->GetPace(),
				mainCharacter->getPositionY() + MainCharacter::GetInstance()->GetPace()));
			MainCharacter::GetInstance()->SetPreventRun(8);
		}
	}

	// MAIN CHARACTER COLLIDE WITH WEAPON SHOP
	if ((a->getCollisionBitmask() == 11 && b->getCollisionBitmask() == MainCharacter::MAIN_CHARACTER_BITMASK)
		|| (a->getCollisionBitmask() == MainCharacter::MAIN_CHARACTER_BITMASK && b->getCollisionBitmask() == 11))
	{
		OpenWeaponShop();
	}

	// MAIN CHARACTER COLLIDE WITH EQUIPMENT SHOP
	if ((a->getCollisionBitmask() == 12 && b->getCollisionBitmask() == MainCharacter::MAIN_CHARACTER_BITMASK)
		|| (a->getCollisionBitmask() == MainCharacter::MAIN_CHARACTER_BITMASK && b->getCollisionBitmask() == 12))
	{
		OpenEquipmentShop();
	}

	// MAIN CHARACTER COLLIDE WITH POTION SHOP
	if ((a->getCollisionBitmask() == 13 && b->getCollisionBitmask() == MainCharacter::MAIN_CHARACTER_BITMASK)
		|| (a->getCollisionBitmask() == MainCharacter::MAIN_CHARACTER_BITMASK && b->getCollisionBitmask() == 13))
	{
		OpenPotionShop();
	}
	return true;
}

void HomeScene::OpenInventory(cocos2d::Ref * sender)
{
	for (int i = 0; i < 5; i++)
	{
		MainCharacter::GetInstance()->GetInventory()->GetTab(i)->removeAllChildren();
	}
	GamePlay::ShowInventoryGrid();
	MainCharacter::GetInstance()->GetInventory()->SetVisible(
		!(MainCharacter::GetInstance()->GetInventory()->IsVisible())
	);
	if (tab->isVisible())
	{
		tab->setVisible(false);
		health->setVisible(false);
		attack->setVisible(false);
		armor->setVisible(false);
		speedBoot->setVisible(false);
		arrowAttack->setVisible(false);
	}
}

void HomeScene::CreateAllButton(Layer* layer)
{
	anchorCamera = Camera::create();
	anchorCamera->setCameraFlag(CameraFlag::USER1);

	layer->addChild(anchorCamera);

	auto get = ResourceManager::GetInstance();
	auto visibleSize = Director::getInstance()->getVisibleSize();

	// CREATE JOYSTICK
	CreateJoystick(layer);

	// STATUS MC ID 0
	auto mainCharacterFace = get->GetButtonById(25);
	mainCharacterFace->setAnchorPoint(Vec2(0, 1));
	mainCharacterFace->removeFromParent();
	mainCharacterFace->setEnabled(true);
	mainCharacterFace->setPosition(Vec2(0, visibleSize.height));
	layer->addChild(mainCharacterFace, 4);
	mainCharacterFace->setCameraMask(2);
	m_buttons.push_back(mainCharacterFace);

	// NAME'S MAIN CHARACTER TEXT
	mName = get->GetLabelById(0);
	mName->setString(MainCharacter::GetInstance()->GetName());
	mName->removeFromParent();
	mName->setPosition(Vec2(mainCharacterFace->getBoundingBox().size.width + 10
		, visibleSize.height - mainCharacterFace->getBoundingBox().size.height / 2));
	mName->setAnchorPoint(Vec2(0, 0.5));
	mName->setCameraMask(2);
	layer->addChild(mName, 4);

	// SPRITE ID 0
	auto infoBar = get->GetSpriteById(11);
	infoBar->setAnchorPoint(Vec2(0, 1));
	infoBar->removeFromParent();
	infoBar->setPosition(Vec2(0, visibleSize.height - mainCharacterFace->getBoundingBox().size.height));
	layer->addChild(infoBar, 4);
	infoBar->setCameraMask(2);
	m_sprites.push_back(infoBar);
	auto infoBarPosition = infoBar->getPosition();
	auto infoBarSize = infoBar->getBoundingBox().size;

	//SPRITE ID 1
	auto hpBar = get->GetSpriteById(12);
	hpBar->removeFromParent();
	layer->addChild(hpBar, 5);
	hpBar->setPosition(Vec2(infoBar->getPositionX() / 2 + infoBarSize.width / 1.6, infoBar->getPositionY() - infoBarSize.height / 2.7));
	m_sprites.push_back(hpBar);
	hpBar->setCameraMask(2);

	//SPRITE ID 2
	auto mpBar = get->GetSpriteById(13);
	mpBar->removeFromParent();
	layer->addChild(mpBar, 5);
	mpBar->setPosition(Vec2(infoBar->getPositionX() / 2 + infoBarSize.width / 1.6, infoBar->getPositionY() - infoBarSize.height / 1.5));
	m_sprites.push_back(mpBar);
	mpBar->setCameraMask(2);

	hpLoadingBar = get->GetLoadingbar(1);
	hpLoadingBar->removeFromParent();
	hpLoadingBar->setPosition(hpBar->getPosition());
	layer->addChild(hpLoadingBar, 6);
	hpLoadingBar->setCameraMask(2);

	mpLoadingBar = get->GetLoadingbar(2);
	mpLoadingBar->removeFromParent();
	mpLoadingBar->setPosition(mpBar->getPosition());
	layer->addChild(mpLoadingBar, 6);
	mpLoadingBar->setCameraMask(2);

	//GOLD FRAME SPRITE ID 3 
	auto goldFrame = get->GetSpriteById(16);
	goldFrame->setAnchorPoint(Vec2(1, 1));
	goldFrame->removeFromParent();
	goldFrame->setPosition(Vec2(visibleSize.width / 1.5, visibleSize.height));
	layer->addChild(goldFrame, 4);
	m_sprites.push_back(goldFrame);
	goldFrame->setCameraMask(2);

	// AMOUNT OF GOLD'S MAINCHARACTER
	gold = get->GetLabelById(4);
	gold->removeFromParent();
	gold->setAnchorPoint(Vec2(1, 1));
	gold->setColor(Color3B::YELLOW);
	gold->setPosition(Vec2(visibleSize.width / 1.5, visibleSize.height
		- goldFrame->getBoundingBox().size.height / 2 + gold->getBoundingBox().size.height / 2));
	gold->setString(std::to_string(MainCharacter::GetInstance()->GetGold()));
	layer->addChild(gold, 5);
	gold->setCameraMask(2);

	// FOCUS SPRITE ID 4
	auto focus = get->GetSpriteById(28);
	focus->removeFromParent();
	layer->addChild(focus, 4);
	m_sprites.push_back(focus);

	// DIRECTION SPRITE ID 5
	auto direction = get->DuplicateSprite(get->GetSpriteById(14));
	direction->setScale(2);
	layer->addChild(direction, 4);
	direction->setPosition(Vec2(visibleSize.width - 70, visibleSize.height - 70));
	direction->setCameraMask(2);
	direction->setRotation(-90);
	direction->setVisible(false);
	m_sprites.push_back(direction);

	// GO TO MAP BUTTON ID 1
	auto map = get->GetButtonById(14);
	map->removeFromParent();
	layer->addChild(map, 4);
	map->setCameraMask(2);
	map->setPosition(Vec2(visibleSize.width / 2, 40));
	m_buttons.push_back(map);

	//BUTTON OPEN INVENTORY 2
	auto buttonOpenInventory = ui::Button::create("res/sprites/item/inventory.png");
	buttonOpenInventory->retain();
	layer->addChild(buttonOpenInventory, 4);
	buttonOpenInventory->setAnchorPoint(Vec2(0.5, 0));
	buttonOpenInventory->setEnabled(true);
	buttonOpenInventory->setPosition(Vec2(visibleSize.width / 2 - map->getBoundingBox().size.width * 2, map->getPositionY() - 15));
	m_buttons.push_back(buttonOpenInventory);
	buttonOpenInventory->setCameraMask(2);
	MainCharacter::GetInstance()->GetInventory()->SetSpritePosition(Vec2(visibleSize / 2));

	health = get->GetLabelById(7);
	health->removeFromParent();
	health->setCameraMask(2);
	health->setVisible(false);
	health->setAnchorPoint(Vec2(0, 0.5));
	health->setPosition(Vec2(visibleSize.width / 2 - 30, visibleSize.height / 2 + 83));
	health->setString(std::to_string(MainCharacter::GetInstance()->GetCurrentHP()) + "/" + std::to_string(MainCharacter::GetInstance()->GetMaxHP()));
	layer->addChild(health, 24);

	attack = get->GetLabelById(8);
	attack->removeFromParent();
	attack->setCameraMask(2);
	attack->setVisible(false);
	attack->setAnchorPoint(Vec2(0, 0.5));
	attack->setPosition(Vec2(visibleSize.width / 2 - 30, visibleSize.height / 2 + 24));
	attack->setString(std::to_string(MainCharacter::GetInstance()->GetAttack()) + " (+"
		+ std::to_string(MainCharacter::GetInstance()->GetAttack() - MainCharacter::ATTACK) + ")");
	layer->addChild(attack, 24);

	armor = get->GetLabelById(9);
	armor->removeFromParent();
	armor->setCameraMask(2);
	armor->setVisible(false);
	armor->setAnchorPoint(Vec2(0, 0.5));
	armor->setPosition(Vec2(visibleSize.width / 2 - 30, visibleSize.height / 2 - 33));
	armor->setString(std::to_string(MainCharacter::GetInstance()->GetDefend()) + " (+"
		+ std::to_string(MainCharacter::GetInstance()->GetDefend() - MainCharacter::DEFEND) + ")");
	layer->addChild(armor, 24);

	speedBoot = get->GetLabelById(10);
	speedBoot->removeFromParent();
	speedBoot->setCameraMask(2);
	speedBoot->setVisible(false);
	speedBoot->setAnchorPoint(Vec2(0, 0.5));
	speedBoot->setPosition(Vec2(visibleSize.width / 2 - 30, visibleSize.height / 2 - 84));
	speedBoot->setString(std::to_string(MainCharacter::GetInstance()->GetSpeed()) + " (+"
		+ std::to_string(MainCharacter::GetInstance()->GetSpeed() - MainCharacter::SPEED) + ")");
	layer->addChild(speedBoot, 24);

	arrowAttack = get->GetLabelById(11);
	arrowAttack->removeFromParent();
	arrowAttack->setCameraMask(2);
	arrowAttack->setVisible(false);
	arrowAttack->setAnchorPoint(Vec2(0, 0.5));
	arrowAttack->setPosition(Vec2(visibleSize.width / 2 - 30, visibleSize.height / 2 - 140));
	arrowAttack->setString(std::to_string(MainCharacter::GetInstance()->GetArrowAttack()));
	layer->addChild(arrowAttack, 24);

	SetCamera(MainCharacter::GetInstance()->GetSprite()->getPosition());
}

void HomeScene::CreateNPC()
{
	weaponSeller = ResourceManager::GetInstance()->GetSpriteById(20);
	weaponSeller->removeFromParent();
	this->addChild(weaponSeller, 0);
	auto obj1 = tileMap->objectGroupNamed("weaponSeller");
	float x1 = obj1->getObject("weaponSeller")["x"].asFloat();
	float y1 = obj1->getObject("weaponSeller")["y"].asFloat();
	weaponSeller->setPosition(x1, y1);
	weaponSeller->setAnchorPoint(Vec2(0.5, 0));
	weapon = ResourceManager::GetInstance()->GetActionById(32);
	CC_SAFE_RETAIN(weapon);
	auto ob1 = tileMap->getLayer("weaponShop");
	ob1->setVisible(false);
	Size layerSize1 = ob1->getLayerSize();
	for (int i = 0; i < layerSize1.width; i++)
	{
		for (int j = 0; j < layerSize1.height; j++)
		{
			auto tileSet1 = ob1->getTileAt(Vec2(i, j));
			if (tileSet1 != NULL)
			{
				auto physics1 = PhysicsBody::createBox(tileSet1->getContentSize(), PHYSICSBODY_MATERIAL_DEFAULT);
				physics1->setDynamic(false);
				physics1->setCollisionBitmask(11);
				physics1->setContactTestBitmask(true);
				tileSet1->setPhysicsBody(physics1);
			}
		}
	}

	equipmentSeller = ResourceManager::GetInstance()->GetSpriteById(19);
	equipmentSeller->removeFromParent();
	this->addChild(equipmentSeller, 0);
	auto obj2 = tileMap->objectGroupNamed("equipmentSeller");
	float x2 = obj2->getObject("equipmentSeller")["x"].asFloat();
	float y2 = obj2->getObject("equipmentSeller")["y"].asFloat();
	equipmentSeller->setPosition(x2, y2);
	equipmentSeller->setAnchorPoint(Vec2(0.5, 0));
	equipment = ResourceManager::GetInstance()->GetActionById(31);
	CC_SAFE_RETAIN(equipment);
	auto ob2 = tileMap->getLayer("equipmentShop");
	ob2->setVisible(false);
	Size layerSize2 = ob2->getLayerSize();
	for (int i = 0; i < layerSize2.width; i++)
	{
		for (int j = 0; j < layerSize2.height; j++)
		{
			auto tileSet2 = ob2->getTileAt(Vec2(i, j));
			if (tileSet2 != NULL)
			{
				auto physics2 = PhysicsBody::createBox(tileSet2->getContentSize(), PHYSICSBODY_MATERIAL_DEFAULT);
				physics2->setDynamic(false);
				physics2->setCollisionBitmask(12);
				physics2->setContactTestBitmask(true);
				tileSet2->setPhysicsBody(physics2);
			}
		}
	}
	
	potionSeller = ResourceManager::GetInstance()->GetSpriteById(21);
	potionSeller->removeFromParent();
	this->addChild(potionSeller, 0);
	auto obj3 = tileMap->objectGroupNamed("potionSeller");
	float x3 = obj3->getObject("potionSeller")["x"].asFloat();
	float y3 = obj3->getObject("potionSeller")["y"].asFloat();
	potionSeller->setPosition(x3, y3);
	potionSeller->setAnchorPoint(Vec2(0.5, 0));
	potion = ResourceManager::GetInstance()->GetActionById(33);
	CC_SAFE_RETAIN(potion);
	auto ob3 = tileMap->getLayer("potionShop");
	ob3->setVisible(false);
	Size layerSize3 = ob3->getLayerSize();
	for (int i = 0; i < layerSize3.width; i++)
	{
		for (int j = 0; j < layerSize3.height; j++)
		{
			auto tileSet3 = ob3->getTileAt(Vec2(i, j));
			if (tileSet3 != NULL)
			{
				auto physics3 = PhysicsBody::createBox(tileSet3->getContentSize(), PHYSICSBODY_MATERIAL_DEFAULT);
				physics3->setDynamic(false);
				physics3->setCollisionBitmask(13);
				physics3->setContactTestBitmask(true);
				tileSet3->setPhysicsBody(physics3);
			}
		}
	}
	CreateShop();
}

void HomeScene::RunActionNPC()
{
	if (weaponSeller->getNumberOfRunningActions() == 0)
	{
		weaponSeller->runAction(weapon);
	}
	if (equipmentSeller->getNumberOfRunningActions() == 0)
	{
		equipmentSeller->runAction(equipment);
	}
	if (potionSeller->getNumberOfRunningActions() == 0)
	{
		potionSeller->runAction(potion);
	}
}

void HomeScene::OpenWeaponShop()
{
	m_sprites[7]->setVisible(true);
	m_sprites[6]->setVisible(true);
	m_buttons[3]->setVisible(true);
	weaponscrollView->setVisible(true);
}

void HomeScene::OpenEquipmentShop()
{
	m_sprites[10]->setVisible(true);
	m_sprites[11]->setVisible(true);
	m_buttons[5]->setVisible(true);
	equipmentScrollView->setVisible(true);
}

void HomeScene::OpenPotionShop()
{
	m_sprites[8]->setVisible(true);
	m_sprites[9]->setVisible(true);
	m_buttons[4]->setVisible(true);
	potionScrollView->setVisible(true);
}

void HomeScene::CreateShop()
{
	Size visibleSize = Director::getInstance()->getVisibleSize();

	// WEAPON SHOP

	// BUTTON ID 3
	
	weaponscrollView = ui::ScrollView::create();
	weaponscrollView->setDirection(ui::ScrollView::Direction::VERTICAL);
	weaponscrollView->setContentSize(Size(517, 387-88)); //517,387
	weaponscrollView->setInnerContainerSize(Size(517, 880));
	weaponscrollView->setAnchorPoint(Vec2(0.5, 0.5));
	weaponscrollView->setPosition(Vec2(visibleSize/2)-Vec2(0,33));
	weaponscrollView->setVisible(false);
	weaponscrollView->setBounceEnabled(true);
	weaponscrollView->setCameraMask(2);
	weaponscrollView->setScrollBarColor(Color3B::WHITE);
	weaponscrollView->setScrollBarOpacity(0);
	addChild(weaponscrollView, 25);
	// SPRITE ID 6
	auto shopweapon = Sprite::create("res/sprites/shop/weaponShop.png");
	shopweapon->setPosition(visibleSize/2);
	shopweapon->setCameraMask(2);
	shopweapon->setVisible(false);
	addChild(shopweapon, 4);
	auto shopWeaponUP = Sprite::create("res/sprites/shop/weaponShopUp.png");
	shopWeaponUP->setPosition(visibleSize / 2);
	shopWeaponUP->setCameraMask(2);
	addChild(shopWeaponUP, 99);
	shopWeaponUP->setVisible(false);
	weaponscrollView->setCameraMask(2);
	m_sprites.push_back(shopweapon);
	m_sprites.push_back(shopWeaponUP);
	auto close = ui::Button::create("res/buttons/shopp/xx.png", "res/buttons/shopp/xx_bg.png");
	close->setPosition(Vec2(visibleSize.width / 2 + shopweapon->getBoundingBox().size.width / 2
		, visibleSize.height / 2 + shopweapon->getBoundingBox().size.height / 2));
	addChild(close, 25);
	close->setCameraMask(2);
	close->setVisible(false);
	m_buttons.push_back(close);
	close->addClickEventListener(CC_CALLBACK_1(HomeScene::OpenCloseWeaponShop, this));
	int rows = 0;
	for (int i = 17; i <= 20; i++)
	{
		auto itemSprite = Sprite::create("res/sprites/shop/item (" + std::to_string(i) + ").png");
		itemSprite->setAnchorPoint(Point(0, 1));
		itemSprite->setPosition(Vec2(25,
			weaponscrollView->getInnerContainerSize().height-rows*itemSprite->getContentSize().height));
		itemSprite->setCameraMask(2);
		weaponscrollView->addChild(itemSprite, 55);
		auto buttonBuy = ui::Button::create("res/buttons/shopp/btbuy.png", "res/buttons/shopp/btbuy_bg.png");
		buttonBuy->setAnchorPoint(Point(0, 1));
		buttonBuy->setPosition(Vec2(13+itemSprite->getPosition().x + itemSprite->getContentSize().width,
			itemSprite->getPosition().y-buttonBuy->getContentSize().height));
		buttonBuy->setCameraMask(2);
		buttonBuy->addClickEventListener(CC_CALLBACK_1(HomeScene::ClickBuyItem, this, i));
		weaponscrollView->addChild(buttonBuy, 56);
		rows++;
	}
	for (int i = 26; i <= 31; i++)
	{
		auto itemSprite = Sprite::create("res/sprites/shop/item (" + std::to_string(i) + ").png");
		itemSprite->setAnchorPoint(Point(0, 1));
		itemSprite->setPosition(Vec2(25,
			weaponscrollView->getInnerContainerSize().height - rows*itemSprite->getContentSize().height));
		itemSprite->setCameraMask(2);
		weaponscrollView->addChild(itemSprite, 55);
		auto buttonBuy = ui::Button::create("res/buttons/shopp/btbuy.png", "res/buttons/shopp/btbuy_bg.png");
		buttonBuy->setAnchorPoint(Point(0, 1));
		buttonBuy->setPosition(Vec2(13 + itemSprite->getPosition().x + itemSprite->getContentSize().width,
			itemSprite->getPosition().y - buttonBuy->getContentSize().height));
		buttonBuy->setCameraMask(2);
		buttonBuy->addClickEventListener(CC_CALLBACK_1(HomeScene::ClickBuyItem, this, i));
		weaponscrollView->addChild(buttonBuy, 56);
		rows++;
	}

	//POTION SHOP

	potionScrollView = ui::ScrollView::create();
	potionScrollView->setDirection(ui::ScrollView::Direction::VERTICAL);
	potionScrollView->setContentSize(Size(517, 387 - 88)); //517,387
	potionScrollView->setInnerContainerSize(Size(517, 440));
	potionScrollView->setAnchorPoint(Vec2(0.5, 0.5));
	potionScrollView->setPosition(Vec2(visibleSize / 2) - Vec2(0, 33));
	potionScrollView->setVisible(false);
	potionScrollView->setBounceEnabled(true);
	potionScrollView->setCameraMask(2);
	potionScrollView->setScrollBarColor(Color3B::WHITE);
	potionScrollView->setScrollBarOpacity(0);
	addChild(potionScrollView, 25);
	// SPRITE ID 8
	auto shopPotion = Sprite::create("res/sprites/shop/potionShop.png");
	shopPotion->setPosition(visibleSize / 2);
	shopPotion->setCameraMask(2);
	shopPotion->setVisible(false);
	addChild(shopPotion, 4);
	auto shopPotionUP = Sprite::create("res/sprites/shop/potionShopUp.png");
	shopPotionUP->setPosition(visibleSize / 2);
	shopPotionUP->setCameraMask(2);
	addChild(shopPotionUP, 99);
	shopPotionUP->setVisible(false);
	weaponscrollView->setCameraMask(2);
	m_sprites.push_back(shopPotion);
	m_sprites.push_back(shopPotionUP);
	auto closePotionShop = ui::Button::create("res/buttons/shopp/xx.png", "res/buttons/shopp/xx_bg.png");
	closePotionShop->setPosition(Vec2(visibleSize.width / 2 + shopPotion->getBoundingBox().size.width / 2
		, visibleSize.height / 2 + shopPotion->getBoundingBox().size.height / 2));
	addChild(closePotionShop, 25);
	closePotionShop->setCameraMask(2);
	closePotionShop->setVisible(false);
	m_buttons.push_back(closePotionShop);
	closePotionShop->addClickEventListener(CC_CALLBACK_1(HomeScene::OpenClosePotionShop, this));
	rows = 0;
	for (int i = 21; i <= 25; i++)
	{
		auto itemSprite = Sprite::create("res/sprites/shop/item (" + std::to_string(i) + ").png");
		itemSprite->setAnchorPoint(Point(0, 1));
		itemSprite->setPosition(Vec2(25,
			potionScrollView->getInnerContainerSize().height - rows*itemSprite->getContentSize().height));
		itemSprite->setCameraMask(2);
		potionScrollView->addChild(itemSprite, 55);
		auto buttonBuy = ui::Button::create("res/buttons/shopp/btbuy.png", "res/buttons/shopp/btbuy_bg.png");
		buttonBuy->setAnchorPoint(Point(0, 1));
		buttonBuy->setPosition(Vec2(13 + itemSprite->getPosition().x + itemSprite->getContentSize().width,
			itemSprite->getPosition().y - buttonBuy->getContentSize().height));
		buttonBuy->setCameraMask(2);
		buttonBuy->addClickEventListener(CC_CALLBACK_1(HomeScene::ClickBuyItem, this, i));
		potionScrollView->addChild(buttonBuy, 56);
		rows++;
	}
	
	//EQUIPMENT SHOP

	equipmentScrollView = ui::ScrollView::create();
	equipmentScrollView->setDirection(ui::ScrollView::Direction::VERTICAL);
	equipmentScrollView->setContentSize(Size(517, 387 - 88)); //517,387
	equipmentScrollView->setInnerContainerSize(Size(517, 1440));
	equipmentScrollView->setAnchorPoint(Vec2(0.5, 0.5));
	equipmentScrollView->setPosition(Vec2(visibleSize / 2) - Vec2(0, 33));
	equipmentScrollView->setVisible(false);
	equipmentScrollView->setBounceEnabled(true);
	equipmentScrollView->setCameraMask(2);
	equipmentScrollView->setScrollBarColor(Color3B::WHITE);
	equipmentScrollView->setScrollBarOpacity(0);
	addChild(equipmentScrollView, 25);
	// SPRITE ID 8
	auto shopEquipment = Sprite::create("res/sprites/shop/equipmentShop.png");
	shopEquipment->setPosition(visibleSize / 2);
	shopEquipment->setCameraMask(2);
	shopEquipment->setVisible(false);
	addChild(shopEquipment, 4);
	auto shopEquipmentUp = Sprite::create("res/sprites/shop/equipmentShopUp.png");
	shopEquipmentUp->setPosition(visibleSize / 2);
	shopEquipmentUp->setCameraMask(2);
	addChild(shopEquipmentUp, 99);
	shopEquipmentUp->setVisible(false);
	weaponscrollView->setCameraMask(2);
	m_sprites.push_back(shopEquipment);
	m_sprites.push_back(shopEquipmentUp);
	auto closeEquipmentShop = ui::Button::create("res/buttons/shopp/xx.png", "res/buttons/shopp/xx_bg.png");
	closeEquipmentShop->setPosition(Vec2(visibleSize.width / 2 + shopEquipment->getBoundingBox().size.width / 2
		, visibleSize.height / 2 + shopEquipment->getBoundingBox().size.height / 2));
	addChild(closeEquipmentShop, 25);
	closeEquipmentShop->setCameraMask(2);
	closeEquipmentShop->setVisible(false);
	m_buttons.push_back(closeEquipmentShop);
	closeEquipmentShop->addClickEventListener(CC_CALLBACK_1(HomeScene::OpenCloseEquipmentShop, this));
	rows = 0;
	for (int i = 1; i <= 16; i++)
	{
		auto itemSprite = Sprite::create("res/sprites/shop/item (" + std::to_string(i) + ").png");
		itemSprite->setAnchorPoint(Point(0, 1));
		itemSprite->setPosition(Vec2(25,
			equipmentScrollView->getInnerContainerSize().height - rows*itemSprite->getContentSize().height));
		itemSprite->setCameraMask(2);
		equipmentScrollView->addChild(itemSprite, 55);
		auto buttonBuy = ui::Button::create("res/buttons/shopp/btbuy.png", "res/buttons/shopp/btbuy_bg.png");
		buttonBuy->setAnchorPoint(Point(0, 1));
		buttonBuy->setPosition(Vec2(13 + itemSprite->getPosition().x + itemSprite->getContentSize().width,
			itemSprite->getPosition().y - buttonBuy->getContentSize().height));
		buttonBuy->setCameraMask(2);
		buttonBuy->addClickEventListener(CC_CALLBACK_1(HomeScene::ClickBuyItem, this, i));
		equipmentScrollView->addChild(buttonBuy, 56);
		rows++;
	}
	
}

void HomeScene::ClickShowInfor(Ref * pSender)
{
	tab->setVisible(!tab->isVisible());
	ShowInfor();
	health->setVisible(!health->isVisible());
	attack->setVisible(!attack->isVisible());
	armor->setVisible(!armor->isVisible());
	speedBoot->setVisible(!speedBoot->isVisible());
	arrowAttack->setVisible(!arrowAttack->isVisible());
	health->setString(std::to_string(MainCharacter::GetInstance()->GetCurrentHP()) + "/" + std::to_string(MainCharacter::GetInstance()->GetMaxHP()));
	attack->setString(std::to_string(MainCharacter::GetInstance()->GetAttack()) + " (+"
		+ std::to_string(MainCharacter::GetInstance()->GetAttack() - MainCharacter::ATTACK) + ")");
	armor->setString(std::to_string(MainCharacter::GetInstance()->GetDefend()) + " (+"
		+ std::to_string(MainCharacter::GetInstance()->GetDefend() - MainCharacter::DEFEND) + ")");
	speedBoot->setString(std::to_string(MainCharacter::GetInstance()->GetSpeed()) + " (+"
		+ std::to_string(MainCharacter::GetInstance()->GetSpeed() - MainCharacter::SPEED) + ")");
	arrowAttack->setString(std::to_string(MainCharacter::GetInstance()->GetArrowAttack()));
	if (MainCharacter::GetInstance()->GetInventory()->IsVisible())
	{
		MainCharacter::GetInstance()->GetInventory()->SetVisible(false);
	}
}

void HomeScene::ClickBuyItem(Ref * sender, int id)
{
	SimpleAudioEngine::getInstance()->playEffect("audio/inventorySoundEffects/sellBuyItem.wav", false);
	switch (id)
	{
	case 1:
		if (MainCharacter::GetInstance()->GetGold() >= 300 && MainCharacter::GetInstance()->GetInventory()->GetCapacity(ItemType::armor)<24)
		{
			MainCharacter::GetInstance()->GetInventory()->AddItem(id);
			MainCharacter::GetInstance()->SubGold(300);
		}
		break;
	case 2:
		if (MainCharacter::GetInstance()->GetGold() >= 500 && MainCharacter::GetInstance()->GetInventory()->GetCapacity(ItemType::armor)<24)
		{
			MainCharacter::GetInstance()->GetInventory()->AddItem(id);
			MainCharacter::GetInstance()->SubGold(500);
		}
		break;
	case 3:
		if (MainCharacter::GetInstance()->GetGold() >= 1000 && MainCharacter::GetInstance()->GetInventory()->GetCapacity(ItemType::armor)<24)
		{
			MainCharacter::GetInstance()->GetInventory()->AddItem(id);
			MainCharacter::GetInstance()->SubGold(1000);
		}
		break;
	case 4:
		if (MainCharacter::GetInstance()->GetGold() >= 2000 && MainCharacter::GetInstance()->GetInventory()->GetCapacity(ItemType::armor)<24)
		{
			MainCharacter::GetInstance()->GetInventory()->AddItem(id);
			MainCharacter::GetInstance()->SubGold(2000);
		}
		break;
	case 5:
		if (MainCharacter::GetInstance()->GetGold() >= 5000 && MainCharacter::GetInstance()->GetInventory()->GetCapacity(ItemType::armor)<24)
		{
			MainCharacter::GetInstance()->GetInventory()->AddItem(id);
			MainCharacter::GetInstance()->SubGold(5000);
		}
		break;
	case 6:
		if (MainCharacter::GetInstance()->GetGold() >= 3 && MainCharacter::GetInstance()->GetInventory()->GetCapacity(ItemType::arrow)<24)
		{
			MainCharacter::GetInstance()->GetInventory()->AddItem(id);
			MainCharacter::GetInstance()->SubGold(3);
		}
		break;
	case 7:
		if (MainCharacter::GetInstance()->GetGold() >= 5 && MainCharacter::GetInstance()->GetInventory()->GetCapacity(ItemType::arrow)<24)
		{
			MainCharacter::GetInstance()->GetInventory()->AddItem(id);
			MainCharacter::GetInstance()->SubGold(5);
		}
		break;
	case 8:
		if (MainCharacter::GetInstance()->GetGold() >= 8 && MainCharacter::GetInstance()->GetInventory()->GetCapacity(ItemType::arrow)<24)
		{
			MainCharacter::GetInstance()->GetInventory()->AddItem(id);
			MainCharacter::GetInstance()->SubGold(8);
		}
		break;
	case 9:
		if (MainCharacter::GetInstance()->GetGold() >= 12 && MainCharacter::GetInstance()->GetInventory()->GetCapacity(ItemType::arrow)<24)
		{
			MainCharacter::GetInstance()->GetInventory()->AddItem(id);
			MainCharacter::GetInstance()->SubGold(12);
		}
		break;
	case 10:
		if (MainCharacter::GetInstance()->GetGold() >= 15 && MainCharacter::GetInstance()->GetInventory()->GetCapacity(ItemType::arrow)<24)
		{
			MainCharacter::GetInstance()->GetInventory()->AddItem(id);
			MainCharacter::GetInstance()->SubGold(15);
		}
		break;
	case 11:
		if (MainCharacter::GetInstance()->GetGold() >= 20 && MainCharacter::GetInstance()->GetInventory()->GetCapacity(ItemType::arrow)<24)
		{
			MainCharacter::GetInstance()->GetInventory()->AddItem(id);
			MainCharacter::GetInstance()->SubGold(20);
		}
		break;
	case 12:
		if (MainCharacter::GetInstance()->GetGold() >= 25 && MainCharacter::GetInstance()->GetInventory()->GetCapacity(ItemType::arrow)<24)
		{
			MainCharacter::GetInstance()->GetInventory()->AddItem(id);
			MainCharacter::GetInstance()->SubGold(25);
		}
		break;
	case 13:
		if (MainCharacter::GetInstance()->GetGold() >= 200 && MainCharacter::GetInstance()->GetInventory()->GetCapacity(ItemType::boots)<24)
		{
			MainCharacter::GetInstance()->GetInventory()->AddItem(id);
			MainCharacter::GetInstance()->SubGold(200);
		}
		break;
	case 14:
		if (MainCharacter::GetInstance()->GetGold() >= 500 && MainCharacter::GetInstance()->GetInventory()->GetCapacity(ItemType::boots)<24)
		{
			MainCharacter::GetInstance()->GetInventory()->AddItem(id);
			MainCharacter::GetInstance()->SubGold(500);
		}
		break;
	case 15:
		if (MainCharacter::GetInstance()->GetGold() >= 1000 && MainCharacter::GetInstance()->GetInventory()->GetCapacity(ItemType::boots)<24)
		{
			MainCharacter::GetInstance()->GetInventory()->AddItem(id);
			MainCharacter::GetInstance()->SubGold(1000);
		}
		break;
	case 16:
		if (MainCharacter::GetInstance()->GetGold() >= 2000 && MainCharacter::GetInstance()->GetInventory()->GetCapacity(ItemType::boots)<24)
		{
			MainCharacter::GetInstance()->GetInventory()->AddItem(id);
			MainCharacter::GetInstance()->SubGold(2000);
		}
		break;
	case 17:
		if (MainCharacter::GetInstance()->GetGold() >= 200 && MainCharacter::GetInstance()->GetInventory()->GetCapacity(ItemType::weapon)<24)
		{
			MainCharacter::GetInstance()->GetInventory()->AddItem(id);
			MainCharacter::GetInstance()->SubGold(200);
		}
		break;
	case 18:
		if (MainCharacter::GetInstance()->GetGold() >= 500 && MainCharacter::GetInstance()->GetInventory()->GetCapacity(ItemType::weapon)<24)
		{
			MainCharacter::GetInstance()->GetInventory()->AddItem(id);
			MainCharacter::GetInstance()->SubGold(500);
		}
		break;
	case 19:
		if (MainCharacter::GetInstance()->GetGold() >= 1000 && MainCharacter::GetInstance()->GetInventory()->GetCapacity(ItemType::weapon)<24)
		{
			MainCharacter::GetInstance()->GetInventory()->AddItem(id);
			MainCharacter::GetInstance()->SubGold(1000);
		}
		break;
	case 20:
		if (MainCharacter::GetInstance()->GetGold() >= 2000 && MainCharacter::GetInstance()->GetInventory()->GetCapacity(ItemType::weapon)<24)
		{
			MainCharacter::GetInstance()->GetInventory()->AddItem(id);
			MainCharacter::GetInstance()->SubGold(2000);
		}
		break;
	case 21:
		if (MainCharacter::GetInstance()->GetGold() >= 25 && MainCharacter::GetInstance()->GetInventory()->GetCapacity(ItemType::potion)<24)
		{
			MainCharacter::GetInstance()->GetInventory()->AddItem(id);
			MainCharacter::GetInstance()->SubGold(25);
		}
		break;
	case 22:
		if (MainCharacter::GetInstance()->GetGold() >= 20 && MainCharacter::GetInstance()->GetInventory()->GetCapacity(ItemType::potion)<24)
		{
			MainCharacter::GetInstance()->GetInventory()->AddItem(id);
			MainCharacter::GetInstance()->SubGold(20);
		}
		break;
	case 23:
		if (MainCharacter::GetInstance()->GetGold() >= 22 && MainCharacter::GetInstance()->GetInventory()->GetCapacity(ItemType::potion)<24)
		{
			MainCharacter::GetInstance()->GetInventory()->AddItem(id);
			MainCharacter::GetInstance()->SubGold(22);
		}
		break;
	case 24:
		if (MainCharacter::GetInstance()->GetGold() >= 15 && MainCharacter::GetInstance()->GetInventory()->GetCapacity(ItemType::potion)<24)
		{
			MainCharacter::GetInstance()->GetInventory()->AddItem(id);
			MainCharacter::GetInstance()->SubGold(15);
		}
		break;
	case 25:
		if (MainCharacter::GetInstance()->GetGold() >= 45 && MainCharacter::GetInstance()->GetInventory()->GetCapacity(ItemType::potion)<24)
		{
			MainCharacter::GetInstance()->GetInventory()->AddItem(id);
			MainCharacter::GetInstance()->SubGold(45);
		}
		break;
	case 26:
		if (MainCharacter::GetInstance()->GetGold() >= 200 && MainCharacter::GetInstance()->GetInventory()->GetCapacity(ItemType::weapon)<24)
		{
			MainCharacter::GetInstance()->GetInventory()->AddItem(id);
			MainCharacter::GetInstance()->SubGold(200);
		}
		break;
	case 27:
		if (MainCharacter::GetInstance()->GetGold() >= 300 && MainCharacter::GetInstance()->GetInventory()->GetCapacity(ItemType::weapon)<24)
		{
			MainCharacter::GetInstance()->GetInventory()->AddItem(id);
			MainCharacter::GetInstance()->SubGold(300);
		}
		break;
	case 28:
		if (MainCharacter::GetInstance()->GetGold() >= 500 && MainCharacter::GetInstance()->GetInventory()->GetCapacity(ItemType::weapon)<24)
		{
			MainCharacter::GetInstance()->GetInventory()->AddItem(id);
			MainCharacter::GetInstance()->SubGold(500);
		}
		break;
	case 29:
		if (MainCharacter::GetInstance()->GetGold() >= 700 && MainCharacter::GetInstance()->GetInventory()->GetCapacity(ItemType::weapon)<24)
		{
			MainCharacter::GetInstance()->GetInventory()->AddItem(id);
			MainCharacter::GetInstance()->SubGold(700);
		}
		break;
	case 30:
		if (MainCharacter::GetInstance()->GetGold() >= 1000 && MainCharacter::GetInstance()->GetInventory()->GetCapacity(ItemType::weapon)<24)
		{
			MainCharacter::GetInstance()->GetInventory()->AddItem(id);
			MainCharacter::GetInstance()->SubGold(1000);
		}
		break;
	case 31:
		if (MainCharacter::GetInstance()->GetGold() >= 3000 && MainCharacter::GetInstance()->GetInventory()->GetCapacity(ItemType::weapon)<24)
		{
			MainCharacter::GetInstance()->GetInventory()->AddItem(id);
			MainCharacter::GetInstance()->SubGold(3000);
		}
		break;
	default:
		break;
	}


}

void HomeScene::OpenCloseWeaponShop(Ref * pSender)
{
	m_buttons[3]->setVisible(!m_buttons[3]->isVisible());
	weaponscrollView->setVisible(!weaponscrollView->isVisible());
	m_sprites[6]->setVisible(!m_sprites[6]->isVisible());
	m_sprites[7]->setVisible(!m_sprites[7]->isVisible());
}

void HomeScene::OpenClosePotionShop(cocos2d::Ref * pSender)
{
	m_buttons[4]->setVisible(!m_buttons[4]->isVisible());
	potionScrollView->setVisible(!potionScrollView->isVisible());
	m_sprites[8]->setVisible(!m_sprites[8]->isVisible());
	m_sprites[9]->setVisible(!m_sprites[9]->isVisible());
}

void HomeScene::OpenCloseEquipmentShop(cocos2d::Ref * pSender)
{
	m_buttons[5]->setVisible(!m_buttons[5]->isVisible());
	equipmentScrollView->setVisible(!equipmentScrollView->isVisible());
	m_sprites[10]->setVisible(!m_sprites[10]->isVisible());
	m_sprites[11]->setVisible(!m_sprites[11]->isVisible());
}
