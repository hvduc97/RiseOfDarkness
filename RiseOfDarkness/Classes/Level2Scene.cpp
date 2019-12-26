#include "Level2Scene.h"
#include "MainCharacter.h"
#include "ResourceManager.h"
#include "MapScene.h"
#include "HomeScene.h"
#include "Monster.h"
#include "Nokken.h"
#include "SimpleAudioEngine.h"
#include "Level1Scene.h"
using namespace CocosDenshion;

using namespace std;

USING_NS_CC;

Scene* Level2Scene::CreateScene()
{
	auto scene = Scene::createWithPhysics();
	//scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
	scene->getPhysicsWorld()->setGravity(Vec2(0, 0));
	auto layer = Level2Scene::create();

	scene->addChild(layer);

	return scene;
}

bool Level2Scene::init()
{
	if (!Layer::init())
	{
		return false;
	}

	SimpleAudioEngine::getInstance()->stopBackgroundMusic(true);

	MainCharacter::GetInstance()->Refresh();

	currentStage = 1;

	tileMap = ResourceManager::GetInstance()->GetTileMapById(5);
	upperTileMap = ResourceManager::GetInstance()->GetTileMapById(6);

	CreatePhysicsWorld("obstacles", "mc", "river", this);

	CreateMonster();

	CreateAllButton(this);

	AddListener();

	if (!MainCharacter::GetInstance()->GetCheckHeartCollect(currentStage))
	{
		CreateTreasure();
	}

	InitChest(this);

	scheduleUpdate();

	return true;
}

void Level2Scene::update(float deltaTime)
{
	amountEnemy->setString("X " + std::to_string(GetAliveEnemies()));

	UpdateInfoBar();

	MainCharacter::GetInstance()->Update(deltaTime);

	SetCamera(mainCharacter->getPosition());

	EnemyUpdate(deltaTime);

	if (CheckClear())
	{
		if (MainCharacter::GetInstance()->GetStageLevel() == currentStage)
		{
			MainCharacter::GetInstance()->IncreaseStage();
		}
		if (!clear->isVisible())
		{
			SimpleAudioEngine::getInstance()->playEffect("audio/win.wav", false);
		}
		clear->setVisible(true);
		m_buttons[4]->setVisible(false);
		m_buttons[5]->setVisible(false);
		m_buttons[6]->setVisible(true);
		//m_buttons[7]->setVisible(true);
		nextStage->setVisible(true);
		shader->setOpacity(200);
		if (MainCharacter::GetInstance()->GetInventory()->IsVisible())
		{
			MainCharacter::GetInstance()->GetInventory()->SetVisible(false);
		}
		if (tab->isVisible())
		{
			tab->setVisible(false);
			health->setVisible(false);
			attack->setVisible(false);
			armor->setVisible(false);
			speedBoot->setVisible(false);
			arrowAttack->setVisible(false);
		}
		//m_buttons[8]->setEnabled(false);
		m_buttons[9]->setEnabled(false);
		m_buttons[10]->setEnabled(false);
		m_buttons[11]->setEnabled(false);
	}

	if (!MainCharacter::GetInstance()->IsAlive())
	{
		m_buttons[4]->setVisible(false);
		m_buttons[5]->setVisible(false);
		m_buttons[6]->setVisible(true);
		m_buttons[7]->setVisible(true);
		gameover->setVisible(true);
		shader->setOpacity(200);
		if (MainCharacter::GetInstance()->GetInventory()->IsVisible())
		{
			MainCharacter::GetInstance()->GetInventory()->SetVisible(false);
		}
		if (tab->isVisible())
		{
			tab->setVisible(false);
			health->setVisible(false);
			attack->setVisible(false);
			armor->setVisible(false);
			speedBoot->setVisible(false);
			arrowAttack->setVisible(false);
		}
		if (m_buttons[8]->isEnabled())
		{
			SimpleAudioEngine::getInstance()->playEffect("audio/lose2.wav", false);
		}
		m_buttons[8]->setEnabled(false);
		m_buttons[9]->setEnabled(false);
		m_buttons[10]->setEnabled(false);
		m_buttons[11]->setEnabled(false);
	}
	
	UpdateJoystick();

	MainCharacter::GetInstance()->GetFlySlash()->Update(deltaTime);

	gold->setString(std::to_string(MainCharacter::GetInstance()->GetGold()));
}

