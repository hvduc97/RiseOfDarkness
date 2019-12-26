#include "Item.h"
#include "Inventory.h"
#include "ui/CocosGUI.h"
#include "cocos2d.h"
#include "MainCharacter.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;
using namespace std;
using namespace ui;
using namespace CocosDenshion;

Inventory::Inventory(cocos2d::Sprite* sprite)
{
	Init(sprite);
	btnUse->removeFromParent();
	btnUse->setPosition(Vec2(btnUse->getContentSize().width / 2 - btnUse->getContentSize().width / 4,
		0 - btnUse->getContentSize().height / 4));
	btnSell->removeFromParent();
	btnSell->setPosition(Vec2(btnUse->getPositionX() + btnUse->getContentSize().width/2, btnUse->getPositionY()));
	GetTab(1)->addChild(btnUse, 99);
	GetTab(1)->addChild(btnSell, 99);
}

Inventory::~Inventory()
{
}

int Inventory::GetCapacity(ItemType type)
{
	int size = 0;
	switch (type)
	{
	case ItemType::weapon:
		size = 0;
		for (int i = 0; i < weapons.size(); i++)
		{
			if (weapons[i]->GetID() != 99)
				size++;
		}
		return size;
		break;
	case ItemType::potion:
		size = 0;
		for (int i = 0; i < slots.size(); i++)
		{
			if (slots[i]->GetID() != 99)
				size++;
		}
		return size;
		break;
	case ItemType::armor:
		size = 0;
		for (int i = 0; i < armors.size(); i++)
		{
			if (armors[i]->GetID() != 99)
				size++;
		}
		return size;
		break;
	case ItemType::boots:
		size = 0;
		for (int i = 0; i < boots.size(); i++)
		{
			if (boots[i]->GetID() != 99)
				size++;
		}
		return size;
		break;
	case ItemType::arrow:
		size = 0;
		for (int i = 0; i < arrows.size(); i++)
		{
			if (arrows[i]->GetID() != 99)
				size++;
		}
		return size;
		break;
	default:
		return 0;
		break;
	}
}

void Inventory::Init(cocos2d::Sprite* sprite)
{
	clickBox = Sprite::create("res/sprites/item/click.png");
	btnUse = ui::Button::create("res/sprites/item/buttonUse1.png", "res/sprites/item/buttonUse.png");
	btnUse->setScale(0.5);
	btnUse->retain();
	btnSell = ui::Button::create("res/sprites/item/buttonSell1.png", "res/sprites/item/buttonSell.png");
	btnSell->setScale(0.5);
	btnSell->retain();
	
	clickBox->setPosition(-500, -500);
	clickBox->retain();
	slotX = 6;
	slotY = 4;
	database = new ItemDatabase();
	database->Init();
	showTooltip = false;
	mSprite = sprite;
	tab = ui::TabControl::create();
	tab->setContentSize(Size(384, 325));
	tab->setHeaderHeight(69.0f);
	tab->setHeaderWidth(70.f);
	tab->setHeaderSelectedZoom(.1f);
	tab->setHeaderDockPlace(ui::TabControl::Dock::TOP);
	weapon = cocos2d::ui::TabHeader::create();
	weapon->setTitleText("WEAPON");
	potion = cocos2d::ui::TabHeader::create();
	potion->setTitleText("POTION");
	armor = cocos2d::ui::TabHeader::create();
	armor->setTitleText("ARMOR");
	arrow = cocos2d::ui::TabHeader::create();
	arrow->setTitleText("ARROW");
	boot = cocos2d::ui::TabHeader::create();
	boot->setTitleText("BOOT");
	capacity = 24;
	container1 = ui::Layout::create();
	container1->setOpacity(255);
	container2 = ui::Layout::create();
	container2->setOpacity(50);
	container3 = ui::Layout::create();
	container3->setOpacity(50);
	arrowContainer = ui::Layout::create();
	arrowContainer->setOpacity(50);
	bootContainer = ui::Layout::create();
	bootContainer->setOpacity(50);
	tab->insertTab(0, weapon, container1);
	tab->insertTab(1, potion, container2);
	tab->insertTab(2, armor, container3);
	tab->insertTab(3, arrow, arrowContainer);
	tab->insertTab(4, boot, bootContainer);
	tab->setSelectTab(1);
	
	CC_SAFE_RETAIN(tab);
	CC_SAFE_RETAIN(container1);
	CC_SAFE_RETAIN(container2);
	CC_SAFE_RETAIN(container3);
	CC_SAFE_RETAIN(arrowContainer);
	CC_SAFE_RETAIN(bootContainer);

	for (int i = 0; i < slotX*slotY; i++)
	{
		inventory.push_back(new Item("sprites/item/box"));
		slots.push_back(new Item("sprites/item/box"));
		weapons.push_back(new Item("sprites/item/box"));
		arrows.push_back(new Item("sprites/item/box"));
		armors.push_back(new Item("sprites/item/box"));
		boots.push_back(new Item("sprites/item/box"));
		itemAmount.push_back(0);
		arrowAmount.push_back(0);
		auto label = Label::createWithTTF("", "fonts/Marker Felt.ttf", 12);
		label->retain();
		auto label1 = Label::createWithTTF("", "fonts/Marker Felt.ttf", 12);
		label1->retain();
		amountArrowLabels.push_back(label1);
		amountLabels.push_back(label);
	}
}

