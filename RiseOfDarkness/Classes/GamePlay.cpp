#include "GamePlay.h"
#include "ResourceManager.h"
#include "MainCharacter.h"
#include <math.h>
#include "SimpleAudioEngine.h"

using namespace CocosDenshion;

GamePlay::GamePlay() {}
GamePlay::~GamePlay() 
{
	for (int i = 0; i < m_enemies.size(); i++)
	{
		delete m_enemies[i];
	}
	m_enemies.clear();
}

void GamePlay::UpdateInfoBar()
{
	hpLoadingBar->setPercent(MainCharacter::GetInstance()->GetPercentHP());
	mpLoadingBar->setPercent(MainCharacter::GetInstance()->GetPercentMP());
}

void GamePlay::CreatePhysicsWorld(const char* obstacle, const char* mc, Layer* layer)
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
}

void GamePlay::CreateAllButton(Layer* layer)
{
	anchorCamera = Camera::create();
	anchorCamera->setCameraFlag(CameraFlag::USER1);

	layer->addChild(anchorCamera);

	auto get = ResourceManager::GetInstance();
	auto visibleSize = Director::getInstance()->getVisibleSize();

	// CREATE JOYSTICK
	CreateJoystick(layer);

	// NORMAL ATTACK BUTTON ID 0
	auto normalAttack = get->GetButtonById(12);
	normalAttack->removeFromParent();
	normalAttack->setAnchorPoint(Vec2(1, 0));
	normalAttack->setPosition(Vec2(visibleSize.width - 20, 20));
	layer->addChild(normalAttack, 4);
	normalAttack->setCameraMask(2);
	m_buttons.push_back(normalAttack);

	auto normalAttackSize = normalAttack->getBoundingBox().size;
	auto normalAttackPosition = normalAttack->getPosition();

	// SPECIAL ATTACK BUTTON ID 1
	auto specialAttack = get->GetButtonById(10);
	specialAttack->removeFromParent();
	specialAttack->setAnchorPoint(Vec2(1, 0));
	specialAttack->setPosition(Vec2(normalAttackPosition.x - normalAttackSize.width - 10
		, normalAttackPosition.y + normalAttackSize.height - 10));
	layer->addChild(specialAttack, 4);
	specialAttack->setCameraMask(2);
	m_buttons.push_back(specialAttack);

	// EVDAE BUTTON ID 2
	auto evade = get->GetButtonById(11);
	evade->removeFromParent();
	evade->setAnchorPoint(Vec2(1, 0));
	evade->setPosition(Vec2(normalAttackPosition.x - normalAttackSize.width - 20, normalAttackPosition.y));
	layer->addChild(evade, 4);
	evade->setCameraMask(2);
	m_buttons.push_back(evade);

	// DEFEND BUTTON ID 3
	auto defend = get->GetButtonById(13);
	defend->removeFromParent();
	defend->setAnchorPoint(Vec2(1, 0));
	defend->setPosition(Vec2(normalAttackPosition.x, normalAttackPosition.y + normalAttackSize.height + 20));
	layer->addChild(defend, 4);
	defend->setCameraMask(2);
	m_buttons.push_back(defend);

	// PAUSE BUTTON ID 4
	auto pause = get->GetButtonById(16);
	pause->removeFromParent();
	pause->setAnchorPoint(Vec2(1, 1));
	pause->setVisible(true);
	pause->setPosition(Vec2(visibleSize.width - 20, visibleSize.height - 20));
	layer->addChild(pause, 4);
	pause->setCameraMask(2);
	m_buttons.push_back(pause);

	// RESUME BUTTON ID 5
	auto resume = get->GetButtonById(17);
	resume->setAnchorPoint(Vec2(1, 1));
	resume->removeFromParent();
	resume->setVisible(false);
	resume->setPosition(pause->getPosition());
	layer->addChild(resume, 8);
	resume->setCameraMask(2);
	m_buttons.push_back(resume);

	// HOME BUTTON ID 6
	auto home = get->GetButtonById(24);
	home->setVisible(false);
	home->removeFromParent();
	home->setPosition(Vec2(visibleSize.width / 2 - home->getBoundingBox().size.width, visibleSize.height / 2));
	layer->addChild(home, 8);
	home->setCameraMask(2);
	m_buttons.push_back(home);

	// RETRY BUTTON ID 7
	auto retry = get->GetButtonById(23);
	retry->removeFromParent();
	retry->setVisible(false);
	retry->setPosition(Vec2(visibleSize.width / 2 + home->getBoundingBox().size.width, visibleSize.height / 2));
	layer->addChild(retry, 8);
	retry->setCameraMask(2);
	m_buttons.push_back(retry);

	nextStage = get->GetButtonById(26);
	nextStage->removeFromParent();
	nextStage->setVisible(false);
	nextStage->setPosition(Vec2(visibleSize.width / 2 + home->getBoundingBox().size.width, visibleSize.height / 2));
	layer->addChild(nextStage, 8);
	nextStage->setCameraMask(2);

	// GAMEOVER TEXT
	gameover = get->GetLabelById(1);
	gameover->setString("GAMEOVER!");
	gameover->setVisible(false);
	gameover->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 1.5));
	gameover->removeFromParent();
	gameover->setCameraMask(2);
	layer->addChild(gameover, 8);

	// CLEAR STAGE TEXT
	clear = get->GetLabelById(2);
	clear->setString("STAGE CLEAR!");
	clear->setVisible(false);
	clear->setPosition(gameover->getPosition());
	clear->removeFromParent();
	clear->setCameraMask(2);
	layer->addChild(clear, 8);

	// PAUSE TEXT
	paused = get->GetLabelById(3);
	paused->setString("PAUSE");
	paused->setVisible(false);
	paused->setPosition(gameover->getPosition());
	paused->removeFromParent();
	paused->setCameraMask(2);
	layer->addChild(paused, 8);

	// STATUS MC ID 8
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

	// HP BUTTON ID 9
	auto hpButton = get->GetButtonById(20);
	hpButton->removeFromParent();
	hpButton->setAnchorPoint(Vec2(0.5, 0));
	hpButton->setEnabled(true);
	hpButton->setPosition(Vec2(evade->getPositionX() - 20 - evade->getBoundingBox().size.width * 1.5, 20));
	layer->addChild(hpButton, 4);
	m_buttons.push_back(hpButton);
	hpButton->setCameraMask(2);

	// AMOUNT OF HEALTH POTION
	amountHP = get->GetLabelById(5);
	amountHP->removeFromParent();
	amountHP->setCameraMask(2);
	if (!MainCharacter::GetInstance()->GetInventory()->InventoryContains(21, ItemType::potion))
	{
		amountHP->setString("0");
	}
	else
	{
		amountHP->setString(std::to_string(MainCharacter::GetInstance()->GetInventory()->GetItemAmount(0)
			[MainCharacter::GetInstance()->GetInventory()->GetIdByIcon(21, ItemType::potion)]));
	}
	amountHP->setAnchorPoint(Vec2(1, 0));
	amountHP->setPosition(Vec2(hpButton->getPositionX() + hpButton->getBoundingBox().size.width / 2, hpButton->getPositionY()));
	layer->addChild(amountHP, 5);

	// MP BUTTON ID 10
	auto mpButton = get->GetButtonById(21);
	mpButton->removeFromParent();
	mpButton->setAnchorPoint(Vec2(0.5, 0));
	mpButton->setEnabled(true);
	mpButton->setPosition(Vec2(hpButton->getPositionX() - hpButton->getBoundingBox().size.width - 20, 20));
	layer->addChild(mpButton, 4);
	m_buttons.push_back(mpButton);
	mpButton->setCameraMask(2);

	// AMOUNT OF MANA POTION
	amountMP = get->GetLabelById(6);
	amountMP->removeFromParent();
	amountMP->setCameraMask(2);
	if (!MainCharacter::GetInstance()->GetInventory()->InventoryContains(22, ItemType::potion))
	{
		amountMP->setString("0");
	}
	else
	{
		amountMP->setString(std::to_string(MainCharacter::GetInstance()->GetInventory()->GetItemAmount(0)
			[MainCharacter::GetInstance()->GetInventory()->GetIdByIcon(22, ItemType::potion)]));
	}
	amountMP->setAnchorPoint(Vec2(1, 0));
	amountMP->setPosition(Vec2(mpButton->getPositionX() + mpButton->getBoundingBox().size.width / 2, mpButton->getPositionY()));
	layer->addChild(amountMP, 5);

	//BUTTON OPEN INVENTORY 11
	auto buttonOpenInventory = ui::Button::create("res/sprites/item/inventory.png");
	buttonOpenInventory->retain();
	layer->addChild(buttonOpenInventory, 4);
	buttonOpenInventory->setAnchorPoint(Vec2(1, 1));
	buttonOpenInventory->setEnabled(true);
	buttonOpenInventory->setPosition(Vec2(pause->getPositionX(), defend->getPositionY() + 150));
	m_buttons.push_back(buttonOpenInventory);
	buttonOpenInventory->setCameraMask(2);
	MainCharacter::GetInstance()->GetInventory()->SetSpritePosition(Vec2(visibleSize / 2));

	// SHADER
	shader = get->GetSpriteById(27);
	shader->setPosition(visibleSize / 2);
	shader->removeFromParent();
	shader->setCameraMask(2);
	shader->setOpacity(0);
	layer->addChild(shader, 7);

	// FOCUS SPRITE ID 4
	auto focus = get->GetSpriteById(28);
	focus->removeFromParent();
	layer->addChild(focus, 4);
	m_sprites.push_back(focus);

	// DIRECTION SPRITE ID 5
	auto direction = get->DuplicateSprite(get->GetSpriteById(14));
	direction->setScale(2);
	layer->addChild(direction, 4);
	direction->setPosition(Vec2(pause->getPositionX() - pause->getBoundingBox().size.width / 2, pause->getPositionY() - direction->getBoundingBox().size.width - 10));
	direction->setCameraMask(2);
	direction->setRotation(-90);
	direction->setVisible(false);
	m_sprites.push_back(direction);

	// ENEMY ICON SPRITE ID 6
	auto enemyIcon = get->GetSpriteById(33);
	enemyIcon->removeFromParent();
	enemyIcon->setCameraMask(2);
	enemyIcon->setAnchorPoint(Vec2(0, 1));
	enemyIcon->setPosition(Vec2(0, infoBarPosition.y - infoBarSize.height));
	layer->addChild(enemyIcon, 4);
	m_sprites.push_back(enemyIcon);

	amountEnemy = get->GetLabelById(12);
	amountEnemy->removeFromParent();
	amountEnemy->setCameraMask(2);
	amountEnemy->setAnchorPoint(Vec2(0, 0.5));
	amountEnemy->setPosition(Vec2(enemyIcon->getBoundingBox().size.width, enemyIcon->getPositionY() - enemyIcon->getBoundingBox().size.height / 2 - 5));
	layer->addChild(amountEnemy, 4);

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

