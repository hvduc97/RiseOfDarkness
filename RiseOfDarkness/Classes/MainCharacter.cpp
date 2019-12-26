#include "MainCharacter.h"
#include "ResourceManager.h"
#include <math.h>
#include "SimpleAudioEngine.h"

using namespace CocosDenshion;

MainCharacter* MainCharacter::m_instance = nullptr;

MainCharacter::MainCharacter() {}

MainCharacter* MainCharacter::GetInstance()
{
	if (m_instance == nullptr)
	{
		m_instance = new MainCharacter();
	}
	return m_instance;
}

MainCharacter::~MainCharacter() {}

void MainCharacter::AddToLayer(Layer* layer)
{
	layer->addChild(mSprite, 1);
	slash->AddToLayer(layer);
	flySlash->AddToLayer(layer);
	for (int i = 0; i < mArrows.size(); i++)
	{
		mArrows[i]->AddToScene(layer);
	}

	inventory->AddToLayer(layer);
	inventory->SetVisible(false);
}

void MainCharacter::Init(std::string name)
{
	mName = name;
	
	CreateMainCharacter();
}

void MainCharacter::CreateMainCharacter()
{
	auto get = ResourceManager::GetInstance();
	//CREATE ALL ITEM ID
	auto grid = Sprite::create("res/sprites/item/gridInventory.png");
	grid->retain();
	inventory = new Inventory(grid);
	for (int i = 0; i < 50; i++)
	{
		inventory->AddItem(21);
		inventory->AddItem(22);
		inventory->AddItem(23);
		inventory->AddItem(24);
		inventory->AddItem(25);
	}

	// sword
	inventory->AddItem(27);
	inventory->AddItem(28);
	inventory->AddItem(29);
	inventory->AddItem(30);
	inventory->AddItem(31);

	// bow
	inventory->AddItem(18);
	inventory->AddItem(19);
	inventory->AddItem(20);

	// armor
	inventory->AddItem(2);
	inventory->AddItem(3);
	inventory->AddItem(4);
	inventory->AddItem(5);

	// boot
	inventory->AddItem(14);
	inventory->AddItem(15);
	inventory->AddItem(16);

	// arrows
	for (int i = 0; i < 20; i++)
	{
		inventory->AddItem(6);
	}

	for (int i = 0; i < 20; i++)
	{
		inventory->AddItem(7);
	}

	for (int i = 0; i < 20; i++)
	{
		inventory->AddItem(8);
	}

	for (int i = 0; i < 20; i++)
	{
		inventory->AddItem(9);
	}

	for (int i = 0; i < 20; i++)
	{
		inventory->AddItem(10);
	}

	for (int i = 0; i < 20; i++)
	{
		inventory->AddItem(11);
	}

	for (int i = 0; i < 20; i++)
	{
		inventory->AddItem(12);
	}


	//Initial Items
	mItems.push_back(inventory->database->items[inventory->database->GetIndexByID(26)]); //sword
	mItems.push_back(inventory->database->items[inventory->database->GetIndexByID(17)]); //bow
	mItems.push_back(inventory->database->items[inventory->database->GetIndexByID(1)]); //armor
	mItems.push_back(inventory->database->items[inventory->database->GetIndexByID(13)]); //boot
	mItems.push_back(inventory->database->items[inventory->database->GetIndexByID(6)]); //arrow

	EquipedItem(0, inventory->database->items[inventory->database->GetIndexByID(26)]);
	EquipedItem(1, inventory->database->items[inventory->database->GetIndexByID(17)]);
	EquipedItem(2, inventory->database->items[inventory->database->GetIndexByID(1)]);
	EquipedItem(3, inventory->database->items[inventory->database->GetIndexByID(13)]);
	EquipedItem(4, inventory->database->items[inventory->database->GetIndexByID(6)]);

	// CREATE SPRITE
	mSprite = get->GetSpriteById(0);
	mSprite->setScale(1.8);

	// CREATE ACTION
	mAction[FRONT_IDLE] = ResourceManager::GetInstance()->GetActionById(3);
	mAction[BACK_IDLE] = ResourceManager::GetInstance()->GetActionById(4);
	mAction[LEFT_IDLE] = ResourceManager::GetInstance()->GetActionById(5);
	mAction[GO_DOWN] = ResourceManager::GetInstance()->GetActionById(1);
	mAction[GO_UP] = ResourceManager::GetInstance()->GetActionById(0);
	mAction[GO_LEFT] = ResourceManager::GetInstance()->GetActionById(2);
	mAction[ROLL_FRONT] = ResourceManager::GetInstance()->GetActionById(15);
	mAction[ROLL_BACK] = ResourceManager::GetInstance()->GetActionById(16);
	mAction[ROLL_LEFT] = ResourceManager::GetInstance()->GetActionById(17);
	mAction[FRONT_SLASH] = ResourceManager::GetInstance()->GetActionById(6);
	mAction[BACK_SLASH] = ResourceManager::GetInstance()->GetActionById(7);
	mAction[LEFT_SLASH] = ResourceManager::GetInstance()->GetActionById(8);
	mAction[FRONT_ARCHERY] = ResourceManager::GetInstance()->GetActionById(9);
	mAction[BACK_ARCHERY] = ResourceManager::GetInstance()->GetActionById(10);
	mAction[LEFT_ARCHERY] = ResourceManager::GetInstance()->GetActionById(11);
	mAction[FRONT_SHIELD] = ResourceManager::GetInstance()->GetActionById(12);
	mAction[BACK_SHIELD] = ResourceManager::GetInstance()->GetActionById(13);
	mAction[LEFT_SHIELD] = ResourceManager::GetInstance()->GetActionById(14);
	mAction[FRONT_GET_DAMAGE] = ResourceManager::GetInstance()->GetActionById(19);
	mAction[BACK_GET_DAMAGE] = ResourceManager::GetInstance()->GetActionById(18);
	mAction[LEFT_GET_DAMAGE] = ResourceManager::GetInstance()->GetActionById(20);
	mAction[DEAD] = ResourceManager::GetInstance()->GetActionById(21);

	// CREATE PHYSICS BODY 
	Size box;
	box.width = mSprite->getContentSize().width / 1.5;
	box.height = mSprite->getContentSize().height / 2;
	mPhysicsBody = PhysicsBody::createBox(box, PhysicsMaterial(0, 0, 0), Vec2(0, -box.height / 2));
	mPhysicsBody->setGravityEnable(false);
	mPhysicsBody->setRotationEnable(false);
	mPhysicsBody->setDynamic(false);
	mPhysicsBody->setCollisionBitmask(MAIN_CHARACTER_BITMASK);
	mPhysicsBody->setContactTestBitmask(true);
	mSprite->setPhysicsBody(mPhysicsBody);

	// CREATE ALL STATUS
	stageLevel = 4;
	direction = 2;
	currentState = FRONT_IDLE;
	attack = ATTACK + 20;
	arrowAttack = 30;
	defend = DEFEND + 10;
	speed = SPEED + 1;
	countingTime = 0;
	maxHP = 300;
	maxMP = 100;
	currentHP = maxMP;
	currentMP = maxMP;
	gold = 10000;
	preventRun = 0;
	arrowRange = 250;
	pace = std::sqrt(2 * speed*speed) / 2;

	slash = new Slash();
	for (int i = 0; i < 10; i++)
	{
		auto sprite = ResourceManager::GetInstance()->DuplicateSprite(ResourceManager::GetInstance()->GetSpriteById(14));
		sprite->retain();
		sprite->setScaleX(0.6f);
		Arrow *arrow = new Arrow(sprite, MainCharacter::NORMAL_ARROW_BITMASK);
		arrow->SetVisible(false);
		arrow->SetDistance(0);
		mArrows.push_back(arrow);
	}

	flySlash = new FlySlash();
	flySlash->GetSprite()->retain();

	for (int i = 0; i < 10; i++)
	{
		heartContainerCollect.push_back(false);
	}
}