void Inventory::AddItem(int id)
{
	int index = GetIndexByID(id);
	switch (database->items[index]->GetType())
	{
	case ItemType::potion:
		if (InventoryContains(id, ItemType::potion))
		{
			StackItem(id, ItemType::potion);
		}
		else
		{
			for (int i = 0; i < inventory.size(); i++)
			{
				if (slots[i]->GetID() == 99)
				{
					slots[i] = new Item(database->items[index]);
					slots[i]->GetIcon()->addClickEventListener(CC_CALLBACK_1(Inventory::ItemClick, this, i, ItemType::potion));
					itemAmount[i]++;
					break;
				}
			}
		}
		break;
	case ItemType::arrow:
		if (InventoryContains(id, ItemType::arrow))
		{
			StackItem(id, ItemType::arrow);
		}
		else
		{
			for (int i = 0; i < inventory.size(); i++)
			{
				if (arrows[i]->GetID() == 99)
				{
					arrows[i] = new Item(database->items[index]);
					arrows[i]->GetIcon()->addClickEventListener(CC_CALLBACK_1(Inventory::ItemClick, this, i, ItemType::arrow));
					arrowAmount[i]++;
					break;
				}
			}
		}
		break;
	case ItemType::weapon:
		for (int i = 0; i < inventory.size(); i++)
		{
			if (weapons[i]->GetID() == 99)
			{
				weapons[i] = new Item(database->items[index]);
				weapons[i]->GetIcon()->addClickEventListener(CC_CALLBACK_1(Inventory::ItemClick, this, i, ItemType::weapon));
				break;
			}
		}
		break;
	case ItemType::armor:
		for (int i = 0; i < inventory.size(); i++)
		{
			if (armors[i]->GetID() == 99)
			{
				armors[i] = new Item(database->items[index]);
				armors[i]->GetIcon()->addClickEventListener(CC_CALLBACK_1(Inventory::ItemClick, this, i, ItemType::armor));
				break;
			}
		}
		break;
	case ItemType::boots:
		for (int i = 0; i < inventory.size(); i++)
		{
			if (boots[i]->GetID() == 99)
			{
				boots[i] = new Item(database->items[index]);
				boots[i]->GetIcon()->addClickEventListener(CC_CALLBACK_1(Inventory::ItemClick, this, i, ItemType::boots));
				break;
			}
		}
		break;
	default:
		break;
	}
}

