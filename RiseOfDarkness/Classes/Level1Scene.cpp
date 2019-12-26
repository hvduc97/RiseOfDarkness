#include "Level1Scene.h"
#include "MainCharacter.h"
#include "ResourceManager.h"
#include "SpearMoblin.h"
#include "BowMoblin.h"
#include "MapScene.h"
#include "HomeScene.h"
#include "SimpleAudioEngine.h"
#include "Level3Scene.h"

using namespace CocosDenshion;

USING_NS_CC;

Scene* Level1Scene::CreateScene()
{
	auto scene = Scene::createWithPhysics();
	//scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
	scene->getPhysicsWorld()->setGravity(Vec2(0, 0));
	auto layer = Level1Scene::create();

	scene->addChild(layer, -1);

	return scene;
}

bool Level1Scene::init()
{
	if (!Layer::init())
	{
		return false;
	}

	SimpleAudioEngine::getInstance()->stopBackgroundMusic(true);

	MainCharacter::GetInstance()->Refresh();

	currentStage = 2;

	tileMap = ResourceManager::GetInstance()->GetTileMapById(3);
	upperTileMap = ResourceManager::GetInstance()->GetTileMapById(4);

	CreatePhysicsWorld("obstacles", "mc", this);

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

void Level1Scene::update(float deltaTime)
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

void Level1Scene::CreateMonster()
{
	float x1, y1;
	int direction1;
	auto spearGoblinGroup = tileMap->getObjectGroup("spearMoblin");
	int amount1 = 4;
	char str1[10];
	for (int i = 1; i <= amount1; i++)
	{
		sprintf(str1, "%02d", i);
		direction1 = spearGoblinGroup->getObject(str1)["direction"].asInt();
		x1 = spearGoblinGroup->getObject(str1)["x"].asFloat();
		y1 = spearGoblinGroup->getObject(str1)["y"].asFloat();
		SpearMoblin *spearMoblin = new SpearMoblin(this, direction1, Vec2(x1, y1), i - 1);
		m_enemies.push_back(spearMoblin);
	}

	float x2, y2;
	int direction2;
	auto bowGoblinGroup = tileMap->getObjectGroup("bowMoblin");
	int amount2 = 3;
	char str2[10];
	for (int i = 1; i <= amount2; i++)
	{
		sprintf(str2, "%02d", i);
		direction2 = bowGoblinGroup->getObject(str2)["direction"].asInt();
		x2 = bowGoblinGroup->getObject(str2)["x"].asFloat();
		y2 = bowGoblinGroup->getObject(str2)["y"].asFloat();
		BowMoblin *bowMoblin = new BowMoblin(this, direction2, Vec2(x2, y2), i - 1 + amount1);
		m_enemies.push_back(bowMoblin);
	}
}

void Level1Scene::AddListener()
{
	auto touchListener = EventListenerTouchOneByOne::create();
	touchListener->onTouchBegan = CC_CALLBACK_2(Level1Scene::OnTouchBegan, this);
	touchListener->onTouchEnded = CC_CALLBACK_2(Level1Scene::OnTouchEnded, this);
	touchListener->onTouchMoved = CC_CALLBACK_2(Level1Scene::OnTouchMoved, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);

	// SKILLS
	m_buttons[1]->addTouchEventListener(CC_CALLBACK_2(Level1Scene::SpecialAttack, this));
	m_buttons[2]->addTouchEventListener(CC_CALLBACK_2(Level1Scene::Evade, this));
	m_buttons[0]->addTouchEventListener(CC_CALLBACK_2(Level1Scene::NormalAttack, this));
	m_buttons[3]->addTouchEventListener(CC_CALLBACK_2(Level1Scene::Defend, this));

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
			Director::getInstance()->replaceScene(Level1Scene::CreateScene());
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
	m_buttons[11]->addClickEventListener(CC_CALLBACK_1(Level1Scene::OpenInventory, this));

	auto contactListener = EventListenerPhysicsContact::create();
	contactListener->onContactBegin = CC_CALLBACK_1(Level1Scene::onContactBegin, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);

	// STATUS
	m_buttons[8]->addClickEventListener(CC_CALLBACK_1(Level1Scene::ClickShowInfor, this));

	nextStage->addClickEventListener([&](Ref* event) {
		CreateMonster();
		auto gotoMap = CallFunc::create([] {
			Director::getInstance()->replaceScene(Level3Scene::CreateScene());
		});
		runAction(gotoMap);
	});
}

bool Level1Scene::OnTouchBegan(Touch* touch, Event* event)
{	
	return true;
}

bool Level1Scene::OnTouchEnded(Touch* touch, Event* event)
{
	return true;
}

void Level1Scene::OnTouchMoved(Touch* touch, Event* event)
{
	
}

bool Level1Scene::onContactBegin(PhysicsContact& contact)
{
	PhysicsBody* a = contact.getShapeA()->getBody();
	PhysicsBody* b = contact.getShapeB()->getBody();

	// MAIN CHARACTER WITH OBSTACLES
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

	// ARROW COLLIDE WITH OBSTACLE
	if ((a->getCollisionBitmask() == MainCharacter::NORMAL_ARROW_BITMASK && b->getCollisionBitmask() == MainCharacter::OBSTACLE_BITMASK)
		|| (a->getCollisionBitmask() == MainCharacter::OBSTACLE_BITMASK && b->getCollisionBitmask() == MainCharacter::NORMAL_ARROW_BITMASK))
	{
		if (a->getCollisionBitmask() == MainCharacter::NORMAL_ARROW_BITMASK)
		{
			MainCharacter::GetInstance()->GetListArrow()[a->getGroup()]->SetVisible(false);
		}
		else if (b->getCollisionBitmask() == MainCharacter::NORMAL_ARROW_BITMASK)
		{
			MainCharacter::GetInstance()->GetListArrow()[b->getGroup()]->SetVisible(false);
		}
	}
	//=======================
	// MAIN CHARACTER SLASH BOWMOBLIN
	if ((a->getCollisionBitmask() == MainCharacter::SLASH_BITMASK && b->getCollisionBitmask() == MainCharacter::BOWMOBLIN_BITMASK)
		|| (a->getCollisionBitmask() == MainCharacter::BOWMOBLIN_BITMASK && b->getCollisionBitmask() == MainCharacter::SLASH_BITMASK))
	{
		if (a->getCollisionBitmask() == MainCharacter::BOWMOBLIN_BITMASK)
		{
			m_enemies[a->getGroup()]->GetDamage(MainCharacter::GetInstance()->GetAttack());
		}
		else if (b->getCollisionBitmask() == MainCharacter::BOWMOBLIN_BITMASK)
		{
			m_enemies[b->getGroup()]->GetDamage(MainCharacter::GetInstance()->GetAttack());
		}
	}

	//ARROW DAMAGE BOWMOBLIN

	if ((a->getCollisionBitmask() == MainCharacter::NORMAL_ARROW_BITMASK && b->getCollisionBitmask() == MainCharacter::BOWMOBLIN_BITMASK)
		|| (a->getCollisionBitmask() == MainCharacter::BOWMOBLIN_BITMASK && b->getCollisionBitmask() == MainCharacter::NORMAL_ARROW_BITMASK))
	{
		if (a->getCollisionBitmask() == MainCharacter::BOWMOBLIN_BITMASK)
		{
			m_enemies[a->getGroup()]->GetDamage(MainCharacter::GetInstance()->GetArrowAttack());
			MainCharacter::GetInstance()->GetListArrow()[b->getGroup()]->SetVisible(false);
		}
		else if (b->getCollisionBitmask() == MainCharacter::BOWMOBLIN_BITMASK)
		{
			m_enemies[b->getGroup()]->GetDamage(MainCharacter::GetInstance()->GetArrowAttack());
			MainCharacter::GetInstance()->GetListArrow()[a->getGroup()]->SetVisible(false);
		}
	}
	// MAIN CHARACTER SLASH SPEARMOBLIN
	if ((a->getCollisionBitmask() == MainCharacter::SLASH_BITMASK && b->getCollisionBitmask() == MainCharacter::SPEARMOBLIN_BITMASK)
		|| (a->getCollisionBitmask() == MainCharacter::SPEARMOBLIN_BITMASK && b->getCollisionBitmask() == MainCharacter::SLASH_BITMASK))
	{
		if (a->getCollisionBitmask() == MainCharacter::SPEARMOBLIN_BITMASK)
		{
			m_enemies[a->getGroup()]->GetDamage(MainCharacter::GetInstance()->GetAttack());
		}
		else if (b->getCollisionBitmask() == MainCharacter::SPEARMOBLIN_BITMASK)
		{
			m_enemies[b->getGroup()]->GetDamage(MainCharacter::GetInstance()->GetAttack());
		}
	}

	// MAIN CHARACTER'S ARROW COLLIDE SPEARMOBLIN
	if ((a->getCollisionBitmask() == MainCharacter::NORMAL_ARROW_BITMASK && b->getCollisionBitmask() == MainCharacter::SPEARMOBLIN_BITMASK)
		|| (a->getCollisionBitmask() == MainCharacter::SPEARMOBLIN_BITMASK && b->getCollisionBitmask() == MainCharacter::NORMAL_ARROW_BITMASK))
	{
		if (a->getCollisionBitmask() == MainCharacter::SPEARMOBLIN_BITMASK)
		{
			m_enemies[a->getGroup()]->GetDamage(MainCharacter::GetInstance()->GetArrowAttack());
			MainCharacter::GetInstance()->GetListArrow()[b->getGroup()]->SetVisible(false);
		}
		else if (b->getCollisionBitmask() == MainCharacter::SPEARMOBLIN_BITMASK)
		{
			m_enemies[b->getGroup()]->GetDamage(MainCharacter::GetInstance()->GetArrowAttack());
			MainCharacter::GetInstance()->GetListArrow()[a->getGroup()]->SetVisible(false);
		}
	}

	// SPEARMOBLIN PIERCE MAIN CHARACTER
	if ((a->getCollisionBitmask() == MainCharacter::PIERCE_BITMASK && b->getCollisionBitmask() == MainCharacter::MAIN_CHARACTER_BITMASK)
		|| (a->getCollisionBitmask() == MainCharacter::MAIN_CHARACTER_BITMASK && b->getCollisionBitmask() == MainCharacter::PIERCE_BITMASK))
	{
		MainCharacter::GetInstance()->GetDamage(MainCharacter::SPEARMOBLIN_DAMAGE);
	}

	// BOWMOBLIN ARROW DAMAGE MAIN CHARACTER
	if ((a->getCollisionBitmask() == MainCharacter::BOWMOBLIN_ARROW_BITMASK && b->getCollisionBitmask() == MainCharacter::MAIN_CHARACTER_BITMASK)
		|| (a->getCollisionBitmask() == MainCharacter::MAIN_CHARACTER_BITMASK && b->getCollisionBitmask() == MainCharacter::BOWMOBLIN_ARROW_BITMASK))
	{
		MainCharacter::GetInstance()->GetDamage(MainCharacter::BOWMOBLIN_DAMAGE);
		if (a->getCollisionBitmask() == MainCharacter::BOWMOBLIN_ARROW_BITMASK)
		{
			m_enemies[a->getGroup()]->GetArrow()->SetVisible(false);
		}
		else if (b->getCollisionBitmask() == MainCharacter::BOWMOBLIN_ARROW_BITMASK)
		{
			m_enemies[b->getGroup()]->GetArrow()->SetVisible(false);
		}
	}

	// SPEARMOBLIN COLLIDE OBSTACLES
	if ((a->getCollisionBitmask() == MainCharacter::OBSTACLE_BITMASK && b->getCollisionBitmask() == MainCharacter::SPEARMOBLIN_BITMASK)
		|| (a->getCollisionBitmask() == MainCharacter::SPEARMOBLIN_BITMASK && b->getCollisionBitmask() == MainCharacter::OBSTACLE_BITMASK))
	{
		if (a->getCollisionBitmask() == MainCharacter::SPEARMOBLIN_BITMASK)
		{
			m_enemies[a->getGroup()]->SetPreventRun();
			m_enemies[a->getGroup()]->ReverseDirection();
		}
		else if (b->getCollisionBitmask() == MainCharacter::SPEARMOBLIN_BITMASK)
		{
			m_enemies[b->getGroup()]->SetPreventRun();
			m_enemies[b->getGroup()]->ReverseDirection();
		}
	}

	// BOWMOBLIN COLLIDE OBSTACLES
	if ((a->getCollisionBitmask() == MainCharacter::OBSTACLE_BITMASK && b->getCollisionBitmask() == MainCharacter::BOWMOBLIN_BITMASK)
		|| (a->getCollisionBitmask() == MainCharacter::BOWMOBLIN_BITMASK && b->getCollisionBitmask() == MainCharacter::OBSTACLE_BITMASK))
	{
		if (a->getCollisionBitmask() == MainCharacter::BOWMOBLIN_BITMASK)
		{
			m_enemies[a->getGroup()]->SetPreventRun();
			m_enemies[a->getGroup()]->ReverseDirection();
		}
		else if (b->getCollisionBitmask() == MainCharacter::BOWMOBLIN_BITMASK)
		{
			m_enemies[b->getGroup()]->SetPreventRun();
			m_enemies[b->getGroup()]->ReverseDirection();
		}
	}

	// BOWMOBLIN ARROW COLLIDE OBSTACLES
	if ((a->getCollisionBitmask() == MainCharacter::BOWMOBLIN_ARROW_BITMASK && b->getCollisionBitmask() == MainCharacter::OBSTACLE_BITMASK)
		|| (a->getCollisionBitmask() == MainCharacter::OBSTACLE_BITMASK && b->getCollisionBitmask() == MainCharacter::BOWMOBLIN_ARROW_BITMASK))
	{
		if (a->getCollisionBitmask() == MainCharacter::BOWMOBLIN_ARROW_BITMASK)
		{
			m_enemies[a->getGroup()]->GetArrow()->SetVisible(false);
		}
		else if (b->getCollisionBitmask() == MainCharacter::BOWMOBLIN_ARROW_BITMASK)
		{
			m_enemies[b->getGroup()]->GetArrow()->SetVisible(false);
		}
	}

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

	// FLY SLASH DAMAGE SPEARMOBLIN
	if ((a->getCollisionBitmask() == MainCharacter::FLY_SLASH_BITMASK && b->getCollisionBitmask() == MainCharacter::SPEARMOBLIN_BITMASK)
		|| (a->getCollisionBitmask() == MainCharacter::SPEARMOBLIN_BITMASK && b->getCollisionBitmask() == MainCharacter::FLY_SLASH_BITMASK))
	{
		if (MainCharacter::GetInstance()->GetFlySlash()->GetSprite()->isVisible())
		{
			if (a->getCollisionBitmask() == MainCharacter::SPEARMOBLIN_BITMASK)
			{
				m_enemies[a->getGroup()]->GetDamage(MainCharacter::GetInstance()->GetAttack() * 2);
			}
			else if (b->getCollisionBitmask() == MainCharacter::SPEARMOBLIN_BITMASK)
			{
				m_enemies[b->getGroup()]->GetDamage(MainCharacter::GetInstance()->GetAttack() * 2);
			}
		}
	}

	// FLY SLASH DAMAGE BOWMOBLIN
	if ((a->getCollisionBitmask() == MainCharacter::FLY_SLASH_BITMASK && b->getCollisionBitmask() == MainCharacter::BOWMOBLIN_BITMASK)
		|| (a->getCollisionBitmask() == MainCharacter::BOWMOBLIN_BITMASK && b->getCollisionBitmask() == MainCharacter::FLY_SLASH_BITMASK))
	{
		if (MainCharacter::GetInstance()->GetFlySlash()->GetSprite()->isVisible())
		{
			if (a->getCollisionBitmask() == MainCharacter::BOWMOBLIN_BITMASK)
			{
				m_enemies[a->getGroup()]->GetDamage(MainCharacter::GetInstance()->GetAttack() * 2);
			}
			else if (b->getCollisionBitmask() == MainCharacter::BOWMOBLIN_BITMASK)
			{
				m_enemies[b->getGroup()]->GetDamage(MainCharacter::GetInstance()->GetAttack() * 2);
			}
		}
	}


	
	//COLLECT ARROW ITEM
	if ((a->getCollisionBitmask() == MainCharacter::MAIN_CHARACTER_BITMASK && b->getCollisionBitmask() == MainCharacter::ARROW1_ITEM_BITMASK)
		|| (a->getCollisionBitmask() == MainCharacter::ARROW1_ITEM_BITMASK && b->getCollisionBitmask() == MainCharacter::MAIN_CHARACTER_BITMASK))
	{
		if (a->getCollisionBitmask() == MainCharacter::ARROW1_ITEM_BITMASK)
		{
			m_enemies[a->getGroup()]->GetItem()->GetIcon()->getPhysicsBody()->setContactTestBitmask(false);
			m_enemies[a->getGroup()]->GetItem()->GetIcon()->setVisible(false);
			MainCharacter::GetInstance()->GetInventory()->AddItem(6);
		}
		else if (b->getCollisionBitmask() == MainCharacter::ARROW1_ITEM_BITMASK)
		{
			m_enemies[b->getGroup()]->GetItem()->GetIcon()->getPhysicsBody()->setContactTestBitmask(false);
			m_enemies[b->getGroup()]->GetItem()->GetIcon()->setVisible(false);
			MainCharacter::GetInstance()->GetInventory()->AddItem(6);
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
	return true;
}

void Level1Scene::NormalAttack(Ref* sender, ui::Widget::TouchEventType type)
{
	if (type == ui::Widget::TouchEventType::BEGAN)
	{
		MainCharacter::GetInstance()->NormalAttack();

	}
}

void Level1Scene::SpecialAttack(Ref* sender, ui::Widget::TouchEventType type)
{
	if (type == ui::Widget::TouchEventType::BEGAN)
	{
		MainCharacter::GetInstance()->SpecialAttack();
	}
}

void Level1Scene::Evade(Ref* sender, ui::Widget::TouchEventType type)
{
	if (type == ui::Widget::TouchEventType::BEGAN)
	{
		MainCharacter::GetInstance()->Evade();
	}
}

void Level1Scene::Defend(Ref* sender, ui::Widget::TouchEventType type)
{
	if (type == ui::Widget::TouchEventType::BEGAN)
	{
		MainCharacter::GetInstance()->Defend();
	}
}

void Level1Scene::OpenInventory(cocos2d::Ref * sender)
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

void Level1Scene::CreateTreasure()
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

void Level1Scene::ClickShowInfor(Ref * pSender)
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