void MainCharacter::Refresh()
{
	direction = 2;
	currentState = FRONT_IDLE;
	currentHP = maxHP;
	currentMP = maxMP;
}

void MainCharacter::SetState(int nextState)
{
	if (currentState != DEAD)
	{
		if (currentState == nextState && mSprite->getNumberOfRunningActions() == 0)
		{
			mSprite->runAction(mAction[nextState]);
		}
		else if (currentState == nextState)
		{
			return;
		}
		else
		{
			switch (nextState)
			{
			case BACK_IDLE:
			case FRONT_IDLE:
			case LEFT_IDLE:
				if (currentState == FRONT_IDLE || currentState == LEFT_IDLE || currentState == BACK_IDLE || mSprite->getNumberOfRunningActions() == 0)
				{
					mSprite->stopAllActions();
					mSprite->runAction(mAction[nextState]);
					currentState = nextState;
				}
				break;
			case GO_UP:
			case GO_DOWN:
			case GO_LEFT:
				mSprite->stopAllActions();
				mSprite->runAction(mAction[nextState]);
				currentState = nextState;
				break;
			case FRONT_SLASH:
			case BACK_SLASH:
			case LEFT_SLASH:
				currentState = nextState;
				mSprite->stopAllActions();
				mSprite->runAction(mAction[nextState]);
				break;
			case ROLL_BACK:
			case ROLL_FRONT:
			case ROLL_LEFT:
				currentState = nextState;
				mSprite->stopAllActions();
				mSprite->runAction(mAction[nextState]);
				break;
			case FRONT_SHIELD:
			case BACK_SHIELD:
			case LEFT_SHIELD:
				currentState = nextState;
				mSprite->stopAllActions();
				mSprite->runAction(mAction[nextState]);
				break;
			case FRONT_ARCHERY:
			case BACK_ARCHERY:
			case LEFT_ARCHERY:
				currentState = nextState;
				mSprite->stopAllActions();
				mSprite->runAction(mAction[nextState]);
				break;
			case FRONT_GET_DAMAGE:
			case BACK_GET_DAMAGE:
			case LEFT_GET_DAMAGE:
				currentState = nextState;
				mSprite->stopAllActions();
				mSprite->runAction(mAction[nextState]);
				break;
			case DEAD:
				currentState = nextState;
				mSprite->stopAllActions();
				mSprite->runAction(mAction[nextState]);
				break;
			}
		}
	}
}