void Inventory::SellItem(int id, int index, ItemType type)
{
	switch (type)
	{
	case ItemType::weapon:
		if (targetID>=0)
		{
			switch (weapons[id]->GetID())
			{
			case 26:
				MainCharacter::GetInstance()->AddGold(100);
				break;
			case 27:
				MainCharacter::GetInstance()->AddGold(150);
				break;
			case 28:
				MainCharacter::GetInstance()->AddGold(250);
				break;
			case 29:
				MainCharacter::GetInstance()->AddGold(350);
				break;
			case 30:
				MainCharacter::GetInstance()->AddGold(500);
				break;
			case 31:
				MainCharacter::GetInstance()->AddGold(1500);
				break;
			case 17:
				MainCharacter::GetInstance()->AddGold(250);
				break;
			case 18:
				MainCharacter::GetInstance()->AddGold(250);
				break;
			case 19:
				MainCharacter::GetInstance()->AddGold(500);
				break;
			case 20:
				MainCharacter::GetInstance()->AddGold(1000);
				break;
			}
		}
		GetTab(0)->removeChild(weapons[id]->GetIcon());
		targetID = -1;
		weapons[id] = new Item("sprites/item/box");
		break;
	case ItemType::armor:
		if (targetID >= 0)
		{
			switch (armors[id]->GetID())
			{
			case 1:
				MainCharacter::GetInstance()->AddGold(150);
				break;
			case 2:
				MainCharacter::GetInstance()->AddGold(250);
				break;
			case 3:
				MainCharacter::GetInstance()->AddGold(500);
				break;
			case 4:
				MainCharacter::GetInstance()->AddGold(1000);
				break;
			case 5:
				MainCharacter::GetInstance()->AddGold(2500);
				break;
			}
		}
		GetTab(2)->removeChild(armors[id]->GetIcon());
		targetID = -1;
		armors[id] = new Item("sprites/item/box");
		break;
	case ItemType::boots:
		if (targetID >= 0)
		{
			switch (boots[id]->GetID())
			{
			case 13:
				MainCharacter::GetInstance()->AddGold(100);
				break;
			case 14:
				MainCharacter::GetInstance()->AddGold(250);
				break;
			case 15:
				MainCharacter::GetInstance()->AddGold(500);
				break;
			case 16:
				MainCharacter::GetInstance()->AddGold(1000);
				break;
			}
		}
		GetTab(4)->removeChild(boots[id]->GetIcon());
		targetID = -1;
		boots[id] = new Item("sprites/item/box");
		break;
	case ItemType::potion:
		
		itemAmount[id]--;
		if (itemAmount[id] <= 0)
		{
			amountLabels[id]->setString("");
			GetTab(1)->removeChild(slots[id]->GetIcon());
			targetID = -1;
			slots[id] = new Item("sprites/item/box");
			//AutoArrange();
		}
		else
		{
			switch (slots[id]->GetID())
			{
			case 21:
				MainCharacter::GetInstance()->AddGold(12);
				break;
			case 22:
				MainCharacter::GetInstance()->AddGold(10);
				break;
			case 23:
				MainCharacter::GetInstance()->AddGold(11);
				break;
			case 24:
				MainCharacter::GetInstance()->AddGold(7);
				break;
			case 25:
				MainCharacter::GetInstance()->AddGold(22);
				break;

			}
			amountLabels[id]->setString(to_string(itemAmount[id]));
		}
		log("removed item %d", id);
		break;
	case ItemType::arrow:
		arrowAmount[id]--;
		
		if (arrowAmount[id] <= 0)
		{
			amountArrowLabels[id]->setString("");
			GetTab(3)->removeChild(arrows[id]->GetIcon());
			targetID = -1;
			arrows[id] = new Item();
			//AutoArrange();
		}
		else
		{
			switch (arrows[id]->GetID())
			{
			case 6:
				MainCharacter::GetInstance()->AddGold(1);
				break;
			case 7:
				MainCharacter::GetInstance()->AddGold(2);
				break;
			case 8:
				MainCharacter::GetInstance()->AddGold(4);
				break;
			case 9:
				MainCharacter::GetInstance()->AddGold(6);
				break;
			case 10:
				MainCharacter::GetInstance()->AddGold(7);
				break;
			case 11:
				MainCharacter::GetInstance()->AddGold(10);
				break;
			case 12:
				MainCharacter::GetInstance()->AddGold(12);
				break;
			}
			amountArrowLabels[id]->setString(to_string(arrowAmount[id]));
		}
		break;
	default:
		break;
	}
}