void GamePlay::SetCamera(Vec2 pos)
{
	auto visibleSize = Director::getInstance()->getVisibleSize();
	auto tileMapSize = tileMap->getBoundingBox().size;
	float xMin = visibleSize.width / 2, xMax = tileMapSize.width - xMin, yMin = visibleSize.height / 2, yMax = tileMapSize.height - yMin;
	if (pos.x < xMin)
	{
		pos.x = xMin;
	}
	else if (pos.x > xMax)
	{
		pos.x = xMax;
	}
	if (pos.y < yMin)
	{
		pos.y = yMin;
	}
	else if (pos.y > yMax)
	{
		pos.y = yMax;
	}
	camera->setPosition(Vec2(pos.x, pos.y));

	m_sprites[4]->setPosition(Vec2(mainCharacter->getPositionX()
		, mainCharacter->getPositionY() + mainCharacter->getBoundingBox().size.height / 2 + 10));
}

void GamePlay::OpenInventory(cocos2d::Ref * sender)
{
	for (int i = 0; i < 5; i++)
	{
		MainCharacter::GetInstance()->GetInventory()->GetTab(i)->removeAllChildren();
	}
	GamePlay::ShowInventoryGrid();
	MainCharacter::GetInstance()->GetInventory()->SetVisible(
		!(MainCharacter::GetInstance()->GetInventory()->IsVisible())
	);
}