void MainCharacter::Update(float deltaTime)
{
	if (IsAlive())
	{
		Idle();
		for (int i = 0; i < mArrows.size(); i++)
		{
			if (mArrows[i]->IsVisible())
			{
				mArrows[i]->update(deltaTime);
			}
		}

		AutoRevive(deltaTime);
	}
}

void MainCharacter::Idle()
{
	if (currentState != FRONT_SHIELD && currentState != BACK_SHIELD && currentState != LEFT_SHIELD && mSprite->getNumberOfRunningActions() == 0)
	{
		if (currentState == BACK_SLASH || currentState == FRONT_SLASH || currentState == LEFT_SLASH)
		{
			slash->GetSprite()->setPosition(Vec2(-1, -1));
		}
		switch (direction)
		{
		case 1:
			SetState(BACK_IDLE);
			break;
		case 2:
			SetState(FRONT_IDLE);
			break;
		case 3:
			SetState(LEFT_IDLE);
			break;
		case 4:
			SetState(LEFT_IDLE);
			break;
		case 5:
			if (currentState == GO_UP || currentState == BACK_ARCHERY || currentState == BACK_SLASH || currentState == ROLL_BACK)
			{
				SetState(BACK_IDLE);
			}
			else if(currentState != BACK_IDLE)
			{
				SetState(LEFT_IDLE);
			}
			else
			{
				SetState(BACK_IDLE);
			}
			break;
		case 6:
			if (currentState == GO_DOWN || currentState == FRONT_ARCHERY || currentState == FRONT_SLASH || currentState == ROLL_FRONT)
			{
				SetState(FRONT_IDLE);
			}
			else if (currentState != FRONT_IDLE)
			{
				SetState(LEFT_IDLE);
			}
			else
			{
				SetState(FRONT_IDLE);
			}
			break;
		case 7:
			if (currentState == GO_UP || currentState == BACK_ARCHERY || currentState == BACK_SLASH || currentState == ROLL_BACK)
			{
				SetState(BACK_IDLE);
			}
			else if (currentState != BACK_IDLE)
			{
				SetState(LEFT_IDLE);
			}
			else
			{
				SetState(BACK_IDLE);
			}
			break;
		case 8:
			if (currentState == GO_DOWN || currentState == FRONT_ARCHERY || currentState == FRONT_SLASH || currentState == ROLL_FRONT)
			{
				SetState(FRONT_IDLE);
			}
			else if (currentState != FRONT_IDLE)
			{
				SetState(LEFT_IDLE);
			}
			else
			{
				SetState(FRONT_IDLE);
			}
			break;
		}
	}
}