void Inventory::RemoveItem(int id,int index,ItemType type)
{
	switch (type)
	{
	case ItemType::weapon:
		for (int  i = 0; i < inventory.size(); i++)
		{
			if (weapons[i]->GetID()==id && i==index)
			{
				GetTab(0)->removeChild(weapons[i]->GetIcon());
				targetID = -1;
				weapons[i] = new Item("sprites/item/box");

				break;
			}
		}
		break;
	case ItemType::armor:
		for (int i = 0; i < inventory.size(); i++)
		{
			if (armors[i]->GetID() == id && i == index)
			{
				GetTab(2)->removeChild(armors[i]->GetIcon());
				targetID = -1;
				armors[i] = new Item("sprites/item/box");

				break;
			}
		}
		break;
	case ItemType::boots:
		for (int i = 0; i < inventory.size(); i++)
		{
			if (boots[i]->GetID() == id && i == index)
			{
				GetTab(4)->removeChild(boots[i]->GetIcon());
				targetID = -1;
				boots[i] = new Item("sprites/item/box");

				break;
			}
		}
		break;
	case ItemType::potion:
		for (int i = 0; i < inventory.size(); i++)
		{
			if (slots[i]->GetID() == id && (id == 21 || id == 22 || id == 23 || id == 24 || id == 25) && slots[i]->GetID() != 99 && i == index)
			{
				if (MainCharacter::GetInstance()->TakePotion(id))
				{
					itemAmount[i]--;
					if (itemAmount[i] == 0)
					{
						amountLabels[i]->setString("");
						GetTab(1)->removeChild(slots[i]->GetIcon());
						targetID = -1;
						slots[i] = new Item("sprites/item/box");
						AutoArrange();
					}
					else
					{
						amountLabels[i]->setString(to_string(itemAmount[i]));
					}
					log("removed item %d", i);
					break;
				}
			}
			else
			{
				if (slots[i]->GetID() == id && i == index)
				{
					itemAmount[i]--;
					if (itemAmount[i] == 0)
					{
						amountLabels[i]->setString("");
						GetTab(1)->removeChild(slots[i]->GetIcon());
						targetID = -1;
						slots[i] = new Item("sprites/item/box");
						AutoArrange();
					}
					else
					{
						amountLabels[i]->setString(to_string(itemAmount[i]));
					}
					log("removed item %d", i);
					break;
				}
				
			}
		}
		break;
	case ItemType::arrow:
		for (int i = 0; i < inventory.size(); i++)
		{
			if (arrows[i]->GetID() == id && (id == 0 || id == 1) && i == index)
			{
				if (MainCharacter::GetInstance()->TakePotion(id))
				{
					arrowAmount[i]--;
					if (arrowAmount[i] == 0)
					{
						amountArrowLabels[i]->setString("");
						GetTab(3)->removeChild(arrows[i]->GetIcon());
						targetID = -1;
						arrows[i] = new Item("sprites/item/box");
						AutoArrange();
					}
					else
					{
						amountArrowLabels[i]->setString(to_string(arrowAmount[i]));
					}
					break;
				}
			}
			else
			{
				if (arrows[i]->GetID() == id && i == index)
				{
					arrowAmount[i]--;
					if (arrowAmount[i] == 0)
					{
						amountArrowLabels[i]->setString("");
						GetTab(1)->removeChild(arrows[i]->GetIcon());
						targetID = -1;
						arrows[i] = new Item();
						AutoArrange();
					}
					else
					{
						amountArrowLabels[i]->setString(to_string(arrowAmount[i]));
					}
					log("removed item %d", i);
					break;
				}
			}
		}
		break;
	default:
		break;
	}
}

cocos2d::Sprite * Inventory::GetClickBox()
{
	return clickBox;
}

void Inventory::SetVisible(bool b)
{
	tab->setVisible(b);
	mSprite->setVisible(b);
}

bool Inventory::IsVisible()
{
	return (tab->isVisible() && mSprite->isVisible());
}

void Inventory::SetSpritePosition(cocos2d::Vec2 pos)
{
	mSprite->setPosition(pos);
	tab->setPosition(pos);
	mSprite->setCameraMask(2);
	tab->setCameraMask(2);
}

cocos2d::Vec2 Inventory::GetSpritePosition()
{
	return mSprite->getPosition();
}

void Inventory::AddToLayer(cocos2d::Layer *layer)
{
	mSprite->removeFromParent();
	tab->removeFromParent();
	layer->addChild(mSprite, 16);
	layer->addChild(tab, 17);
}

cocos2d::ui::Layout *Inventory::GetTab(int tabIndex)
{
	switch (tabIndex)
	{
	case 0:
		return container1;
	case 1:
		return container2;
	case 2: 
		return container3;
	case 3:
		return arrowContainer;
	case 4:
		return bootContainer;
	default:
		return container3;
		break;
	}
}

std::vector<Item*> Inventory::GetItems()
{
	return slots;
}

std::vector<Item*> Inventory::GetItemsWeapon()
{
	return weapons;
}

std::vector<int> Inventory::GetItemAmount(int type)
{
	return (type == 0 ? itemAmount : arrowAmount);
}