void GamePlay::ShowInventoryGrid()
{
	auto boots = MainCharacter::GetInstance()->GetInventory()->GetBoots();
	auto arrows = MainCharacter::GetInstance()->GetInventory()->GetArrows();
	auto weapons = MainCharacter::GetInstance()->GetInventory()->GetItemsWeapon();
	auto items = MainCharacter::GetInstance()->GetInventory()->GetItems();
	auto armors = MainCharacter::GetInstance()->GetInventory()->GetArmors();
	std::vector<int> itemAmount = MainCharacter::GetInstance()->GetInventory()->GetItemAmount(0);
	std::vector<int> arrowAmount = MainCharacter::GetInstance()->GetInventory()->GetItemAmount(1);
	std::vector<Label*> amountLabel = MainCharacter::GetInstance()->GetInventory()->GetAmountLabel(0);
	std::vector<Label*> amountArrowLabel = MainCharacter::GetInstance()->GetInventory()->GetAmountLabel(1);
	int cols = 0, rows = 0;
	MainCharacter::GetInstance()->GetInventory()->GetClickBox()->removeFromParent();
	// SHOW TAB POTION

	MainCharacter::GetInstance()->GetInventory()->GetTab(1)->addChild(MainCharacter::GetInstance()->GetInventory()->GetClickBox(), 22);
	MainCharacter::GetInstance()->GetInventory()->GetClickBox()->setCameraMask(2);
	for (int i = 0; i < items.size(); i++)
	{
		if (items[i]->GetIcon() != NULL)
		{
			if (cols == 6)
			{
				rows++;
				cols = 0;
			}
			MainCharacter::GetInstance()->GetInventory()->slots[i]->GetIcon()->removeFromParent();
			// get tab to add item
			MainCharacter::GetInstance()->GetInventory()->GetTab(1)->addChild(items[i]->GetIcon(), 21);
			items[i]->GetIcon()->setCameraMask(2);
			MainCharacter::GetInstance()->GetInventory()->slots[i]->GetIcon()->setPosition(
				Vec2(64 * cols + 32,
					MainCharacter::GetInstance()->GetInventory()->GetSize().y - 64 * rows - 32) - Vec2(0, 69)
			);

			amountLabel[i]->removeFromParent();
			if (itemAmount[i]>1)
			{
				amountLabel[i]->setString(std::to_string(itemAmount[i]));
			}
			amountLabel[i]->setCameraMask(2);
			amountLabel[i]->setPosition(items[i]->GetIcon()->getPosition() + Vec2(16, -16));

			MainCharacter::GetInstance()->GetInventory()->GetTab(1)->addChild(amountLabel[i], 22);
			cols++;
		}
	}
	//SHOW TAB WEAPON
	cols = rows = 0;
	for (int i = 0; i < weapons.size(); i++)
	{
		if (weapons[i]->GetIcon() != NULL)
		{
			if (cols == 6)
			{
				rows++;
				cols = 0;
			}
			MainCharacter::GetInstance()->GetInventory()->weapons[i]->GetIcon()->removeFromParent();
			// get tab to add item
			MainCharacter::GetInstance()->GetInventory()->GetTab(0)->addChild(weapons[i]->GetIcon(), 21);
			weapons[i]->GetIcon()->setCameraMask(2);
			MainCharacter::GetInstance()->GetInventory()->weapons[i]->GetIcon()->setPosition(
				Vec2(64 * cols + 32,
					MainCharacter::GetInstance()->GetInventory()->GetSize().y - 64 * rows - 32) - Vec2(0, 69)
			);
			cols++;
		}
	}
	//SHOW TAB ARROW
	cols = rows = 0;
	MainCharacter::GetInstance()->GetInventory()->GetClickBox()->removeFromParent();
	MainCharacter::GetInstance()->GetInventory()->GetTab(3)->addChild(MainCharacter::GetInstance()->GetInventory()->GetClickBox(), 22);
	for (int i = 0; i < arrows.size(); i++)
	{
		if (arrows[i]->GetIcon() != NULL)
		{
			if (cols == 6)
			{
				rows++;
				cols = 0;
			}
			MainCharacter::GetInstance()->GetInventory()->arrows[i]->GetIcon()->removeFromParent();
			// get tab to add item
			MainCharacter::GetInstance()->GetInventory()->GetTab(3)->addChild(arrows[i]->GetIcon(), 21);
			arrows[i]->GetIcon()->setCameraMask(2);
			MainCharacter::GetInstance()->GetInventory()->arrows[i]->GetIcon()->setPosition(
				Vec2(64 * cols + 32,
					MainCharacter::GetInstance()->GetInventory()->GetSize().y - 64 * rows - 32) - Vec2(0, 69)
			);
			amountArrowLabel[i]->removeFromParent();
			if (arrowAmount[i]>1)
			{
				amountArrowLabel[i]->setString(std::to_string(arrowAmount[i]));
			}
			amountArrowLabel[i]->setCameraMask(2);
			amountArrowLabel[i]->setPosition(arrows[i]->GetIcon()->getPosition() + Vec2(16, -16));
			MainCharacter::GetInstance()->GetInventory()->GetTab(3)->addChild(amountArrowLabel[i], 22);
			cols++;
		}
	}
	//SHOW TAB ARMOR
	cols = rows = 0;
	for (int i = 0; i < armors.size(); i++)
	{
		if (armors[i]->GetIcon() != NULL)
		{
			if (cols == 6)
			{
				rows++;
				cols = 0;
			}
			MainCharacter::GetInstance()->GetInventory()->armors[i]->GetIcon()->removeFromParent();
			// get tab to add item
			MainCharacter::GetInstance()->GetInventory()->GetTab(2)->addChild(armors[i]->GetIcon(), 21);
			armors[i]->GetIcon()->setCameraMask(2);
			MainCharacter::GetInstance()->GetInventory()->armors[i]->GetIcon()->setPosition(
				Vec2(64 * cols + 32,
					MainCharacter::GetInstance()->GetInventory()->GetSize().y - 64 * rows - 32) - Vec2(0, 69)
			);
			cols++;
		}
	}
	//SHOW TAB BOOT
	cols = rows = 0;
	for (int i = 0; i < boots.size(); i++)
	{
		if (boots[i]->GetIcon() != NULL)
		{
			if (cols == 6)
			{
				rows++;
				cols = 0;
			}
			MainCharacter::GetInstance()->GetInventory()->boots[i]->GetIcon()->removeFromParent();
			// get tab to add item
			MainCharacter::GetInstance()->GetInventory()->GetTab(4)->addChild(boots[i]->GetIcon(), 21);
			boots[i]->GetIcon()->setCameraMask(2);
			MainCharacter::GetInstance()->GetInventory()->boots[i]->GetIcon()->setPosition(
				Vec2(64 * cols + 32,
					MainCharacter::GetInstance()->GetInventory()->GetSize().y - 64 * rows - 32) - Vec2(0, 69)
			);
			cols++;
		}
	}
}

