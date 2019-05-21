#pragma once
#include <vector>
#include <map>
#include "entity.h"
#include "item.h"
#include "storyManager.h"
#include "colorManager.h"
#include "calcManager.h"

const static int MAX_STACK_SIZE = 99;

class Player : public Entity
{
public:
	//Constructor
	using Entity::Entity;
	void constructPlayerExtras(int level, int gold, int health, int exp, std::vector<Level> levels, std::vector<Item> inventory, std::vector<Item> equipment);
	void constructPlayerAttributes(int attributePoints, int endurance, int spirit, int capability);

	//Getters
	Item getInventoryItem(int atIndex) { return this->_inventory[atIndex]; }
	Item* getWeapon();
	std::vector<Item>* getInventory() { return &this->_inventory; }
	std::vector<Item>* getEquipment() { return &this->_equipment; }
	std::vector<WeaponLevel> getWeaponLevels() { return this->_weaponLevels; }
	int getLevel() { return this->_level; }
	int getGold() { return this->_gold; }
	int getHealth() { return this->_health; }
	int getExp() { return this->_exp; }
	int getAttributePoints() { return this->_attributePoints; }
	int getEndurance() { return this->_endurance; }
	int getSpirit() { return this->_spirit; }
	int getCapability() { return this->_capability; }
	int getWeaponLevel(ItemSubType thisWeapon);
	int getMaxHealth();
	float getWeaponProficiency();

	//Totaling Functions
	int getTotalItems();
	int getTotalArmor();
	int getRegeneration();
	float getTotalDamage(bool withCapability = true, bool withProficiency = true);
	int getInventoryValue();

	//Setters
	void addItemToInventory(Item thisItem);
	void setWeaponLevels(std::vector<WeaponLevel> weaponLevels) { this->_weaponLevels = weaponLevels; }
	void addWeaponLevel(ItemSubType thisType);
	void addAttribute(int choice); //0 - Endurance. 1 - Spirit. 2 - Capability.
	void heal(int thisMuch = 0);

	//Functions
	void passiveUpdate();
	void openInventory();
	void openCharacter();
	void openAttributes();
	void promptRemove(int atIndex);
	void removeFromInventory(int atIndex, int quantity = 1);
	void addXp(int thisMuch);
	void equip(Item equipThis, int index = -1); 
	void unequip(ItemSubType thisPiece);
	void updateHealth();
	bool getPiece(ItemSubType thisType, Item* thisItem);
	Item getPiece(ItemSubType thisType, bool* equipped);
	Item getWeapon(bool* equipped);
	bool getWeapon(Item* thisItem);
	bool takeDamage(int thisMuch);
	ColorString createHealthBar(int width);
	float getEffectiveArmor() { return (float)this->getTotalArmor() / 3; }
	float getEffectiveEndurance() { return ((float)(this->_endurance * 2) + 100) / 100; }

private:

	//Variables
	std::vector<Item> _equipment;
	std::vector<Item> _inventory;
	std::vector<Level> _levels;
	std::vector<WeaponLevel> _weaponLevels;
	int _gold;
	int _level;
	int _health;
	int _exp;

	//Attributes
	int _attributePoints = 0;
	int _endurance = 0;
	int _spirit = 0;
	int _capability = 0;
};