std::vector<Item*> Inventory::GetArmors()
{
	return armors;
}

std::vector<Item*> Inventory::GetBoots()
{
	return boots;
}

std::vector<Item*> Inventory::GetArrows()
{
	return arrows;
}

std::vector<Label*> Inventory::GetAmountLabel(int type)
{
	if (type==0)
	{
		return amountLabels;
	}
	else
	{
		return amountArrowLabels;
	}
}

void Inventory::ShowCloseButton(Layout* layout)
{
	closeButton->removeFromParent();
	layout->addChild(closeButton, 22);
}

cocos2d::Vec2 Inventory::GetSize()
{
	return tab->getContentSize();
}

int Inventory::GetIdByIcon(int id, ItemType type)
{
	switch (type)
	{
	case ItemType::weapon:
		for (int i = 0; i < weapons.size(); i++)
		{
			if (weapons[id]->GetIcon() != NULL)
			{
				return weapons[id]->GetID();
			}
		}
		return -1;
		break;
	case ItemType::potion:
		for (int i = 0; i < slots.size(); i++)
		{
			if (slots[i]->GetID() == id)
			{
				return i;
			}
		}
		return -1;
		break;
	default:
		return -1;
		break;
	}
	
	
}

int Inventory::GetIndexByID(int id)
{
	for (int i = 0; i < database->items.size(); i++)
	{
		if (database->items[i]->GetID()==id)
		{
			return i;
		}
	}
	return -1;
}

void Inventory::StackItem(int id, ItemType type)
{
	switch (type)
	{
	case ItemType::potion:
		for (int i = 0; i < slots.size(); i++)
		{
			if (slots[i]->GetID() == id)
			{
				itemAmount[i]++;
				break;
			}
		}
	case ItemType::arrow:
		for (int i = 0; i < arrows.size(); i++)
		{
			if (arrows[i]->GetID() == id)
			{
				arrowAmount[i]++;
				break;
			}
		}
	default:
		break;
	}
}

void Inventory::AutoArrange()
{
	//ARRANGE POTION
	for (int i = 0; i < slots.size()-1; i++)
	{
		if (slots[i]->GetIcon()==NULL)
		{
			for (int j = i+1; j < slots.size(); j++)
			{
				if (slots[j]->GetIcon()!=NULL)
				{
					swap(slots[i], slots[j]);
					swap(itemAmount[i], itemAmount[j]);
					swap(amountLabels[i], amountLabels[j]);
					break;
				}
			}
		}
	}
	//ARRANGE WEAPON
	for (int i = 0; i < weapons.size() - 1; i++)
	{
		if (weapons[i]->GetIcon() == NULL)
		{
			for (int j = i + 1; j < weapons.size(); j++)
			{
				if (weapons[j]->GetIcon() != NULL)
				{
					swap(weapons[i], weapons[j]);
					break;
				}
			}
		}
	}
}

bool Inventory::InventoryContains(int id, ItemType type)
{
	bool result = false;
	switch (type)
	{
	case ItemType::potion:
		
		for (int i = 0; i < slots.size(); i++)
		{
			result = (slots[i]->GetID() == id && slots[i]->GetIcon() != NULL);
			if (result)
			{
				break;
			}
		}
		return result;
		break;
	case ItemType::arrow:
		for (int i = 0; i < arrows.size(); i++)
		{
			result = (arrows[i]->GetID() == id && arrows[i]->GetIcon() != NULL);
			if (result)
			{
				break;
			}
		}
		return result;
		break;
	default:
		return false;
		break;
	}
	
}