bool GamePlay::CheckClear()
{
	for (int i = 0; i < m_enemies.size(); i++)
	{
		if (m_enemies[i]->GetSprite()->isVisible())
		{
			return false;
		}
	}
	return true;
}

void GamePlay::EnemyUpdate(float deltaTime)
{
	for (int i = 0; i < m_enemies.size(); i++)
	{
		if (m_enemies[i]->GetSprite()->isVisible())
		{
			m_enemies[i]->Update(deltaTime);
		}
	}
}

void GamePlay::CreateJoystick(Layer* layer)
{
	// JOYSTICK
	auto thumb = ResourceManager::GetInstance()->GetSpriteById(25);
	thumb->removeFromParent();
	auto joystick = ResourceManager::GetInstance()->GetSpriteById(26);
	joystick->removeFromParent();

	Rect joystickBaseDimensions;
	joystickBaseDimensions = Rect(0, 0, 80.f, 80.0f);

	Point joystickBasePosition;
	joystickBasePosition = Vec2(thumb->getBoundingBox().size.width / 2 + joystick->getBoundingBox().size.width / 2
		, thumb->getBoundingBox().size.height / 2 + joystick->getBoundingBox().size.height / 2);

	joystickBase = new SneakyJoystickSkinnedBase();
	joystickBase->init();
	joystickBase->setPosition(joystickBasePosition);
	joystickBase->setBackgroundSprite(thumb);
	joystickBase->setThumbSprite(joystick);

	SneakyJoystick *aJoystick = new SneakyJoystick();
	aJoystick->initWithRect(joystickBaseDimensions);
	aJoystick->autorelease();
	joystickBase->setJoystick(aJoystick);
	joystickBase->setPosition(joystickBasePosition);

	leftJoystick = joystickBase->getJoystick();
	layer->addChild(joystickBase, 4);
	joystickBase->setCameraMask(2);

	activeRunRange = thumb->getBoundingBox().size.height / 2;
}