void Level2Scene::CreateMonster()
{
	float x1, y1;
	int direction1;
	auto nokenGroup = tileMap->getObjectGroup("nokken");
	int amount1 = 3;
	char str1[10];
	for (int i = 1; i <= amount1; i++)
	{
		sprintf(str1, "%02d", i);
		x1 = nokenGroup->getObject(str1)["x"].asFloat();
		y1 = nokenGroup->getObject(str1)["y"].asFloat();
		Nokken *nokken = new Nokken(this, Vec2(x1, y1), i - 1);
		nokken->GetPhysicsBody()->setGroup(i - 1);
		m_enemies.push_back(nokken);
	}

	float x2, y2;
	int direction2;
	auto ropeGroup = tileMap->getObjectGroup("rope");
	int amount2 = 4;
	char str2[10];
	for (int i = 1; i <= amount2; i++)
	{
		sprintf(str2, "%02d", i);
		direction2 = ropeGroup->getObject(str2)["direction"].asInt();
		x2 = ropeGroup->getObject(str2)["x"].asFloat();
		y2 = ropeGroup->getObject(str2)["y"].asFloat();
		Monster *monster = new Monster(this, direction2, Vec2(x2, y2), i - 1 + amount1);
		monster->GetPhysicsBody()->setGroup(i - 1 + amount1);
		m_enemies.push_back(monster);
	}
}

void Level2Scene::CreatePhysicsWorld(const char * obstacle, const char * mc, const char * river, Layer * layer)
{
	// Create camera
	camera = Camera::create();
	layer->addChild(camera, 10);

	// Add tile map
	tileMap->removeFromParent();
	tileMap->setPosition(Vec2(0, 0));
	layer->addChild(tileMap, 0);

	// Create physics body world
	auto edgeBody = PhysicsBody::createEdgeBox(tileMap->getContentSize(), PHYSICSBODY_MATERIAL_DEFAULT);
	edgeBody->setCollisionBitmask(MainCharacter::OBSTACLE_BITMASK);
	edgeBody->setContactTestBitmask(true);
	auto edgeNode = Node::create();
	edgeNode->setPosition(tileMap->getContentSize() / 2);
	edgeNode->setPhysicsBody(edgeBody);
	layer->addChild(edgeNode);

	// Create box 2d for obstacles
	auto ob = tileMap->getLayer(obstacle);
	ob->setVisible(false);
	Size layerSize = ob->getLayerSize();
	for (int i = 0; i < layerSize.width; i++)
	{
		for (int j = 0; j < layerSize.height; j++)
		{
			auto tileSet = ob->getTileAt(Vec2(i, j));
			if (tileSet != NULL)
			{
				auto physics = PhysicsBody::createBox(tileSet->getContentSize(), PHYSICSBODY_MATERIAL_DEFAULT);
				physics->setDynamic(false);
				physics->setCollisionBitmask(MainCharacter::OBSTACLE_BITMASK);
				physics->setContactTestBitmask(true);
				tileSet->setPhysicsBody(physics);
			}
		}
	}

	// Add upper tile map
	upperTileMap->removeFromParent();
	upperTileMap->setPosition(Vec2(0, 0));
	layer->addChild(upperTileMap, 3);

	// Add main character
	mainCharacter = MainCharacter::GetInstance()->GetSprite();
	mainCharacter->removeFromParent();
	MainCharacter::GetInstance()->AddToLayer(layer);
	auto obj = tileMap->objectGroupNamed(mc);
	float x = obj->getObject(mc)["x"].asFloat();
	float y = obj->getObject(mc)["y"].asFloat();
	mainCharacter->setPosition(x, y);

	// Get body
	body = MainCharacter::GetInstance()->GetPhysicsBody();

	// Create box 2d for river
	auto rv = tileMap->getLayer(river);
	rv->setVisible(false);
	Size layerRVSize = rv->getLayerSize();
	for (int i = 0; i < layerRVSize.width; i++)
	{
		for (int j = 0; j < layerRVSize.height; j++)
		{
			auto tileSet = rv->getTileAt(Vec2(i, j));
			if (tileSet != NULL)
			{
				auto physics = PhysicsBody::createBox(tileSet->getContentSize(), PHYSICSBODY_MATERIAL_DEFAULT);
				physics->setDynamic(false);
				physics->setCollisionBitmask(MainCharacter::RIVER_BITMASK);
				physics->setContactTestBitmask(true);
				tileSet->setPhysicsBody(physics);
			}
		}
	}
}

