#include "ItemDatabase.h"
#include "cocos2d.h"
USING_NS_CC;
ItemDatabase::ItemDatabase()
{
}

ItemDatabase::~ItemDatabase()
{
}

void ItemDatabase::Init()
{
	std::string data, temp;
	int num;
	int id;
	data = FileUtils::getInstance()->getStringFromFile("res/sprites/item/itemData.bin");
	std::istringstream iss(data);
	// Armor
	
	iss >> num;
	for (int i = 1; i <= num; i++)
	{
		auto sprite = Sprite::create("res/sprites/item/itemDetail/itemInfo (" + std::to_string(i) + ").png");
		iss >> id;
		iss >> temp;
		temp.replace(temp.find("__"), 2, " ");
		items.push_back(new Item(
			temp, id, "Armor", 20, 50, sprite, ItemType::armor)
		);
	}
	iss >> num;
	for (int i = 1; i <= num; i++)
	{
		auto sprite = Sprite::create("res/sprites/item/itemDetail/itemInfo (" + std::to_string(5+i) + ").png");
		iss >> id;
		iss >> temp;
		temp.replace(temp.find("__"), 2, " ");
		items.push_back(new Item(
			temp, id, "Arrow", 10, 2, sprite, ItemType::arrow, WeaponType::arrow)
		);
	}
	iss >> num;
	for (int i = 1; i <= num; i++)
	{
		auto sprite = Sprite::create("res/sprites/item/itemDetail/itemInfo (" + std::to_string(i+12) + ").png");
		iss >> id;
		iss >> temp;
		temp.replace(temp.find("__"), 2, " ");
		items.push_back(new Item(
			temp, id, "Boots", 5, 30, sprite, ItemType::boots)
		);
	}
	iss >> num;
	for (int i = 1; i <= num; i++)
	{
		auto sprite = Sprite::create("res/sprites/item/itemDetail/itemInfo (" + std::to_string(i+16) + ").png");
		iss >> id;
		iss >> temp;
		temp.replace(temp.find("__"), 2, " ");
		items.push_back(new Item(
			temp, id, "Bow", 10, 100, sprite, ItemType::weapon, WeaponType::bow)
		);
	}
	iss >> num;
	for (int i = 1; i <= num; i++)
	{
		auto sprite = Sprite::create("res/sprites/item/itemDetail/itemInfo (" + std::to_string(i+20) + ").png");
		iss >> id;
		iss >> temp;
		temp.replace(temp.find("__"), 2, " ");
		items.push_back(new Item(
			temp, id, "Potion", 50, 10, sprite, ItemType::potion)
		);
	}
	iss >> num;
	for (int i = 1; i <= num; i++)
	{
		auto sprite = Sprite::create("res/sprites/item/itemDetail/itemInfo (" + std::to_string(i+25) + ").png");
		iss >> id;
		iss >> temp;
		temp.replace(temp.find("__"), 2, " ");
		items.push_back(new Item(
			temp, id, "Sword", 10, 1000,sprite, ItemType::weapon, WeaponType::sword)
		);
	}
	
	items.push_back(new Item("sprites/item/box"));
}

int ItemDatabase::GetIndexByID(int id)
{
	for (int i = 0; i < items.size(); i++)
	{
		if (id==items[i]->GetID())
		{
			return i;
		}
	}
	return -1;
}