void GamePlay::UpdateJoystick()
{
	Point pos = leftJoystick->getStickPosition();
	float radius = std::sqrt(pos.x*pos.x + pos.y*pos.y);
	if (radius > 0)
	{
		float degree = std::atan2f(pos.y, pos.x) * 180 / 3.141593;

		// GO UP
		if (degree < 120 && degree > 60)
		{
			m_sprites[5]->setRotation(90);
			MainCharacter::GetInstance()->SetDirection(1);
		}
		// GO DOWN
		else if (degree < -60 && degree > -120)
		{
			m_sprites[5]->setRotation(-90);
			MainCharacter::GetInstance()->SetDirection(2);
		}
		// GO LEFT
		else if (degree > 150 || degree < -150)
		{
			m_sprites[5]->setRotation(0);
			MainCharacter::GetInstance()->SetDirection(3);
		}
		// GO RIGHT
		else if (degree < 30 && degree > -30)
		{
			m_sprites[5]->setRotation(180);
			MainCharacter::GetInstance()->SetDirection(4);
		}
		// GO LEFT UP
		else if (degree <= 150 && degree >= 120)
		{
			m_sprites[5]->setRotation(45);
			MainCharacter::GetInstance()->SetDirection(5);
		}
		// GO LEFT DOWN
		else if(degree <= -120 && degree >= -150)
		{
			m_sprites[5]->setRotation(-45);
			MainCharacter::GetInstance()->SetDirection(6);
		}
		// GO RIGHT UP
		else if (degree <= 60 && degree >= 30)
		{
			m_sprites[5]->setRotation(135);
			MainCharacter::GetInstance()->SetDirection(7);
		}
		// GO RIGHT DOWN
		else
		{
			m_sprites[5]->setRotation(-135);
			MainCharacter::GetInstance()->SetDirection(8);
		}
		MainCharacter::GetInstance()->Run(radius / activeRunRange);
	}
	else
	{
		MainCharacter::GetInstance()->StopRun();
	}
}