void Level2Scene::AddListener()
{
	auto touchListener = EventListenerTouchOneByOne::create();
	touchListener->onTouchBegan = CC_CALLBACK_2(Level2Scene::OnTouchBegan, this);
	touchListener->onTouchEnded = CC_CALLBACK_2(Level2Scene::OnTouchEnded, this);
	touchListener->onTouchMoved = CC_CALLBACK_2(Level2Scene::OnTouchMoved, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);

	// SKILLS
	m_buttons[1]->addTouchEventListener(CC_CALLBACK_2(Level2Scene::SpecialAttack, this));
	m_buttons[2]->addTouchEventListener(CC_CALLBACK_2(Level2Scene::Evade, this));
	m_buttons[0]->addTouchEventListener(CC_CALLBACK_2(Level2Scene::NormalAttack, this));
	m_buttons[3]->addTouchEventListener(CC_CALLBACK_2(Level2Scene::Defend, this));

	// PAUSE GAME
	m_buttons[4]->addClickEventListener([&](Ref* event) {
		if (!m_buttons[5]->isVisible())
		{
			m_buttons[4]->setVisible(false);
			m_buttons[5]->setVisible(true);
			m_buttons[6]->setVisible(true);
			m_buttons[7]->setVisible(true);
			paused->setVisible(true);
			shader->setOpacity(200);
			Director::getInstance()->pause();
			if (MainCharacter::GetInstance()->GetInventory()->IsVisible())
			{
				MainCharacter::GetInstance()->GetInventory()->SetVisible(false);
			}
			if (tab->isVisible())
			{
				tab->setVisible(false);
				health->setVisible(false);
				attack->setVisible(false);
				armor->setVisible(false);
				speedBoot->setVisible(false);
				arrowAttack->setVisible(false);
			}
			m_buttons[8]->setEnabled(false);
			m_buttons[9]->setEnabled(false);
			m_buttons[10]->setEnabled(false);
			m_buttons[11]->setEnabled(false);
		}
	});

	// RESUME GAME
	m_buttons[5]->addClickEventListener([&](Ref* event) {
		m_buttons[4]->setVisible(true);
		m_buttons[5]->setVisible(false);
		m_buttons[6]->setVisible(false);
		m_buttons[7]->setVisible(false);
		shader->setOpacity(0);
		paused->setVisible(false);
		Director::getInstance()->resume();
		m_buttons[8]->setEnabled(true);
		m_buttons[9]->setEnabled(true);
		m_buttons[10]->setEnabled(true);
		m_buttons[11]->setEnabled(true);
	});

	// GO TO HOMESCENE
	m_buttons[6]->addClickEventListener([&](Ref* event) {
		Director::getInstance()->resume();
		auto gotoMap = CallFunc::create([] {
			Director::getInstance()->replaceScene(HomeScene::CreateScene());
		});
		runAction(gotoMap);
	});

	// RESTART GAME STAGE
	m_buttons[7]->addClickEventListener([&](Ref* event) {
		Director::getInstance()->resume();
		auto gotoMap = CallFunc::create([] {
			Director::getInstance()->replaceScene(Level2Scene::CreateScene());
		});
		runAction(gotoMap);
	});

	// USE HP POTION
	m_buttons[9]->addClickEventListener([&](Ref* event) {
		if (MainCharacter::GetInstance()->GetPercentHP() < 100)
		{
			int index = MainCharacter::GetInstance()->GetInventory()->GetIdByIcon(21, ItemType::potion);
			MainCharacter::GetInstance()->GetInventory()->RemoveItem(21, index, ItemType::potion);
			if (!MainCharacter::GetInstance()->GetInventory()->InventoryContains(21, ItemType::potion))
			{
				amountHP->setString("0");
			}
			else
			{
				amountHP->setString(std::to_string(MainCharacter::GetInstance()->GetInventory()->GetItemAmount(0)
					[MainCharacter::GetInstance()->GetInventory()->GetIdByIcon(21, ItemType::potion)]));
			}
		}
	});

	// USE MP POTION
	m_buttons[10]->addClickEventListener([&](Ref* event) {
		if (MainCharacter::GetInstance()->GetPercentMP() < 100)
		{
			int index = MainCharacter::GetInstance()->GetInventory()->GetIdByIcon(22, ItemType::potion);
			MainCharacter::GetInstance()->GetInventory()->RemoveItem(22, index, ItemType::potion);
			if (!MainCharacter::GetInstance()->GetInventory()->InventoryContains(22, ItemType::potion))
			{
				amountMP->setString("0");
			}
			else
			{
				amountMP->setString(std::to_string(MainCharacter::GetInstance()->GetInventory()->GetItemAmount(0)
					[MainCharacter::GetInstance()->GetInventory()->GetIdByIcon(22, ItemType::potion)]));
			}
		}
	});

	// INVENTORY
	m_buttons[11]->addClickEventListener(CC_CALLBACK_1(Level2Scene::OpenInventory, this));

	auto contactListener = EventListenerPhysicsContact::create();
	contactListener->onContactBegin = CC_CALLBACK_1(Level2Scene::onContactBegin, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);

	// STATUS
	m_buttons[8]->addClickEventListener(CC_CALLBACK_1(Level2Scene::ClickShowInfor, this));

	nextStage->addClickEventListener([&](Ref* event) {
		CreateMonster();
		auto gotoMap = CallFunc::create([] {
			Director::getInstance()->replaceScene(Level1Scene::CreateScene());
		});
		runAction(gotoMap);
	});
}

