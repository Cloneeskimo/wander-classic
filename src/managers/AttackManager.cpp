#include "AttackManager.h"

//Static Variables
std::vector<WeaponSkill> AttackManager::_weaponSkills = std::vector<WeaponSkill>();
std::vector<Cooldown> AttackManager::_cooldowns = std::vector<Cooldown>();

//Functions
void AttackManager::openWeaponSkills(Player * player) {

	//variables
	std::vector<std::string> weapons;
	int input = 0;

	//acquire weapon levels
	for (int i = 0; i < player->getWeaponLevels().size(); i++) {
		weapons.push_back(convertItemSubType(player->getWeaponLevels()[i].weapon) + ": " + to_string(player->getWeaponLevels()[i].level));
	}

	//menu
	weapons.push_back("[Return]");
	clCons::cls();
	while (input != weapons.size()) {
		input = ColorManager::menu(ColorFlag::GRAY, "[Weapon Skills]", ColorFlag::GRAY, "[Select to see skills]", ColorFlag::GRAY, weapons, '-', '\033', weapons.size());

		if (input != weapons.size()) {
			openSpecificWeaponSkills(player, player->getWeaponLevels()[input - 1].weapon);
		}
	}
	clCons::cls();
}
void AttackManager::openSpecificWeaponSkills(Player * player, ItemSubType weapon) {
	
	//variables
	std::vector<ColorString> leftDisplay, rightDisplay;
	std::vector<WeaponSkill*> skills;
	int input = 0;
	int displayHeight = 0;
	int displayWidth = 0;
	int leftDisplayWidth = 0;
	int rightDisplayWidth = 0;
	int selection = 0;

	//load all skills for specific weapon
	for (int i = 0; i < AttackManager::_weaponSkills.size(); i++) {
		if (AttackManager::_weaponSkills[i].weapon == weapon) {
			skills.push_back(&AttackManager::_weaponSkills[i]);
		}
	}

	//display Loop
	clCons::cls();
	while (input != '\033') {

		//variables
		displayWidth = clCons::getConsoleWidth();
		leftDisplay = rightDisplay = std::vector<ColorString>();

		//create left display
		leftDisplayWidth = 0;
		for (int i = 0; i < skills.size(); i++) {
			std::string pref = "  ", suff = "  ";
			if (i == selection) {
				pref = "-[ ";
				suff = " ]-";
			}
			if (skills[i]->level <= player->getWeaponLevel(weapon)) {
				leftDisplay.push_back(ColorString(pref + skills[i]->name + suff, ColorFlag::GREEN, 0, 10000, false));
			} else {
				leftDisplay.push_back(ColorString(pref + "????" + suff, ColorFlag::RED, 0, 10000, false));
			}
			if (leftDisplay[i].length() > leftDisplayWidth) {
				leftDisplayWidth = leftDisplay[i].length();
			}
		}
		if (displayHeight == 0)
			displayHeight = leftDisplay.size() + 2;

		//create right display	
		rightDisplayWidth = displayWidth - leftDisplayWidth - 3;
		if (skills[selection]->level <= player->getWeaponLevel(weapon)) { //IF SKILL IS UNLOCKED

			//skill name display
			rightDisplay.push_back(ColorString(clCons::centerStringIn(rightDisplayWidth, "-=[ " + skills[selection]->name + " ]=-"), ColorFlag::GREEN, true, 1, clUtil::wordCount(skills[selection]->name)));
			rightDisplay.push_back(ColorString("", ColorFlag::GRAY, 0, 0, false));

			//skill description display
			std::string nextDescLine = "";
			for (int i = 0; i < skills[selection]->description.length(); i++) {
				if (nextDescLine.length() > ((float)rightDisplayWidth * ((float)2 / (float)3)) && skills[selection]->description[i] == ' ') {
					rightDisplay.push_back(ColorString(clCons::centerStringIn(rightDisplayWidth, nextDescLine), ColorFlag::GRAY, 0, 10000, false));
					nextDescLine = "";
				} else {
					nextDescLine += skills[selection]->description[i];
				}
			}
			rightDisplay.push_back(ColorString(clCons::centerStringIn(rightDisplayWidth, nextDescLine), ColorFlag::GRAY, 0, 10000, false));
			rightDisplay.push_back(ColorString("", ColorFlag::GRAY, 0, 0, false));
			rightDisplay.push_back(ColorString("", ColorFlag::GRAY, 0, 0, false));
		} else { //IF SKILL IS NOT UNLOCKED
			rightDisplay.push_back(ColorString(clCons::centerStringIn(rightDisplayWidth, "This skill requires level " + to_string(skills[selection]->level) + " in " + convertItemSubType(skills[selection]->weapon) + "S."), ColorFlag::RED, true, 4, 1, true));
		}
		if (displayHeight - 2 < rightDisplay.size()) {
			displayHeight = rightDisplay.size() + 2;
		}
		for (int i = 0; i < (displayHeight - rightDisplay.size()) / 2; i++) {
			rightDisplay.insert(rightDisplay.begin(), ColorString("", ColorFlag::GRAY, 0, 0, false));
		}

		//top display
		clCons::clsNew();
		clCons::centerVer(displayHeight + 2);
		ColorManager::centerHorColor(ColorString("--==[ " + convertItemSubType(weapon) + "S (Lv. " + to_string(player->getWeaponLevel(weapon)) + ") ]==--", ColorFlag::BLUE, true, 1, 3, true), ' ', 2);

		//middle display
		for (int i = 0; i < displayHeight; i++) {
			if (i == 0 || i == displayHeight - 1) { //TOP LINE & BOTTOM LINE
				for (int j = 0; j < displayWidth; j++) {
					if (j == 0 || j == displayWidth - 1 || j == leftDisplayWidth + 1) {
						std::cout << "+";
					} else {
						std::cout << "-";
					}
				}
			} else { //CONTENT LINES
				//LEFT DISPLAY
				std::cout << "|";
				if (i - 1 < leftDisplay.size()) {
					ColorManager::csout(leftDisplay[i - 1]);
					for (int j = 0; j < (leftDisplayWidth - leftDisplay[i - 1].length()); j++) {
						std::cout << " ";
					}
				} else {
					for (int j = 0; j < leftDisplayWidth; j++) {
						std::cout << " ";
					}
				}
				std::cout << "|";

				//RIGHT DISPLAY
				if (i - 1 < rightDisplay.size()) {
					ColorManager::csout(rightDisplay[i - 1]);
					for (int j = 0; j < (rightDisplayWidth - rightDisplay[i - 1].length()); j++) {
						std::cout << " ";
					}
				} else {
					for (int j = 0; j < rightDisplayWidth; j++) {
						std::cout << " ";
					}
				}
				std::cout << "|";
			}
		}

		//input
		input = _getch();
		switch (input) {
			case 'w':
			case 'W':
				selection--;
				if (selection < 0)
					selection = skills.size() - 1;
				break;
			case 's':
			case 'S':
				selection++;
				if (selection > skills.size() - 1)
					selection = 0;
				break;
		};
	}
	clCons::cls();
}
void AttackManager::updateCooldowns(Player * player) {
	for (int i = 0; i < AttackManager::_cooldowns.size(); i++) {
		if (AttackManager::_cooldowns[i].playerName == player->getName()) {
			if (AttackManager::_cooldowns[i].turnsLeft == 0)
				AttackManager::_cooldowns.erase(AttackManager::_cooldowns.begin() + i);
			else
				AttackManager::_cooldowns[i].turnsLeft--;
		}
	}
}
void AttackManager::loadWeaponSkills(std::string storyName) {
	
	//variables
	ifstream read;
	AttackManager::_weaponSkills = std::vector<WeaponSkill>();
	WeaponSkill nextWeaponSkill = WeaponSkill();
	std::string nextLine = "";

	//open ile
	read.open("data//stories//" + storyName + "//weaponskills.wtxt");
	if (read.fail()) {
		OptionsManager::wError("Unable to open 'weaponskills.wtxt'", "ATTACKMANAGER_H", true);
	}

	//parse lines
	while (!read.eof()) {
		getline(read, nextLine);

		if (nextLine == "\t{") { //NEW SKILL
			nextWeaponSkill = WeaponSkill();
		} else if (nextLine == "\t}") { //FINISHED WITH SKILL
			AttackManager::_weaponSkills.push_back(nextWeaponSkill);
		} else if (nextLine.find("weapon:") != std::string::npos) { //WEAPON
			nextWeaponSkill.weapon = convertItemSubString(clFile::cut(nextLine));
		} else if (nextLine.find("name:") != std::string::npos) { //NAME
			nextWeaponSkill.name = clFile::cut(nextLine);
		} else if (nextLine.find("description:") != std::string::npos) { //DESCRIPTION
			nextWeaponSkill.description = clFile::cut(nextLine);
		} else if (nextLine.find("effect code:") != std::string::npos) { //EFFECT CODE
			nextWeaponSkill.effectCode = clFile::cut(nextLine);
		} else if (nextLine.find("level:") != std::string::npos) { //LEVEL
			nextWeaponSkill.level = stoi(clFile::cut(nextLine));
		} else if (nextLine.find("cooldown:") != std::string::npos) { //COOLDOWN
			nextWeaponSkill.cooldown = stoi(clFile::cut(nextLine));
		}
	}
}
std::string AttackManager::findEffectInString(std::string string, int *start) {
	std::string effect;
	bool done = false;
	while (!done) {
		if (!(*start > string.length())) {
			if (string[*start] != ' ') {
				effect += string[*start];
				(*start)++;
			} else
				done = true;
		} else
			done = true;
	}
	return effect;
}
void AttackManager::applyWeaponSkill(std::vector<IndicatorInfo>* indicators, std::vector<Player*> players, int playerTurn, std::vector<Enemy*> enemies, int playerTarget, int selection) {
	
	//collect effect
	std::vector<WeaponSkill> skills = AttackManager::getWeaponSkills(players[playerTurn]);
	std::string effect = skills[selection].effectCode;

	//act on effect
	for (int i = 0; i < effect.length(); i++) {
		if (effect.substr(i, 6) == "damage") { //DAMAGE

			//find damage
			i += 7;
			std::string damage = findEffectInString(effect, &i);

			//determine multiple
			bool multiple = false;
			for (int i = 0; i < damage.length(); i++) {
				if (damage[i] == 'x') {
					multiple = true;
					damage.erase(damage.begin() + i);
				}
			}

			//determine damage
			int dmg = 0;
			if (multiple) {
				dmg = players[playerTurn]->getTotalDamage() * stof(damage);
			} else {
				dmg = stof(damage);
			}

			//deal damage
			enemies[playerTarget]->takeDamage(dmg);

			//indicator
			indicators->push_back(IndicatorInfo(true, ColorString("-" + to_string(dmg), ColorFlag::RED, 0, 10000, true), true, ColorString(players[playerTurn]->getName() + " attacks " + enemies[playerTarget]->getName() + " for " + to_string(dmg) + " damage!", ColorFlag::RED, 0, 10000, true)));

		} else if (effect.substr(i, 4) == "stun") { //STUN

			//find stun
			i += 5;
			std::string stun = findEffectInString(effect, &i);

			//deal stun
			enemies[playerTarget]->stun(stoi(stun));

			//indicator
			indicators->push_back(IndicatorInfo(true, ColorString("-" + stun + "turns", ColorFlag::GRAY, 0, 10000, true), true, ColorString(players[playerTurn]->getName() + " stuns " + enemies[playerTarget]->getName() + " for " + stun + " turns!", ColorFlag::RED, 0, 10000, true)));

		} else if (effect.substr(i, 3) == "aoe") { //AOE

			//find AOE
			i += 4;
			std::string aoe = findEffectInString(effect, &i);

			//determine multiple
			bool multiple = false;
			for (int i = 0; i < aoe.length(); i++) {
				if (aoe[i] == 'x') {
					multiple = true;
					aoe.erase(aoe.begin() + i);
				}
			}

			//determine damage
			int dmg = 0;
			if (multiple) {
				dmg = players[playerTurn]->getTotalDamage() * stof(aoe);
			} else {
				dmg = stof(aoe);
			}

			//deal damage
			for (int i = 0; i < enemies.size(); i++) {
				enemies[i]->takeDamage(dmg);
			}

			//indicator
			indicators->push_back(IndicatorInfo(true, ColorString("- " + to_string((dmg)* enemies.size()), ColorFlag::RED, 0, 10000, true), true, ColorString(players[playerTurn]->getName() + " attacks all enemies for " + to_string(dmg) + " damage each!", ColorFlag::RED, 0, 10000, true)));

		} else if (effect.substr(i, 3) == "dot") { //DOT

			//find DOT
			i += 4;
			std::string dot = findEffectInString(effect, &i);

			//determine multiple
			bool multiple = false;
			for (int i = 0; i < dot.length(); i++) {
				if (dot[i] == 'x') {
					multiple = true;
					dot.erase(dot.begin() + i);
					i--;
				}
			}

			//determine damage
			int dmg = 0;
			if (multiple) {
				dmg = (float)players[playerTurn]->getTotalDamage() * stof(dot) + 0.5;
			} else {
				dmg = stof(dot);
			}

			//deal DOT
			enemies[playerTarget]->addDot(Dot(5, dmg));

			//indicator
			indicators->push_back(IndicatorInfo(true, ColorString("- " + to_string((int)(dmg * 5)), ColorFlag::GRAY, 0, 10000, true), true, ColorString(players[playerTurn]->getName() + " inflicts " + enemies[playerTarget]->getName() + " with " + to_string(dmg) + " damage for 5 turns!", ColorFlag::RED, 0, 10000, true)));

		} else if (effect.substr(i, 4) == "heal") { //SINGLE HEAL

			//find Heal
			i += 5;
			std::string heal = findEffectInString(effect, &i);

			//determine multiple
			bool multiple = false;
			for (int i = 0; i < heal.length(); i++) {
				if (heal[i] == 'x') {
					multiple = true;
					heal.erase(heal.begin() + i);
					i--;
				}
			}

			//determine heal value
			int healValue = 0;
			if (multiple) {
				healValue = (float)players[playerTurn]->getMaxHealth() * stof(heal) + 0.5;
			} else {
				healValue = stof(heal);
			}

			//deal heal
			players[playerTurn]->heal(healValue);

			//indicator
			indicators->push_back(IndicatorInfo(false, ColorString("+ " + to_string((int)(healValue)), ColorFlag::GREEN, 0, 10000, true), true, ColorString(players[playerTurn]->getName() + " heals themself for " + to_string(healValue) + " HP!", ColorFlag::GREEN, 0, 10000, true)));

		} else if (effect.substr(i, 5) == "group") { //GROUP HEAL

			//find heal
			i += 6;
			std::string heal = findEffectInString(effect, &i);

			//determine multiple
			bool multiple = false;
			for (int i = 0; i < heal.length(); i++) {
				if (heal[i] == 'x') {
					multiple = true;
					heal.erase(heal.begin() + i);
					i--;
				}
			}

			//heal each player loop
			for (int i = 0; i < players.size(); i++) {

				//determine heal value
				int healValue = 0;
				if (multiple) {
					healValue = (float)players[playerTurn]->getMaxHealth() * stof(heal) + 0.5;
				} else {
					healValue = stof(heal);
				}

				//deal heal	
				players[i]->heal(healValue);

				//indicator
				indicators->push_back(IndicatorInfo(false, ColorString("+ " + to_string((int)(healValue)), ColorFlag::GREEN, 0, 10000, true), true, ColorString(players[playerTurn]->getName() + " heals " + players[i]->getName() + " for " + to_string(healValue) + " HP!", ColorFlag::GREEN, 0, 10000, true)));
			}
		}
	}

	//apply cooldown
	AttackManager::_cooldowns.push_back(Cooldown(skills[selection].name, players[playerTurn]->getName(), skills[selection].cooldown));
}
void AttackManager::displayIndicators(IndicatorInfo info) {

	//pre-display
	clCons::clsNew();
	for (int i = 0; i < (clCons::getConsoleHeight() / 2 - 1); i++) {
		std::cout << std::endl;
	}

	//display message
	ColorManager::centerHorColor(info.message);
	for (int i = 0; i < clCons::getConsoleWidth() / 2 - (info.message.length() / 2); i++) {
		std::cout << " ";
	}

	//display indicator
	for (int i = 0; i < 6; i++) {
		clCons::clsNew();
		int heightToStart = clCons::getConsoleHeight() / 2;
		int widthToStart = clCons::getConsoleWidth() / 3;
		for (int i = 0; i < heightToStart; i++) {
			std::cout << std::endl;
		}
		for (int j = 0; j < i; j++)
			for (int k = 0; k < clCons::getConsoleWidth(); k++) {
				std::cout << " ";
			}
		if (info.rightSide)
			widthToStart *= 2;
		for (int j = 0; j < widthToStart; j++) {
			std::cout << " ";
		}
		ColorManager::csout(info.indicator);
		std::cout << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(150));
	}
}
std::vector<std::string> AttackManager::weaponLevelCheck(std::vector<Player*> players) {
	bool hasCleared = false;
	std::vector<std::string> toReturn;
	for (int i = 0; i < players.size(); i++) {
		Item* playerWeapon = players[i]->getWeapon();
		float chance = 100 / players[i]->getWeaponLevel(playerWeapon->getSubType());
		chance *= 3;
		if (chance < 5) { chance = 5; }

		if (CalcManager::getRandomNumber(100) <= chance) {
			players[i]->addWeaponLevel(playerWeapon->getSubType());
			toReturn.push_back(players[i]->getName() + "'s level in " + clUtil::lowercase(convertItemSubType(playerWeapon->getSubType())) + "s has increased to " + to_string(players[i]->getWeaponLevel(playerWeapon->getSubType())));
		}
	}

	return toReturn;
}
std::vector<std::string> AttackManager::getWeaponSkillsList(Player * player) {
	
	//variables
	std::vector<std::string> weaponSkills;
	std::string nextSkill = "";

	//add all related to equipped weapon
	for (int i = 0; i < AttackManager::_weaponSkills.size(); i++) {
		if (AttackManager::_weaponSkills[i].weapon == player->getWeapon()->getSubType()) {
			if (AttackManager::_weaponSkills[i].level <= player->getWeaponLevel(AttackManager::_weaponSkills[i].weapon)) {
				nextSkill = AttackManager::_weaponSkills[i].name;
				nextSkill += " [" + to_string(AttackManager::getCooldown(player, AttackManager::_weaponSkills[i].name)) + "]";
				weaponSkills.push_back(nextSkill);
			}
		}
	}

	//return
	return weaponSkills;
}
std::vector<WeaponSkill> AttackManager::getWeaponSkills(Player* player) {
	
	//variables
	std::vector<WeaponSkill> weaponSkills;
	std::string nextSkill = "";

	//add all related to equipped weapon
	for (int i = 0; i < AttackManager::_weaponSkills.size(); i++) {
		if (AttackManager::_weaponSkills[i].weapon == player->getWeapon()->getSubType()) {
			weaponSkills.push_back(AttackManager::_weaponSkills[i]);
		}
	}

	//return
	return weaponSkills;
}
std::vector<WeaponLevel> AttackManager::getGenericWeaponLevels() {
	std::vector<WeaponLevel> toReturn;
	toReturn.push_back(WeaponLevel(SWORD, 1));
	toReturn.push_back(WeaponLevel(BOW, 1));
	toReturn.push_back(WeaponLevel(DAGGER, 1));
	toReturn.push_back(WeaponLevel(STAFF, 1));
	toReturn.push_back(WeaponLevel(WAND, 1));
	toReturn.push_back(WeaponLevel(HAMMER, 1));
	return toReturn;
}
int AttackManager::getCooldown(Player * player, std::string skillName) {
	int cooldown = 0;
	for (int i = 0; i < AttackManager::_cooldowns.size(); i++) {
		if (AttackManager::_cooldowns[i].skillName == skillName) {
			cooldown = AttackManager::_cooldowns[i].turnsLeft;
		}
	}

	return cooldown;
}

//Weapon Skills Code Syntax:
// 'damage [int thisMuch]' will damage the enemy for [thisMuch] damage. can be followed by an 'x' to become a relative
// 'heal [int thisMuch]' will heal the player for [thisMuch] health. can be followed by a '%' to become a percent of full health