void Inventory::ItemClick(cocos2d::Ref *pSender, int id, ItemType type)
{
	switch (type)
	{
	case ItemType::potion:
		btnSell->removeFromParent();
		btnUse->removeFromParent();
		GetTab(1)->addChild(btnUse, 99);
		GetTab(1)->addChild(btnSell, 99);
		slots[id]->GetInfo()->removeFromParent();
		slots[id]->GetInfo()->setAnchorPoint(Vec2(0, 1));
		GetTab(1)->addChild(slots[id]->GetInfo(), 99);
		slots[id]->GetInfo()->setPosition(GetSize());
		slots[id]->GetInfo()->setCameraMask(2);
		if (slots[id]->GetIcon() != NULL)
		{
			clickBox->removeFromParent();
			GetTab(1)->addChild(clickBox, 22);
			clickBox->setPosition(slots[id]->GetIcon()->getPosition());
		}
		targetID = id;
		if (targetID >= 0)
		{
			btnSell->addClickEventListener(CC_CALLBACK_1(Inventory::btnSellItem, this, id, type));
			btnUse->addClickEventListener(CC_CALLBACK_1(Inventory::btnEquipInventory, this));
		}
		break;
	case ItemType::weapon:
		btnSell->removeFromParent();
		btnUse->removeFromParent();
		GetTab(0)->addChild(btnUse, 99);
		GetTab(0)->addChild(btnSell, 99);
		weapons[id]->GetInfo()->removeFromParent();
		weapons[id]->GetInfo()->setAnchorPoint(Vec2(0, 1));
		GetTab(0)->addChild(weapons[id]->GetInfo(), 99);
		weapons[id]->GetInfo()->setPosition(GetSize());
		weapons[id]->GetInfo()->setCameraMask(2);
		
		if (weapons[id]->GetIcon() != NULL)
		{
			clickBox->removeFromParent();
			GetTab(0)->addChild(clickBox, 22);
			
			clickBox->setPosition(weapons[id]->GetIcon()->getPosition());
		}
		targetID = id;
		if (targetID >= 0)
		{
			btnSell->addClickEventListener(CC_CALLBACK_1(Inventory::btnSellItem, this, id, type));
			btnUse->addClickEventListener(CC_CALLBACK_1(Inventory::EquipItem, this, id, type, weapons[id]->GetWeaponType()));
		}
		break;
	case ItemType::arrow:
		btnSell->removeFromParent();
		btnUse->removeFromParent();
		GetTab(3)->addChild(btnUse, 99);
		GetTab(3)->addChild(btnSell, 99);
		arrows[id]->GetInfo()->removeFromParent();
		arrows[id]->GetInfo()->setAnchorPoint(Vec2(0, 1));
		GetTab(3)->addChild(arrows[id]->GetInfo(), 99);
		arrows[id]->GetInfo()->setPosition(GetSize());
		arrows[id]->GetInfo()->setCameraMask(2);
		if (arrows[id]->GetIcon() != NULL)
		{
			clickBox->removeFromParent();
			GetTab(3)->addChild(clickBox, 22);

			clickBox->setPosition(arrows[id]->GetIcon()->getPosition());
		}
		targetID = id;
		if (targetID >= 0)
		{
			btnSell->addClickEventListener(CC_CALLBACK_1(Inventory::btnSellItem, this, id, type));
			btnUse->addClickEventListener(CC_CALLBACK_1(Inventory::EquipItem, this, id, type, WeaponType::other));
		}
		break;
	case ItemType::armor:
		btnSell->removeFromParent();
		btnUse->removeFromParent();
		GetTab(2)->addChild(btnUse, 99);
		GetTab(2)->addChild(btnSell, 99);
		armors[id]->GetInfo()->removeFromParent();
		armors[id]->GetInfo()->setAnchorPoint(Vec2(0, 1));
		GetTab(2)->addChild(armors[id]->GetInfo(), 99);
		armors[id]->GetInfo()->setPosition(GetSize());
		armors[id]->GetInfo()->setCameraMask(2);
		if (armors[id]->GetIcon() != NULL)
		{
			clickBox->removeFromParent();
			GetTab(2)->addChild(clickBox, 22);

			clickBox->setPosition(armors[id]->GetIcon()->getPosition());
		}
		targetID = id;
		if (targetID >= 0)
		{
			btnSell->addClickEventListener(CC_CALLBACK_1(Inventory::btnSellItem, this, id, type));
			btnUse->addClickEventListener(CC_CALLBACK_1(Inventory::EquipItem, this,id,type, WeaponType::other));
		}
		break;
	case ItemType::boots:
		btnSell->removeFromParent();
		btnUse->removeFromParent();
		GetTab(4)->addChild(btnUse, 99);
		GetTab(4)->addChild(btnSell, 99);
		boots[id]->GetInfo()->removeFromParent();
		boots[id]->GetInfo()->setAnchorPoint(Vec2(0, 1));
		GetTab(4)->addChild(boots[id]->GetInfo(), 99);
		boots[id]->GetInfo()->setPosition(GetSize());
		boots[id]->GetInfo()->setCameraMask(2);
		if (boots[id]->GetIcon() != NULL)
		{
			clickBox->removeFromParent();
			GetTab(4)->addChild(clickBox, 22);

			clickBox->setPosition(boots[id]->GetIcon()->getPosition());
		}
		targetID = id;
		if (targetID >= 0)
		{
			btnSell->addClickEventListener(CC_CALLBACK_1(Inventory::btnSellItem, this, id, type));
			btnUse->addClickEventListener(CC_CALLBACK_1(Inventory::EquipItem, this, id, type, WeaponType::other));
		}
		break;
	default:
		break;
	}
	
}