bool Level2Scene::OnTouchBegan(Touch* touch, Event* event)
{
	return true;
}

bool Level2Scene::OnTouchEnded(Touch* touch, Event* event)
{
	return true;
}

void Level2Scene::OnTouchMoved(Touch* touch, Event* event)
{

}

bool Level2Scene::onContactBegin(PhysicsContact& contact)
{
	// MAIN CHARACTER WITH OBSTACLES
	Collision(contact, MainCharacter::MAIN_CHARACTER_BITMASK, MainCharacter::OBSTACLE_BITMASK, 1);
	
	// MAIN CHARACTER WITH MONSTER
	Collision(contact, MainCharacter::MAIN_CHARACTER_BITMASK, MainCharacter::ROPE_MONSTER_BITMASK, 1);

	// MAIN CHARACTER WITH RIVER
	Collision(contact, MainCharacter::MAIN_CHARACTER_BITMASK, MainCharacter::RIVER_BITMASK, 1);

	// ARROW COLLIDE WITH OBSTACLE
	Collision(contact, MainCharacter::NORMAL_ARROW_BITMASK, MainCharacter::OBSTACLE_BITMASK, 2);

	// MAIN CHARACTER SLASH ROPE MONSTER
	Collision(contact, MainCharacter::SLASH_BITMASK, MainCharacter::ROPE_MONSTER_BITMASK, 3);
	
	// MAIN CHARACTER SLASH NOKKEN MONSTER
	Collision(contact, MainCharacter::SLASH_BITMASK, MainCharacter::NOKKEN_MONSTER_BITMASK, 3);

	// MAIN CHARACTER'S ARROW COLLIDE ROPE MONSTER
	Collision(contact, MainCharacter::NORMAL_ARROW_BITMASK, MainCharacter::ROPE_MONSTER_BITMASK, 4);

	// MAIN CHARACTER'S ARROW COLLIDE NOKKEN MONSTER
	Collision(contact, MainCharacter::NORMAL_ARROW_BITMASK, MainCharacter::NOKKEN_MONSTER_BITMASK, 4);
	
	// BULLET DAMAGE MAIN CHARACTER
	Collision(contact, MainCharacter::BULLET_BITMASK, MainCharacter::MAIN_CHARACTER_BITMASK, 6);
	
	// ROPE MONSTER COLLIDE OBSTACLES
	Collision(contact, MainCharacter::OBSTACLE_BITMASK, MainCharacter::ROPE_MONSTER_BITMASK, 5);

	// ROPE MONSTER COLLIDE RIVER
	Collision(contact, MainCharacter::RIVER_BITMASK, MainCharacter::ROPE_MONSTER_BITMASK, 5);

	// BULLET COLLIDE OBSTACLES
	Collision(contact, MainCharacter::BULLET_BITMASK, MainCharacter::OBSTACLE_BITMASK, 7);

	PhysicsBody* a = contact.getShapeA()->getBody();
	PhysicsBody* b = contact.getShapeB()->getBody();

	// COLLECT HEART CONTAINER
	if ((a->getCollisionBitmask() == MainCharacter::MAIN_CHARACTER_BITMASK && b->getCollisionBitmask() == MainCharacter::HEART_CONTAINER_BITMASK)
		|| (a->getCollisionBitmask() == MainCharacter::HEART_CONTAINER_BITMASK && b->getCollisionBitmask() == MainCharacter::MAIN_CHARACTER_BITMASK))
	{
		if (heartContainer->isVisible())
		{
			MainCharacter::GetInstance()->TakeHeartContainer();
			heartContainer->setVisible(false);
			MainCharacter::GetInstance()->SetHeartCollected(currentStage);
		}
	}

	// FLY SLASH COLLIDE OBSTACLES
	if ((a->getCollisionBitmask() == MainCharacter::FLY_SLASH_BITMASK && b->getCollisionBitmask() == MainCharacter::OBSTACLE_BITMASK)
		|| (a->getCollisionBitmask() == MainCharacter::OBSTACLE_BITMASK && b->getCollisionBitmask() == MainCharacter::FLY_SLASH_BITMASK))
	{
		MainCharacter::GetInstance()->GetFlySlash()->Disappear();
	}

	// FLY SLASH DAMAGE NOKKEN
	if ((a->getCollisionBitmask() == MainCharacter::FLY_SLASH_BITMASK && b->getCollisionBitmask() == MainCharacter::NOKKEN_MONSTER_BITMASK)
		|| (a->getCollisionBitmask() == MainCharacter::NOKKEN_MONSTER_BITMASK && b->getCollisionBitmask() == MainCharacter::FLY_SLASH_BITMASK))
	{
		if (MainCharacter::GetInstance()->GetFlySlash()->GetSprite()->isVisible())
		{
			if (a->getCollisionBitmask() == MainCharacter::NOKKEN_MONSTER_BITMASK)
			{
				m_enemies[a->getGroup()]->GetDamage(MainCharacter::GetInstance()->GetAttack() * 2);
			}
			else if (b->getCollisionBitmask() == MainCharacter::NOKKEN_MONSTER_BITMASK)
			{
				m_enemies[b->getGroup()]->GetDamage(MainCharacter::GetInstance()->GetAttack() * 2);
			}
		}
	}

	// FLY SLASH DAMAGE ROPE
	if ((a->getCollisionBitmask() == MainCharacter::FLY_SLASH_BITMASK && b->getCollisionBitmask() == MainCharacter::ROPE_MONSTER_BITMASK)
		|| (a->getCollisionBitmask() == MainCharacter::ROPE_MONSTER_BITMASK && b->getCollisionBitmask() == MainCharacter::FLY_SLASH_BITMASK))
	{
		if (MainCharacter::GetInstance()->GetFlySlash()->GetSprite()->isVisible())
		{
			if (a->getCollisionBitmask() == MainCharacter::ROPE_MONSTER_BITMASK)
			{
				m_enemies[a->getGroup()]->GetDamage(MainCharacter::GetInstance()->GetAttack() * 2);
			}
			else if (b->getCollisionBitmask() == MainCharacter::ROPE_MONSTER_BITMASK)
			{
				m_enemies[b->getGroup()]->GetDamage(MainCharacter::GetInstance()->GetAttack() * 2);
			}
		}
	}

	//COLLECT MEAT ITEM
	if ((a->getCollisionBitmask() == MainCharacter::MAIN_CHARACTER_BITMASK && b->getCollisionBitmask() == MainCharacter::MEAT_ITEM_BITMASK)
		|| (a->getCollisionBitmask() == MainCharacter::MEAT_ITEM_BITMASK && b->getCollisionBitmask() == MainCharacter::MAIN_CHARACTER_BITMASK))
	{
		if (a->getCollisionBitmask() == MainCharacter::MEAT_ITEM_BITMASK)
		{
			m_enemies[a->getGroup()]->GetItem()->GetIcon()->getPhysicsBody()->setContactTestBitmask(false);
			m_enemies[a->getGroup()]->GetItem()->GetIcon()->setVisible(false);
			MainCharacter::GetInstance()->GetInventory()->AddItem(25);
		}
		else if (b->getCollisionBitmask() == MainCharacter::MEAT_ITEM_BITMASK)
		{
			m_enemies[b->getGroup()]->GetItem()->GetIcon()->getPhysicsBody()->setContactTestBitmask(false);
			m_enemies[b->getGroup()]->GetItem()->GetIcon()->setVisible(false);
			MainCharacter::GetInstance()->GetInventory()->AddItem(25);
		}
	}

	//COLLECT MUSHROOM ITEM
	if ((a->getCollisionBitmask() == MainCharacter::MAIN_CHARACTER_BITMASK && b->getCollisionBitmask() == MainCharacter::MUSHROOM_ITEM_BITMASK)
		|| (a->getCollisionBitmask() == MainCharacter::MUSHROOM_ITEM_BITMASK && b->getCollisionBitmask() == MainCharacter::MAIN_CHARACTER_BITMASK))
	{
		if (a->getCollisionBitmask() == MainCharacter::MUSHROOM_ITEM_BITMASK)
		{
			m_enemies[a->getGroup()]->GetItem()->GetIcon()->getPhysicsBody()->setContactTestBitmask(false);
			m_enemies[a->getGroup()]->GetItem()->GetIcon()->setVisible(false);
			MainCharacter::GetInstance()->GetInventory()->AddItem(24);
		}
		else if (b->getCollisionBitmask() == MainCharacter::MUSHROOM_ITEM_BITMASK)
		{
			m_enemies[b->getGroup()]->GetItem()->GetIcon()->getPhysicsBody()->setContactTestBitmask(false);
			m_enemies[b->getGroup()]->GetItem()->GetIcon()->setVisible(false);
			MainCharacter::GetInstance()->GetInventory()->AddItem(24);
		}
	}
	return true;
}