void MainCharacter::Defend()
{
	if (currentMP >= 80)
	{
		if (currentState == GO_UP || currentState == GO_DOWN || currentState == GO_LEFT || currentState == FRONT_IDLE || currentState == LEFT_IDLE || currentState == BACK_IDLE)
		{
			flySlash->SetDirection(direction);
			flySlash->SetPosition();
			SimpleAudioEngine::getInstance()->playEffect("audio/slash/ultiSlash.mp3", false);
			switch (direction)
			{
			case 1:
				SetState(BACK_SLASH);
				break;
			case 2:
				SetState(FRONT_SLASH);
				break;
			case 3:
				SetState(LEFT_SLASH);
				break;
			case 4:
				SetState(LEFT_SLASH);
				break;
			case 5:
				if (currentState == BACK_IDLE || currentState == GO_UP)
				{
					SetState(BACK_SLASH);
				}
				else
				{
					SetState(LEFT_SHIELD);
				}
				break;
			case 6:
				if (currentState == FRONT_IDLE || currentState == GO_DOWN)
				{
					SetState(FRONT_SLASH);
				}
				else
				{
					SetState(LEFT_SLASH);
				}
				break;
			case 7:
				if (currentState == BACK_IDLE || currentState == GO_UP)
				{
					SetState(BACK_SLASH);
				}
				else
				{
					SetState(LEFT_SLASH);
				}
				break;
			case 8:
				if (currentState == FRONT_IDLE || currentState == GO_DOWN)
				{
					SetState(FRONT_SLASH);
				}
				else
				{
					SetState(LEFT_SLASH);
				}
				break;
			}
			currentMP -= 80;
		}
	}
}

void MainCharacter::StopDefend()
{
	switch (direction)
	{
	case 1:
		SetState(BACK_IDLE);
		break;
	case 2:
		SetState(FRONT_IDLE);
		break;
	case 3:
		SetState(LEFT_IDLE);
		break;
	case 4:
		SetState(LEFT_IDLE);
		break;
	case 5:
		if (currentState == FRONT_SHIELD)
		{
			SetState(BACK_IDLE);
		}
		else
		{
			SetState(LEFT_IDLE);
		}
		break;
	case 6:
		if (currentState == BACK_SHIELD)
		{
			SetState(FRONT_IDLE);
		}
		else
		{
			SetState(LEFT_IDLE);
		}
		break;
	case 7:
		if (currentState == FRONT_SHIELD)
		{
			SetState(BACK_IDLE);
		}
		else
		{
			SetState(LEFT_IDLE);
		}
		break;
	case 8:
		if (currentState == BACK_SHIELD)
		{
			SetState(FRONT_IDLE);
		}
		else
		{
			SetState(LEFT_IDLE);
		}
		break;
	}
}

void MainCharacter::Fire(int direction)
{
	for (int i = 0; i < mArrows.size(); i++)
	{
		if (!mArrows[i]->IsVisible())
		{
			mArrows[i]->SetRotate(180);
			mArrows[i]->SetPosition(mSprite->getPosition());
			mArrows[i]->SetVisible(true);
			mArrows[i]->SetDirection(direction);
			break;
		}
	}
}

void MainCharacter::SetListArrow(std::vector<Arrow*> mArrows)
{
	this->mArrows = mArrows;
}

std::vector<Arrow*> MainCharacter::GetListArrow()
{
	return mArrows;
}

void MainCharacter::SpecialAttack()
{
	if (currentState == GO_UP || currentState == GO_DOWN || currentState == GO_LEFT || currentState == FRONT_IDLE || currentState == LEFT_IDLE || currentState == BACK_IDLE
		|| currentState == FRONT_SHIELD || currentState == BACK_SHIELD || currentState == LEFT_SHIELD)
	{
		SimpleAudioEngine::getInstance()->playEffect("audio/archery/arrow.wav", false);
		switch (direction)
		{
		case 1:
			SetState(BACK_ARCHERY);
			Fire(2);
			break;
		case 2:
			SetState(FRONT_ARCHERY);
			Fire(3);
			break;
		case 4:
			SetState(LEFT_ARCHERY);
			Fire(1);
			break;
		case 5:
			SetState(LEFT_ARCHERY);
			Fire(4);
			break;
		case 7:
			SetState(LEFT_ARCHERY);
			Fire(5);
			break;
		case 6:
			SetState(LEFT_ARCHERY);
			Fire(7);
			break;
		case 8:
			SetState(FRONT_ARCHERY);
			Fire(6);
			break;
		default:
			SetState(LEFT_ARCHERY);
			Fire(0);
			break;
		}
	}
}