void Inventory::EquipItem(cocos2d::Ref *pSender,int id, ItemType type,WeaponType wType)
{
	auto mItems = MainCharacter::GetInstance()->GetEquipedItem();
	Item *item;
	switch (type)
	{
	case ItemType::weapon:
		if (targetID >= 0)
		{
			if (wType==WeaponType::sword)
			{
				if (mItems[0]->GetID()==99)
				{
					MainCharacter::GetInstance()->MainCharacter::EquipedItem(0, weapons[targetID]);
					RemoveItem(weapons[targetID]->GetID(), targetID, ItemType::weapon);
				}
				else
				{
					int index = targetID;
					MainCharacter::GetInstance()->MainCharacter::EquipedItem(0, weapons[targetID]);
					RemoveItem(weapons[targetID]->GetID(), targetID, ItemType::weapon);
					item = new Item(mItems[0]);
					AddItem(item->GetID());
				}
			}
			else
			{
				if (mItems[1]->GetID() == 99)
				{
					MainCharacter::GetInstance()->MainCharacter::EquipedItem(1, weapons[targetID]);
					RemoveItem(weapons[targetID]->GetID(), targetID, ItemType::weapon);
				}
				else
				{
					int index = targetID;
					MainCharacter::GetInstance()->MainCharacter::EquipedItem(1, weapons[targetID]);
					RemoveItem(weapons[targetID]->GetID(), targetID, ItemType::weapon);
					item = new Item(mItems[1]);
					AddItem(item->GetID());
				}
			}
		}
		break;
	case ItemType::armor:
		if (targetID >= 0)
		{
			if (mItems[2]->GetID() == 99)
			{
				MainCharacter::GetInstance()->MainCharacter::EquipedItem(2, armors[targetID]);
				RemoveItem(armors[targetID]->GetID(), targetID, ItemType::armor);
			}
			else
			{
				int index = targetID;
				MainCharacter::GetInstance()->MainCharacter::EquipedItem(2, armors[targetID]);
				RemoveItem(armors[targetID]->GetID(), targetID, ItemType::armor);
				item = new Item(mItems[2]);
				AddItem(item->GetID());
			}
		}
		break;
	case ItemType::boots:
		if (targetID >= 0)
		{
			if (mItems[3]->GetID() == 99)
			{
				MainCharacter::GetInstance()->MainCharacter::EquipedItem(3, boots[targetID]);
				RemoveItem(boots[targetID]->GetID(), targetID, ItemType::boots);
			}
			else
			{
				int index = targetID;
				MainCharacter::GetInstance()->MainCharacter::EquipedItem(3, boots[targetID]);
				RemoveItem(boots[targetID]->GetID(), targetID, ItemType::boots);
				item = new Item(mItems[3]);
				AddItem(item->GetID());
			}
		}
		break;
	case ItemType::arrow:
		if (targetID >= 0)
		{
			if (mItems[4]->GetID() == 99)
			{
				MainCharacter::GetInstance()->MainCharacter::EquipedItem(4, arrows[targetID]);
				//RemoveItem(arrows[targetID]->GetID(), targetID, ItemType::arrow);
			}
			else
			{
				int index = targetID;
				MainCharacter::GetInstance()->MainCharacter::EquipedItem(4, arrows[targetID]);
				//RemoveItem(arrows[targetID]->GetID(), targetID, ItemType::arrow);
				item = new Item(mItems[4]);
				//AddItem(item->GetID());
			}
		}
		break;
	default:
		break;
	}
	
}

void Inventory::btnEquipInventory(cocos2d::Ref *pSender)
{
	if (targetID>=0)
	{
		RemoveItem(slots[targetID]->GetID(), targetID,ItemType::potion);
	}
}

void Inventory::btnSellItem(cocos2d::Ref *sender, int id, ItemType type)
{
	SellItem(id, GetIndexByID(id), type);
}