void Level2Scene::NormalAttack(Ref* sender, ui::Widget::TouchEventType type)
{
	if (type == ui::Widget::TouchEventType::BEGAN)
	{
		MainCharacter::GetInstance()->NormalAttack();

	}
}

void Level2Scene::SpecialAttack(Ref* sender, ui::Widget::TouchEventType type)
{
	if (type == ui::Widget::TouchEventType::BEGAN)
	{
		MainCharacter::GetInstance()->SpecialAttack();
	}
}

void Level2Scene::Evade(Ref* sender, ui::Widget::TouchEventType type)
{
	if (type == ui::Widget::TouchEventType::BEGAN)
	{
		MainCharacter::GetInstance()->Evade();
	}
}

void Level2Scene::Defend(Ref* sender, ui::Widget::TouchEventType type)
{
	if (type == ui::Widget::TouchEventType::BEGAN)
	{
		MainCharacter::GetInstance()->Defend();
	}
}

void Level2Scene::Collision(PhysicsContact & contact, int bitmask1, int bitmask2, int type)
{
	PhysicsBody* a = contact.getShapeA()->getBody();
	PhysicsBody* b = contact.getShapeB()->getBody();

	switch (type)
	{
	case 1:
		// MAINCHARACTER COLLISION
		if ((a->getCollisionBitmask() == bitmask1 && b->getCollisionBitmask() == bitmask2)
			|| (a->getCollisionBitmask() == bitmask2 && b->getCollisionBitmask() == bitmask1))
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
		break;
	case 2:
		// MAIN CHARACTER'S ARROW COLLIDE WITH OBSTACLE
		if ((a->getCollisionBitmask() == bitmask1 && b->getCollisionBitmask() == bitmask2)
			|| (a->getCollisionBitmask() == bitmask2 && b->getCollisionBitmask() == bitmask1))
		{
			if (a->getCollisionBitmask() == bitmask1)
			{
				MainCharacter::GetInstance()->GetListArrow()[a->getGroup()]->SetVisible(false);
			}
			else if (b->getCollisionBitmask() == bitmask1)
			{
				MainCharacter::GetInstance()->GetListArrow()[b->getGroup()]->SetVisible(false);
			}
		}
		break;
	case 3:
		// MAIN CHARACTER SLASH COLLIDE WITH MONSTER
		if ((a->getCollisionBitmask() == bitmask1 && b->getCollisionBitmask() == bitmask2)
			|| (a->getCollisionBitmask() == bitmask2 && b->getCollisionBitmask() == bitmask1))
		{
			if (a->getCollisionBitmask() == bitmask2)
			{
				m_enemies[a->getGroup()]->GetDamage(MainCharacter::GetInstance()->GetAttack());
			}
			else if (b->getCollisionBitmask() == bitmask2)
			{
				m_enemies[b->getGroup()]->GetDamage(MainCharacter::GetInstance()->GetAttack());
			}
		}
		break;
	case 4:
		// MAIN CHARACTER'S ARROW COLLIDE MONSTER
		if ((a->getCollisionBitmask() == bitmask1 && b->getCollisionBitmask() == bitmask2)
			|| (a->getCollisionBitmask() == bitmask2 && b->getCollisionBitmask() == bitmask1))
		{
			if (a->getCollisionBitmask() == bitmask2)
			{
				m_enemies[a->getGroup()]->GetDamage(MainCharacter::GetInstance()->GetArrowAttack());
				MainCharacter::GetInstance()->GetListArrow()[b->getGroup()]->SetVisible(false);
			}
			else if (b->getCollisionBitmask() == bitmask2)
			{
				m_enemies[b->getGroup()]->GetDamage(MainCharacter::GetInstance()->GetArrowAttack());
				MainCharacter::GetInstance()->GetListArrow()[a->getGroup()]->SetVisible(false);
			}
		}
		break;
	case 5:
		// MONSTERS COLLIDE OBSTACLES
		if ((a->getCollisionBitmask() == bitmask1 && b->getCollisionBitmask() == bitmask2)
			|| (a->getCollisionBitmask() == bitmask2 && b->getCollisionBitmask() == bitmask1))
		{
			if (a->getCollisionBitmask() == bitmask2)
			{
				m_enemies[a->getGroup()]->SetPreventRun();
				m_enemies[a->getGroup()]->ReverseDirection();
			}
			else if (b->getCollisionBitmask() == bitmask2)
			{
				m_enemies[b->getGroup()]->SetPreventRun();
				m_enemies[b->getGroup()]->ReverseDirection();
			}
		}
		break;
	case 6:
		// BULLET DAMAGE MAIN CHARACTER
		if ((a->getCollisionBitmask() == bitmask1 && b->getCollisionBitmask() == bitmask2)
			|| (a->getCollisionBitmask() == bitmask2 && b->getCollisionBitmask() == bitmask1))
		{
			MainCharacter::GetInstance()->GetDamage(MainCharacter::BOWMOBLIN_DAMAGE);
			if (a->getCollisionBitmask() == bitmask1)
			{
				m_enemies[a->getGroup()]->GetBullet()->SetVisible(false);
			}
			else if (b->getCollisionBitmask() == bitmask1)
			{
				m_enemies[b->getGroup()]->GetBullet()->SetVisible(false);
			}
		}
		break;
	case 7:
		//// BULLET COLLIDE OBSTACLES
		if ((a->getCollisionBitmask() == bitmask1 && b->getCollisionBitmask() == bitmask2)
			|| (a->getCollisionBitmask() == bitmask2 && b->getCollisionBitmask() == bitmask1))
		{
			if (a->getCollisionBitmask() == bitmask1)
			{
				m_enemies[a->getGroup()]->GetBullet()->SetVisible(false);
			}
			else if (b->getCollisionBitmask() == bitmask1)
			{
				m_enemies[b->getGroup()]->GetBullet()->SetVisible(false);
			}
		}
		break;
	default:
		break;
	}
}

void Level2Scene::OpenInventory(cocos2d::Ref * sender)
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

void Level2Scene::CreateTreasure()
{
	auto heartContainerGroup = tileMap->getObjectGroup("heartContainer");
	heartContainer = ResourceManager::GetInstance()->GetSpriteById(29);
	heartContainer->removeFromParent();
	heartContainer->setVisible(true);
	heartContainer->setPosition(Vec2(heartContainerGroup->getObject("heartContainer")["x"].asFloat()
		, heartContainerGroup->getObject("heartContainer")["y"].asFloat()));
	this->addChild(heartContainer);

	auto physics = PhysicsBody::createBox(heartContainer->getContentSize(), PhysicsMaterial(0, 0, 0));
	physics->setRotationEnable(false);
	physics->setCollisionBitmask(MainCharacter::HEART_CONTAINER_BITMASK);
	physics->setDynamic(false);
	physics->setContactTestBitmask(true);
	physics->setGravityEnable(false);
	heartContainer->setPhysicsBody(physics);
}

void Level2Scene::ClickShowInfor(Ref * pSender)
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