void MainCharacter::NormalAttack()
{
	if (currentState == GO_UP || currentState == GO_DOWN || currentState == GO_LEFT || currentState == FRONT_IDLE || currentState == LEFT_IDLE 
		|| currentState == BACK_IDLE || currentState == FRONT_SHIELD || currentState == BACK_SHIELD || currentState == LEFT_SHIELD)
	{
		SimpleAudioEngine::getInstance()->playEffect("audio/slash/slash.mp3", false);
		switch (direction)
		{
		case 1:
			SetState(BACK_SLASH);
			slash->GetSprite()->setPosition(Vec2(mSprite->getPositionX(), mSprite->getPositionY() + 20));
			slash->GetSprite()->setRotation(0);
			break;
		case 2:
			SetState(FRONT_SLASH);
			slash->GetSprite()->setPosition(Vec2(mSprite->getPositionX(), mSprite->getPositionY() - 30));
			slash->GetSprite()->setRotation(0);
			break;
		case 3:
			SetState(LEFT_SLASH);
			slash->GetSprite()->setPosition(Vec2(mSprite->getPositionX() - 25, mSprite->getPositionY()));
			slash->GetSprite()->setRotation(90);
			break;
		case 4:
			SetState(LEFT_SLASH);
			slash->GetSprite()->setPosition(Vec2(mSprite->getPositionX() + 25, mSprite->getPositionY()));
			slash->GetSprite()->setRotation(90);
			break;
		case 5:
			if (currentState == BACK_SHIELD || currentState == BACK_IDLE || currentState == GO_UP)
			{
				SetState(BACK_SLASH);
			}
			else
			{
				SetState(LEFT_SLASH);
			}
			slash->GetSprite()->setPosition(Vec2(mSprite->getPositionX() - 15, mSprite->getPositionY() + 15));
			slash->GetSprite()->setRotation(135);
			break;
		case 6:
			if (currentState == FRONT_SHIELD || currentState == FRONT_IDLE || currentState == GO_DOWN)
			{
				SetState(FRONT_SLASH);
			}
			else
			{
				SetState(LEFT_SLASH);
			}
			slash->GetSprite()->setPosition(Vec2(mSprite->getPositionX() - 15, mSprite->getPositionY() - 15));
			slash->GetSprite()->setRotation(-135);
			break;
		case 7:
			if (currentState == BACK_SHIELD || currentState == BACK_IDLE || currentState == GO_UP)
			{
				SetState(BACK_SLASH);
			}
			else
			{
				SetState(LEFT_SLASH);
			}
			slash->GetSprite()->setPosition(Vec2(mSprite->getPositionX() + 15, mSprite->getPositionY() + 15));
			slash->GetSprite()->setRotation(45);
			break;
		case 8:
			if (currentState == FRONT_SHIELD || currentState == FRONT_IDLE || currentState == GO_DOWN)
			{
				SetState(FRONT_SLASH);
			}
			else
			{
				SetState(LEFT_SLASH);
			}
			slash->GetSprite()->setPosition(Vec2(mSprite->getPositionX() + 15, mSprite->getPositionY() - 15));
			slash->GetSprite()->setRotation(-45);
			break;
		}
	}
}

void MainCharacter::Evade()
{
	if ((currentState == GO_UP || currentState == GO_DOWN || currentState == GO_LEFT || currentState == FRONT_IDLE || currentState == LEFT_IDLE 
		|| currentState == BACK_IDLE || currentState == FRONT_SHIELD || currentState == BACK_SHIELD || currentState == LEFT_SHIELD)
		&& currentMP >= 10)
	{
		switch (direction)
		{
		case 1:
			SetState(ROLL_BACK);
			mSprite->runAction(MoveBy::create(0.5f, Vec2(0, 100)));
			break;
		case 2:
			SetState(ROLL_FRONT);
			mSprite->runAction(MoveBy::create(0.5f, Vec2(0, -100)));
			break;
		case 3:
			SetState(ROLL_LEFT);
			mSprite->runAction(MoveBy::create(0.5f, Vec2(-100, 0)));
			break;
		case 4:
			SetState(ROLL_LEFT);
			mSprite->runAction(MoveBy::create(0.5f, Vec2(100, 0)));
			break;
		case 5:
			if (currentState == BACK_IDLE || currentState == GO_UP || BACK_SHIELD)
			{
				SetState(ROLL_BACK);
			}
			else
			{
				SetState(ROLL_LEFT);
			}
			mSprite->runAction(MoveBy::create(0.5f, Vec2(-70.710678, 70.710678)));
			break;
		case 6:
			if (currentState == FRONT_IDLE || currentState == GO_DOWN || currentState == FRONT_SHIELD)
			{
				SetState(ROLL_FRONT);
			}
			else
			{
				SetState(ROLL_LEFT);
			}
			mSprite->runAction(MoveBy::create(0.5f, Vec2(-70.710678, -70.710678)));
			break;
		case 7:
			if (currentState == BACK_IDLE || currentState == GO_UP || currentState == BACK_SHIELD)
			{
				SetState(ROLL_BACK);
			}
			else
			{
				SetState(ROLL_LEFT);
			}
			mSprite->runAction(MoveBy::create(0.5f, Vec2(70.710678, 70.710678)));
			break;
		case 8:
			if (currentState == FRONT_IDLE || currentState == GO_DOWN || currentState == FRONT_SHIELD)
			{
				SetState(ROLL_FRONT);
			}
			else
			{
				SetState(ROLL_LEFT);
			}
			mSprite->runAction(MoveBy::create(0.5f, Vec2(70.710678, -70.710678)));
		}
		currentMP -= 10;
	}
}

