#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <cstdlib> 
#include <ctime> 
#include "item.h"
#include "clfram.h"

struct EquipmentRandomInfo
{
	std::string name;
	int level;
	std::vector<std::string> positiveAdjs;
	std::vector<std::string> negativeAdjs;
};

static class CalcManager
{
public:
	//Variables
	static std::vector<EquipmentRandomInfo> armorRandomInfo;
	static std::vector<EquipmentRandomInfo> weaponRandomInfo;

	//Random Info Functions
	static void loadRandomInfo(std::string storyName);
	static EquipmentRandomInfo getArmorRandomInfo(int level);
	static EquipmentRandomInfo getWeaponRandomInfo(int level);

	//Variable Calculations
	static float getEffectiveArmor(float armor) { return armor / 3; }
	static float getEffectiveEndurance(float endurance) { return (endurance + 100) / 100; }
	static int getRandomNumber(int limit = 100) { return (rand() % limit) + 1; }

	//Constant Calculations
	static int getBaseHealth(int level);
	static int getBaseArmorValue(int level);
	static int getPieceArmorPoints(ItemSubType thisType);

	//Average Calculations
	static int getAverageArmorForPiece(int level, ItemSubType thisType);
	static int getAverageArmor(int level);
	static int getAverageHealth(int level);
	static int getAverageDamage(int level);
	static int getAverageEnemyHealth(int level);
	static int getAverageEnemyDamage(int level);
	static int getEnemyHealth(int level, float eliteness = 1.0f);
	static int getEnemyDamage(int level, float eliteness = 1.0f);
	static int getExp(int level, float eliteness = 1.0f);

	//Random Item Generation Functions
	static Item createRandomArmor(ArmorInfo info, float fortune = 1); //FORTUNE OF 2 -> DOUBLE STATS // FORTUNE OF 1/2 -> HALF STATS
	static Item createRandomWeapon(WeaponInfo info, float fortune = 1); //FORTUNE OF 2 -> DOUBLE STATS // FORTUNE OF 1/2 -> HALF STATS
	static std::vector<Item> createItemsFromStrings(std::vector<std::string> strings, int referenceLevel);
	static std::vector<Drop> createDropsFromStrings(std::vector<std::string> strings, int referenceLevel, float eliteness = 1.0f);
	static std::vector<Item> convertDropsToItems(std::vector<Drop> drops);

	//CALCULATION MAP
	// Player Health = (Base Health) + (Effective Armor * Effective Endurance)
	// Base Health = (Level * Level) - (Level) + (10)
	// Effective Armor = (Armor) / (3)
	// Effective Endurance = (Endurance + 100) / 100
	
	//STATS MAP
	// Endurance (how much health you get from armor)
	//    - starts at 0, can get from leveling & armor
	//
	// Vitality (how fast you regenerate health)
	//    - starts at 0, can get from leveling & armor
	//
	// Charisma (how cheap you can acquire items)
	//    - starts at 0, can get from leveling & armor
	//
	// Agility (how many tiles you can move per turn)
	//    - starts at 1, can only get from armor

	//ARMOR MAP
	// Helmet - 2 Base Points
	// Shoulderpads - 2 Base Points
	// Necklace - 1 Base Point
	// Chestpiece - 4 Base Points
	// Bracers - 2 Base Points
	// Gloves - 2 Base Points
	// Belt - 1 Base Point
	// Leggings - 3 Base Points
	// Boots - 2 Base Points

private:
};