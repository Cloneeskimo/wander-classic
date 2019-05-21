
#ifndef AttackManager_h
#define AttackManager_h

//Includes
#include <vector>
#include <thread>
#include <chrono>
#include <map>
#include <Item.h>
#include <entity/Player.h>
#include <entity/Enemy.h>

//WeaponSkill Struct
struct WeaponSkill {

	//Contructors
	WeaponSkill() {}; //default
	WeaponSkill(ItemSubType weapon, std::string name, std::string description, std::string effectCode, int level, int cooldown) { //full
		this->weapon = weapon;
		this->name = name;
		this->description = description;
		this->effectCode = effectCode;
		this->level = level;
		this->cooldown = cooldown;
	}

	//Data
	ItemSubType weapon;
	std::string name;
	std::string description;
	std::string effectCode;
	int level;
	int cooldown;
};

//Cooldown Struct
struct Cooldown {

	//Constructors
	Cooldown() {}; //default
	Cooldown(std::string skillName, std::string playerName, int turnsLeft) { //full
		this->skillName = skillName;
		this->playerName = playerName;
		this->turnsLeft = turnsLeft;
	}

	//Data
	std::string skillName;
	std::string playerName;
	int turnsLeft;
};

//IndicatorInfo Struct
struct IndicatorInfo {

	//Constructors
	IndicatorInfo() {}; //default
	IndicatorInfo(bool rightSide, ColorString indicator, bool displayIndicator, ColorString message = ColorString("", ColorFlag::GRAY, 0, 10000, true)) { //full
		this->rightSide = rightSide;
		this->indicator = indicator;
		this->displayIndicator = displayIndicator;
		this->message = message;
	}

	//Data
	bool rightSide = true;
	bool displayIndicator = false;
	ColorString indicator;
	ColorString message;
};

//AttackManager Class
static class AttackManager {
public:

	//Functions
	static void openWeaponSkills(Player* player);
	static void openSpecificWeaponSkills(Player* player, ItemSubType weapon);
	static void updateCooldowns(Player* player);
	static void loadWeaponSkills(std::string storyName);
	static std::string findEffectInString(std::string string, int *start);
	static void applyWeaponSkill(std::vector<IndicatorInfo> * indicators, std::vector<Player*> players, int playerTurn, std::vector<Enemy*> enemies, int playerTarget, int selection);
	static void displayIndicators(IndicatorInfo info);
	static std::vector<std::string> weaponLevelCheck(std::vector<Player*> player);
	static std::vector<std::string> getWeaponSkillsList(Player* player);
	static std::vector<WeaponSkill> getWeaponSkills(Player* player);
	static std::vector<WeaponLevel> getGenericWeaponLevels();
	static int getCooldown(Player* player, std::string skillName);

private:

	//Data
	static std::vector<WeaponSkill> _weaponSkills;
	static std::vector<Cooldown> _cooldowns;
};

#endif