void GamePlay::InitChest(Layer* layer)
{
	//ShowInfor
	tab = ui::Layout::create();
	tab->setSize(Size(228, 318));
	tab->removeFromParent();
	layer->addChild(tab, 22);
	tab->setVisible(false);
	tab->removeAllChildren();
	tab->retain();
	tab->setAnchorPoint(Point(0, 0));
	tab->setCameraMask(2);
	tab->setPosition(Vec2(Director::getInstance()->getVisibleSize().width / 2 - tab->getBoundingBox().size.width / 2
		, Director::getInstance()->getVisibleSize().height / 2 - tab->getBoundingBox().size.height / 2));
	auto bg = Sprite::create("res/sprites/item/chest.png");
	bg->setPosition(0, 0);
	bg->retain();
	bg->setAnchorPoint(Point(0, 0));
	bg->removeFromParent();
	tab->addChild(bg, 23);

	bg->setCameraMask(2);
}

void GamePlay::ShowInfor()
{
	int rows = 0;
	auto items = MainCharacter::GetInstance()->GetEquipedItem();
	for (int i = 0; i < items.size(); i++)
	{
		items[i]->GetIcon()->removeFromParent();
		tab->addChild(items[i]->GetIcon(), 24);
		items[i]->GetIcon()->setPosition(Vec2(32, tab->getSize().height - 64 * rows - 32));
		rows++;
		items[i]->GetIcon()->setCameraMask(2);
	}
}

int GamePlay::GetAliveEnemies()
{
	int amount = 0;
	for (int i = 0; i < m_enemies.size(); i++)
	{
		if (m_enemies[i]->GetSprite()->isVisible())
		{
			amount++;
		}
	}
	return amount;
}