void MainCharacter::SetPreventRun(int prevent)
{
	preventRun = prevent;
}

void MainCharacter::Run(float percentSpeed)
{
	if ((currentState == FRONT_SHIELD || currentState == BACK_SHIELD || currentState == LEFT_SHIELD || currentState == FRONT_IDLE
		|| currentState == LEFT_IDLE || currentState == BACK_IDLE || currentState == GO_UP || currentState == GO_LEFT || currentState == GO_DOWN) && currentHP > 0)
	{
		pace = std::sqrt(2 * speed*speed) / 2;
		switch (direction)
		{
		case 1:
			SetState(GO_UP);
			if (preventRun != 1)
			{
				mSprite->setPositionY(mSprite->getPositionY() + speed * percentSpeed);
				preventRun = 0;
			}
			break;
		case 2:
			SetState(GO_DOWN);
			if (preventRun != 2)
			{
				mSprite->setPositionY(mSprite->getPositionY() - speed * percentSpeed);
				preventRun = 0;
			}
			break;
		case 3:
			SetState(GO_LEFT);
			if (preventRun != 3)
			{
				mSprite->setPositionX(mSprite->getPositionX() - speed * percentSpeed);
				preventRun = 0;
			}
			break;
		case 4:
			SetState(GO_LEFT);
			if (preventRun != 4)
			{
				mSprite->setPositionX(mSprite->getPositionX() + speed * percentSpeed);
				preventRun = 0;
			}
			break;
		case 5:
			if (currentState == BACK_IDLE || currentState == GO_UP || currentState == BACK_SHIELD)
			{
				SetState(GO_UP);
			}
			else
			{
				SetState(GO_LEFT);
			}
			if (preventRun != 5)
			{
				mSprite->setPosition(mSprite->getPositionX() - pace * percentSpeed, mSprite->getPositionY() + pace * percentSpeed);
				preventRun = 0;
			}
			break;
		case 6:
			if (currentState == FRONT_IDLE || currentState == GO_DOWN || currentState == FRONT_SHIELD)
			{
				SetState(GO_DOWN);
			}
			else
			{
				SetState(GO_LEFT);
			}
			if (preventRun != 6)
			{
				mSprite->setPosition(mSprite->getPositionX() - pace * percentSpeed, mSprite->getPositionY() - pace * percentSpeed);
				preventRun = 0;
			}
			break;
		case 7:
			if (currentState == BACK_IDLE || currentState == GO_UP || currentState == BACK_SHIELD)
			{
				SetState(GO_UP);
			}
			else
			{
				SetState(GO_LEFT);
			}
			if (preventRun != 7)
			{
				mSprite->setPosition(mSprite->getPositionX() + pace * percentSpeed, mSprite->getPositionY() + pace * percentSpeed);
				preventRun = 0;
			}
			break;
		case 8:
			if (currentState == FRONT_IDLE || currentState == GO_DOWN || currentState == FRONT_SHIELD)
			{
				SetState(GO_DOWN);
			}
			else
			{
				SetState(GO_LEFT);
			}
			if (preventRun != 8)
			{
				mSprite->setPosition(mSprite->getPositionX() + pace * percentSpeed, mSprite->getPositionY() - pace * percentSpeed);
				preventRun = 0;
			}
		}
	}
}

