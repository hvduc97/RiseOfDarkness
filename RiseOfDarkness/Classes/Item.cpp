#include "Item.h"

ItemType Item::GetType()
{
	return itemType;
}

WeaponType Item::GetWeaponType()
{
	return weaponType;
}

void Item::CreateSprite(int id)
{
}

void Item::AddToScene(cocos2d::ui::Layout *layout, cocos2d::Vec2 pos)
{
}

void Item::RemoveFromScene()
{
	
}

int Item::GetSellCost()
{
	return sellCost;
}

Item::Item()
{
	itemID = NULL;
	itemStackable = false;
	itemIcon = nullptr;
	weaponType = WeaponType::other;
}

Item::~Item()
{
}

Item::Item(std::string name, int id, std::string desc, int power, int Cost, cocos2d::Sprite* info, ItemType type)
{
	itemID = id;
	itemName = name;
	itemDesc = desc;
	itemPower = power;
	sellCost = Cost;
	itemType = type;
	itemStackable = ((type == ItemType::potion || type== ItemType::arrow )? true : false);
	weaponType = WeaponType::other;
	itemIcon = (cocos2d::ui::Button*)cocos2d::ui::Button::create("res/" + name + ".png")->Button::clone();
	itemIcon->retain();
	this->info = info;
	this->info->retain();
}

Item::Item(std::string name, int id, std::string desc, int power, int Cost, cocos2d::Sprite* info, ItemType type, WeaponType wType)
{
	itemID = id;
	itemName = name;
	itemDesc = desc;
	itemPower = power;
	sellCost = Cost;
	itemType = type;
	this->info = info;
	this->info->retain();
	itemStackable = ((type == ItemType::potion || type==ItemType::arrow) ? true : false);
	weaponType = wType;
	itemIcon = (cocos2d::ui::Button*)cocos2d::ui::Button::create("res/" + name + ".png")->Button::clone();
	itemIcon->retain();
}

Item::Item(std::string name)
{
	itemID = 99;
	itemStackable = false;
	itemIcon = (cocos2d::ui::Button*)cocos2d::ui::Button::create("res/" + name + ".png")->clone();
	itemIcon->retain();
}

cocos2d::ui::Button * Item::GetIcon()
{
	return itemIcon;
}

int Item::GetID()
{
	return itemID;
}

cocos2d::Sprite * Item::GetInfo()
{
	return info;
}

bool Item::IsStackable()
{
	return itemStackable;
}

Item::Item(const Item *item)
{
	itemID = item->itemID;
	itemName = item->itemName;
	itemDesc = item->itemDesc;
	itemPower = item->itemPower;
	sellCost = item->sellCost;
	itemType = item->itemType;
	itemStackable = item->itemStackable;
	itemIcon = (cocos2d::ui::Button*)item->itemIcon->clone();
	itemIcon->retain();
	info = item->info;
	weaponType = item->weaponType;
}

std::string Item::GetDesc()
{
	return itemDesc;
}
