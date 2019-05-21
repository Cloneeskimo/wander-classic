#pragma once
#include <vector>
#include <thread>
#include <chrono>
#include <map>
#include "item.h"
#include "player.h"
#include "enemy.h"

//Structs
struct WeaponSkill
{
	//Contructor
	WeaponSkill() {};
	WeaponSkill(ItemSubType weapon, std::string name, std::string description, std::string effectCode, int level, int cooldown)
	{
		this->weapon = weapon;
		this->name = name;
		this->description = description;
		this->effectCode = effectCode;
		this->level = level;
		this->cooldown = cooldown;
	}

	//Variables
	ItemSubType weapon;
	std::string name;
	std::string description;
	std::string effectCode;
	int level;
	int cooldown;
};
struct Cooldown
{
	//Constructor
	Cooldown() {};
	Cooldown(std::string skillName, std::string playerName, int turnsLeft)
	{
		this->skillName = skillName;
		this->playerName = playerName;
		this->turnsLeft = turnsLeft;
	}

	//Variables
	std::string skillName;
	std::string playerName;
	int turnsLeft;
};
struct IndicatorInfo
{
	//Constructor
	IndicatorInfo() {};
	IndicatorInfo(bool rightSide, ColorString indicator, bool displayIndicator, ColorString message = ColorString("", ColorFlag::GRAY, 0, 10000, true))
	{
		this->rightSide = rightSide;
		this->indicator = indicator;
		this->displayIndicator = displayIndicator;
		this->message = message;
	}

	//Variables
	bool rightSide = true;
	bool displayIndicator = false;
	ColorString indicator;
	ColorString message;
};

static class AttackManager
{
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

	//Variables
	static std::vector<WeaponSkill> _weaponSkills;
	static std::vector<Cooldown> _cooldowns;
};