void MainCharacter::GetDamage(int damage)
{
	currentHP += defend - damage;
	slash->GetSprite()->setPosition(Vec2(-100, -100));
	SimpleAudioEngine::getInstance()->playEffect("audio/hit.mp3", false);
	switch (direction)
	{
	case 1:
		SetState(BACK_GET_DAMAGE);
		break;
	case 2:
		SetState(FRONT_GET_DAMAGE);
		break;
	case 3:
		SetState(LEFT_GET_DAMAGE);
		break;
	case 4:
		SetState(LEFT_GET_DAMAGE);
		break;
	case 5:
		if (currentState == BACK_IDLE || currentState == GO_UP || currentState == BACK_SLASH || currentState == BACK_ARCHERY)
		{
			SetState(BACK_GET_DAMAGE);
		}
		else
		{
			SetState(LEFT_GET_DAMAGE);
		}
		break;
	case 6:
		if (currentState == FRONT_IDLE || currentState == GO_DOWN || currentState == FRONT_SLASH || currentState == FRONT_ARCHERY)
		{
			SetState(FRONT_GET_DAMAGE);
		}
		else
		{
			SetState(LEFT_GET_DAMAGE);
		}
		break;
	case 7:
		if (currentState == BACK_IDLE || currentState == GO_UP || currentState == BACK_SLASH || currentState == BACK_ARCHERY)
		{
			SetState(BACK_GET_DAMAGE);
		}
		else
		{
			SetState(LEFT_GET_DAMAGE);
		}
		break;
	case 8:
		if (currentState == FRONT_IDLE || currentState == GO_DOWN || currentState == FRONT_SLASH || currentState == FRONT_ARCHERY)
		{
			SetState(FRONT_GET_DAMAGE);
		}
		else
		{
			SetState(LEFT_GET_DAMAGE);
		}
		break;
	}
}

void MainCharacter::StopRun()
{
	mSprite->stopAction(mAction[GO_UP]);
	mSprite->stopAction(mAction[GO_DOWN]);
	mSprite->stopAction(mAction[GO_LEFT]);
}

bool MainCharacter::IsAlive()
{
	if (currentHP <= 0)
	{
		SetState(DEAD);
		return false;
	}
	return true;
}

void MainCharacter::AutoRevive(float deltaTime)
{
	if (currentMP < maxMP)
	{
		countingTime += deltaTime;
		if (countingTime >= 0.5)
		{
			countingTime = 0;
			currentMP += 2;
			if (currentMP > maxMP)
			{
				currentMP = maxMP;
			}
		}
	}
}

float MainCharacter::GetPercentHP()
{
	return currentHP / maxHP * 100.0;
}

float MainCharacter::GetPercentMP()
{
	return currentMP / maxMP * 100.0;
}

PhysicsBody* MainCharacter::GetPhysicsBody()
{
	return mPhysicsBody;
}

int MainCharacter::GetArrowAttack()
{
	return arrowAttack;
}

void MainCharacter::TakeArrowAttack(int id)
{
	SimpleAudioEngine::getInstance()->playEffect("audio/inventorySoundEffects/metalClash.wav", false);
	switch (id)
	{
	case 6:
		arrowAttack = 30;
		break;
	case 7:
		arrowAttack = 35;
		break;
	case 8:
		arrowAttack = 40;
		break;
	case 9:
		arrowAttack = 45;
		break;
	case 10:
		arrowAttack = 50;
		break;
	case 11:
		arrowAttack = 60;
		break;
	case 12:
		arrowAttack = 70;
		break;
	default:
		break;
	}
}

int MainCharacter::GetArrowRange()
{
	return arrowRange;
}

void MainCharacter::TakeBow(int id)
{
	SimpleAudioEngine::getInstance()->playEffect("audio/inventorySoundEffects/metalClash.wav", false);
	switch (id)
	{
	case 17:
		arrowRange = 250;
		break;
	case 18:
		arrowRange = 300;
		break;
	case 19:
		arrowRange = 350;
		break;
	case 20:
		arrowRange = 400;
		break;
 	default:
		break;
	}
}

int MainCharacter::GetSpeed()
{
	return speed;
}

int MainCharacter::GetCurrentState()
{
	return currentState;
}

Sprite* MainCharacter::GetSprite()
{
	return mSprite;
}

std::string MainCharacter::GetName()
{
	return mName;
}

Inventory * MainCharacter::GetInventory()
{
	return inventory;
}

int MainCharacter::GetStageLevel()
{
	return stageLevel;
}

int MainCharacter::GetDirection()
{
	return direction;
}

int MainCharacter::GetAttack()
{
	return attack;
}

void MainCharacter::SetDirection(int direction)
{
	switch (direction)
	{
	case 1:
		mSprite->setFlippedX(false);
		break;
	case 2:
		mSprite->setFlippedX(false);
		break;
	case 3:
		mSprite->setFlippedX(false);
		break;
	case 4:
		mSprite->setFlippedX(true);
		break;
	case 5:
		mSprite->setFlippedX(false);
		break;
	case 6:
		mSprite->setFlippedX(false);
		break;
	case 7:
		mSprite->setFlippedX(true);
		break;
	case 8:
		mSprite->setFlippedX(true);
		break;
	}
	this->direction = direction;
}

bool MainCharacter::TakePotion(int index)
{
	switch (index)
	{
	case 21:
		if (currentHP < maxHP)
		{
			currentHP += HP;
			if (currentHP > maxHP)
			{
				currentHP = maxHP;
			}
			return true;
		}
		return false;
	case 22:
		if (currentMP < maxMP)
		{
			currentMP += MP;
			if (currentMP > maxMP)
			{
				currentMP = maxMP;
			}
			return true;
		}
		return false;
	case 23:
		if (currentHP < maxHP || currentMP < maxMP)
		{
			currentHP += 30;
			currentMP += 5;
			return true;
		}
		return false;
	case 24:
		return false;
	case 25:
		if (currentHP < maxHP || currentMP < maxMP)
		{
			currentHP += 50;
			currentMP += 20;
			return true;
		}
		return false;
	default:
		return false;
		break;
	}
}

void MainCharacter::IncreaseStage()
{
	stageLevel += 1;
}

int MainCharacter::GetGold()
{
	return gold;
}

void MainCharacter::AddGold(int numb)
{
	gold += numb;
	SimpleAudioEngine::getInstance()->playEffect("audio/inventorySoundEffects/sellBuyItem.wav", false);
}

void MainCharacter::SubGold(int numb)
{
	gold -= numb;
	SimpleAudioEngine::getInstance()->playEffect("audio/inventorySoundEffects/sellBuyItem.wav", false);
}

float MainCharacter::GetPace()
{
	return pace;
}

int MainCharacter::GetHeartContainer()
{
	return heartContainer;
}

void MainCharacter::TakeHeartContainer()
{
	SimpleAudioEngine::getInstance()->playEffect("audio/takeHeartContainer/confirm.wav", false);
	heartContainer++;
	maxHP += 200;
	currentHP = maxHP;
}

FlySlash* MainCharacter::GetFlySlash()
{
	return flySlash;
}

std::vector<Item*> MainCharacter::GetEquipedItem()
{
	return mItems;
}

void MainCharacter::EquipedItem(int index, Item item)
{
	mItems[index]->GetIcon()->removeFromParent();
	mItems[index] = new Item(item);
	switch (index)
	{
	case 0:
		TakeSword(mItems[0]->GetID());
		break;
	case 2:
		TakeArmor(mItems[2]->GetID());
		break;
	case 3:
		TakeBoots(mItems[3]->GetID());
		break;
	case 1:
		TakeBow(mItems[1]->GetID());
		break;
	case 4:
		TakeArrowAttack(mItems[4]->GetID());
		break;
	default:
		break;
	}
}

int MainCharacter::GetMaxHP()
{
	return maxHP;
}

int MainCharacter::GetCurrentHP()
{
	return currentHP;
}

int MainCharacter::GetMaxMP()
{
	return maxMP;
}

int MainCharacter::GetCurrentMP()
{
	return maxMP;
}

int MainCharacter::GetDefend()
{
	return defend;
}

void MainCharacter::TakeSword(int index)
{
	SimpleAudioEngine::getInstance()->playEffect("audio/inventorySoundEffects/metalClash.wav", false);
	switch (index)
	{
	case 26:
		attack = ATTACK + 20;
		break;
	case 27:
		attack = ATTACK + 40;
		break;
	case 28:
		attack = ATTACK + 60;
		break;
	case 29:
		attack = ATTACK + 80;
		break;
	case 30:
		attack = ATTACK + 90;
		break;
	case 31:
		attack = ATTACK + 100;
		break;
	}
}

void MainCharacter::TakeArmor(int index)
{
	SimpleAudioEngine::getInstance()->playEffect("audio/inventorySoundEffects/clothInventory.wav", false);
	switch (index)
	{
	case 1:
		defend = DEFEND + 10;
		break;
	case 2:
		defend = DEFEND + 14;
		break;
	case 3:
		defend = DEFEND + 18;
		break;
	case 4:
		defend = DEFEND + 22;
		break;
	case 5:
		defend = DEFEND + 26;
		break;
	}
}

void MainCharacter::TakeBoots(int index)
{
	SimpleAudioEngine::getInstance()->playEffect("audio/inventorySoundEffects/leatherInventory.wav", false);
	switch (index)
	{
	case 13:
		speed = SPEED + 1;
		break;
	case 14:
		speed = SPEED + 2;
		break;
	case 15:
		speed = SPEED + 3;
		break;
	case 16:
		speed = SPEED + 4;
		break;
	}
}

bool MainCharacter::GetCheckHeartCollect(int stage)
{
	return heartContainerCollect[stage - 1];
}

void MainCharacter::SetHeartCollected(int stage)
{
	heartContainerCollect[stage - 1] = true;
}