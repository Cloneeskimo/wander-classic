#include "calcManager.h"

//Variables
std::vector<EquipmentRandomInfo> CalcManager::armorRandomInfo;
std::vector<EquipmentRandomInfo> CalcManager::weaponRandomInfo;

//Random Info Functions
void CalcManager::loadRandomInfo(std::string storyName)
{
	//Variables
	std::string nextLine;
	std::string material;
	std::vector<string> posAdjs, negAdjs;
	int level;
	int componentsRead = 0;
	bool startReading = false;
	armorRandomInfo = std::vector<EquipmentRandomInfo>();

	//Open Armor File
	std::ifstream read;
	read.open("data//stories//" + storyName + "//random//armor.wtxt");
	if (read.fail()) {
		OptionsManager::wError("Unable to open armor.wtxt", "RANDOMMANAGER_H");
	}

	
	//Read Armor File
	getline(read, nextLine);
	while (!read.eof()) {
		if (nextLine.find("name:") != std::string::npos) {
			material = clFile::cut(nextLine);
			componentsRead++;
		} else if (nextLine.find("level:") != std::string::npos) {
			level = stoi(clFile::cut(nextLine));
			componentsRead++;
		} else if (nextLine.find("positive adjectives:") != std::string::npos) {
			posAdjs = clUtil::explode(clFile::cut(nextLine));
			componentsRead++;
		} else if (nextLine.find("negative adjectives:") != std::string::npos) {
			negAdjs = clUtil::explode(clFile::cut(nextLine));
			componentsRead++;
		}
		if (componentsRead == 4) {
			armorRandomInfo.push_back(EquipmentRandomInfo{ material, level, posAdjs, negAdjs });
			componentsRead = 0;
		}
		getline(read, nextLine);
	}

	//Open Weapon File
	read.close();
	read.open("data//stories//" + storyName + "//random//weapons.wtxt");
	if (read.fail()) {
		OptionsManager::wError("Unable to open weapons.wtxt", "RANDOMMANAGER_H");
	}

	
	//Read Weapon File
	getline(read, nextLine);
	while (!read.eof()) {
		if (nextLine.find("name:") != std::string::npos) {
			material = clFile::cut(nextLine);
			componentsRead++;
		} else if (nextLine.find("level:") != std::string::npos) {
			level = stoi(clFile::cut(nextLine));
			componentsRead++;
		} else if (nextLine.find("positive adjectives:") != std::string::npos) {
			posAdjs = clUtil::explode(clFile::cut(nextLine));
			componentsRead++;
		} else if (nextLine.find("negative adjectives:") != std::string::npos) {
			negAdjs = clUtil::explode(clFile::cut(nextLine));
			componentsRead++;
		}
		if (componentsRead == 4) {
			weaponRandomInfo.push_back(EquipmentRandomInfo{ material, level, posAdjs, negAdjs });
			componentsRead = 0;
		}
		getline(read, nextLine);
	}

	//Close File
	read.close();

	//Initiate Random Generation
	srand((unsigned)time(NULL));
}
EquipmentRandomInfo CalcManager::getArmorRandomInfo(int level)
{
	int atIndex = 0;
	for (int i = 0; i < CalcManager::armorRandomInfo.size(); i++) {
		if (!(CalcManager::armorRandomInfo[i].level > level)) {
			atIndex = i;
		}
	}
	return CalcManager::armorRandomInfo[atIndex];
}
EquipmentRandomInfo CalcManager::getWeaponRandomInfo(int level)
{
	int atIndex = 0;
	for (int i = 0; i < CalcManager::weaponRandomInfo.size(); i++) {
		if (!(CalcManager::weaponRandomInfo[i].level > level)) {
			atIndex = i;
		}
	}
	return CalcManager::weaponRandomInfo[atIndex];
}

//Constant Calculations
int CalcManager::getPieceArmorPoints(ItemSubType thisType)
{
	switch (thisType) {
	case NECKLACE:
	case BELT:
		return 1;
	case HELMET:
	case SHOULDERPADS:
	case BRACERS:
	case GLOVES:
	case BOOTS:
		return 2;
	case LEGGINGS:
		return 3;
	case CHESTPIECE:
		return 4;
	case SWORD:
	case BOW:
	case NOSUBTYPE:
		return 0;
	}
}
int CalcManager::getBaseArmorValue(int level)
{
	return (((level * level) - level) / 4) + 1;
}

//Average Calculations
int CalcManager::getBaseHealth(int level)
{
	return (((level * level) - level) / 2) + 10;
}
int CalcManager::getAverageArmorForPiece(int level, ItemSubType thisType)
{
	return getBaseArmorValue(level) * getPieceArmorPoints(thisType);
}
int CalcManager::getAverageArmor(int level)
{
	int averageArmor = 0;
	for (int i = 1; i <= SUBTYPEWEAPONAFTER; i++) {
		averageArmor += getAverageArmorForPiece(level, ItemSubType(i));
	}
	return averageArmor;
}
int CalcManager::getAverageHealth(int level)
{
	float effectiveEndurance = ((((float)2 / (float)3) * (float)level) + 100) / 100;
	return getEffectiveArmor((float)getAverageArmor(level) * effectiveEndurance) + (float)getBaseHealth(level);
}
int CalcManager::getAverageDamage(int level)
{
	return getAverageHealth(level) / 4;
}
int CalcManager::getAverageEnemyHealth(int level)
{
	//Factor in difficulty
	float multiplier = 0.8;
	if (OptionsManager::difficulty == 0) {
		multiplier = 0.6;
	} else if (OptionsManager::difficulty == 2) {
		multiplier = 1.0;
	}

	//Return HP
	return (float)getAverageHealth(level) * multiplier;
}
int CalcManager::getAverageEnemyDamage(int level)
{
	//Factor in difficulty
	float multiplier = 1;
	if (OptionsManager::difficulty == 0) {
		multiplier = 0.8;
	} else if (OptionsManager::difficulty == 2) {
		multiplier = 1.2;
	}

	//Return Damage
	int damage = ((float)getAverageHealth(level) / 9) * multiplier;
	if (damage < 1) {
		damage = 1;
	}
	return damage;
}
int CalcManager::getEnemyHealth(int level, float eliteness)
{
	//Raw Health
	float health = getAverageEnemyHealth(level);

	//Factor in Random
	health *= ((float)(rand() % 40 + 81) / 100);

	//Factor in Eliteness
	health = (float)health * eliteness;

	//Return Health
	return health;
}
int CalcManager::getEnemyDamage(int level, float eliteness)
{
	//Raw Health
	float damage = getAverageEnemyDamage(level);

	//Factor in Random
	damage *= ((float)(rand() % 40 + 81) / 100);

	//Factor in Eliteness
	damage = (float)damage * eliteness;

	//Return Health
	return damage;
}
int CalcManager::getExp(int level, float eliteness)
{
	float exp = (float)level / 5;
	exp *= (eliteness * eliteness);
	int expInt = (int)(exp + 0.5);
	if (expInt < 1) { expInt = 1; }
	return expInt;
}

//Random Item Generation Functions
Item CalcManager::createRandomArmor(ArmorInfo info, float fortune)
{
	//Create Item
	Item created;

	//Determine Piece
	if (info.piece == ItemSubType::NOSUBTYPE) {
		info.piece = ItemSubType((rand() % (SUBTYPEWEAPONAFTER)) + 1);
	}

	//Determine Effect
	if (info.effect == -1) {
		float FortuneScale = 80 * (fortune - 1);
		float luck = 1 + ((rand() % 80 - (40 - FortuneScale)) / 100) * fortune;
		if (luck < 0.01) { luck = 0.01; }

		info.effect = luck * (float)CalcManager::getAverageArmorForPiece(info.level, info.piece);

	}

	//Determine Name
	if (info.name == "RANDOM") {
		info.name = "";

		EquipmentRandomInfo namingInfo = CalcManager::getArmorRandomInfo(info.level);
		if (info.effect > CalcManager::getAverageArmorForPiece(info.level, info.piece)) {
			info.name += namingInfo.positiveAdjs[(rand() + 1) % namingInfo.positiveAdjs.size()] + " ";
		} else if (info.effect < CalcManager::getAverageArmorForPiece(info.level, info.piece)) {
			info.name += namingInfo.negativeAdjs[(rand() + 1) % namingInfo.negativeAdjs.size()] + " ";
		} else {
			info.name += "Utterly Average ";
		}
		info.name += namingInfo.name + " ";
		info.name += clUtil::uppercase(clUtil::lowercase(convertItemSubType(info.piece)), 1);
	} else if (info.name.find("[type]") != std::string::npos) {
		bool done = false;
		for (int i = 0; i < info.name.length() && !done; i++) {
			if (info.name.substr(i, 6) == "[type]") {
				std::string itemNewBeg = info.name.substr(0, i);
				std::string itemNewEnd = "";
				if (info.name.length() > (i + 6))
					itemNewEnd = " " + info.name.substr(itemNewBeg.length() + 7, info.name.length() - (itemNewBeg.length() + 7));
				info.name = itemNewBeg + clUtil::uppercase(clUtil::lowercase(convertItemSubType(info.piece)), 1) + itemNewEnd;
				done = true;
			}
		}
	}

	//Apply Item Stats
	created.construct(ItemType::ARMOR, info.piece, info.name, CalcManager::getBaseArmorValue(info.level) * fortune, 1, info.level, info.effect);
	return created;
}
Item CalcManager::createRandomWeapon(WeaponInfo info, float fortune)
{
	//Create Item
	Item created;

	//Determine Piece
	if (info.type == ItemSubType::NOSUBTYPE) {
		info.type = ItemSubType((rand() % (ITEMSUBTYPES - (SUBTYPEWEAPONAFTER + 1))) + SUBTYPEWEAPONAFTER + 1);
	}

	//Determine Effect
	if (info.effect == -1) {
		float FortuneScale = 80 * (fortune - 1);
		float luck = 1 + ((rand() % 80 - (40 - FortuneScale)) / 100) * fortune;
		if (luck < 0.01) { luck = 0.01; }

		info.effect = luck * (float)CalcManager::getAverageDamage(info.level);

	}

	//Determine Name
	if (info.name == "RANDOM") {
		info.name = "";

		EquipmentRandomInfo namingInfo = CalcManager::getWeaponRandomInfo(info.level);
		if (info.effect > CalcManager::getAverageDamage(info.level)) {
			info.name += namingInfo.positiveAdjs[(rand() + 1) % namingInfo.positiveAdjs.size()] + " ";
		} else if (info.effect < CalcManager::getAverageDamage(info.level)) {
			info.name += namingInfo.negativeAdjs[(rand() + 1) % namingInfo.negativeAdjs.size()] + " ";
		} else {
			info.name += "Utterly Average ";
		}
		info.name += namingInfo.name + " " + clUtil::uppercase(clUtil::lowercase(convertItemSubType(info.type)), 1);
	} else if (info.name.find("[type]") != std::string::npos) {
		bool done = false;
		for (int i = 0; i < info.name.length() && !done; i++) {
			if (info.name.substr(i, 6) == "[type]") {
				std::string itemNewBeg = info.name.substr(0, i);
				std::string itemNewEnd = "";
				if (info.name.length() > (i + 6))
					itemNewEnd = " " + info.name.substr(itemNewBeg.length() + 7, info.name.length() - (itemNewBeg.length() + 7));
				info.name = itemNewBeg + clUtil::uppercase(clUtil::lowercase(convertItemSubType(info.type)), 1) + itemNewEnd;
				done = true;
			}
		}
	}

	//Apply Item Stats
	created.construct(ItemType::WEAPON, info.type, info.name, (CalcManager::getBaseArmorValue(info.level) * 3) * fortune, 1, info.level, info.effect);
	return created;
}
std::vector<Item> CalcManager::createItemsFromStrings(std::vector<std::string> strings, int referenceLevel)
{
	return convertDropsToItems(createDropsFromStrings(strings, referenceLevel));
}
std::vector<Drop> CalcManager::createDropsFromStrings(std::vector<std::string> strings, int referenceLevel, float eliteness)
{
	//Set Enemy Drops
	std::vector<Drop> drops;
	std::string itemName = "RANDOM";
	int itemChance = 50, itemQuantity = 1, itemValue = -1, itemLevel = referenceLevel, itemEffect = -1;
	float itemFortune = 1.0f;
	ItemType itemType = ItemType(rand() % ITEMTYPES + 1);
	ItemSubType itemSubType = NOSUBTYPE;

	//Determine Lowest Bracket Level
	int lowestBracket = 10000;
	for (int i = 0; i < strings.size(); i++) {
		int thisBracket = 0;
		for (int j = 0; j < strings[i].length(); j++) {
			if (strings[i][j] == '\t') {
				thisBracket++;
			}
		}
		if (thisBracket < lowestBracket) {
			lowestBracket = thisBracket;
		}
	}

	//Reduce To Lowest Bracket Level
	for (int i = 0; i < strings.size(); i++) {
		for (int j = 0; j < lowestBracket; j++) {
			strings[i].erase(strings[i].begin());
		}
	}

	//Parse Lines
	for (int i = 0; i < strings.size(); i++) {

		if (strings[i].find("item chance:") != std::string::npos) { //ITEM CHANCE
			if (clFile::cut(strings[i]) == "ADAPT")
				itemChance = 50;
			else 
				itemChance = stoi(clFile::cut(strings[i]));
		} else if (strings[i].find("item fortune:") != std::string::npos) { //ITEM FORTUNE
			if (clFile::cut(strings[i]) != "ADAPT") {
				itemFortune = stof(clFile::cut(strings[i]));
			}
		} else if (strings[i].find("item name:") != std::string::npos) { //ITEM NAME
			if (clFile::cut(strings[i]) == "ADAPT") {
				itemName = "RANDOM";
			} else {
				itemName = clFile::cut(strings[i]);
			}
		} else if (strings[i].find("item type:") != std::string::npos) { //ITEM TYPE
			if (clFile::cut(strings[i]) == "ADAPT") {
				itemType = ItemType(rand() % ITEMTYPES + 1);
			} else {
				itemType = convertItemString(clFile::cut(strings[i]));
			}
		} else if (strings[i].find("item subtype:") != std::string::npos) { //ITEM SUBTYPE
			if (clFile::cut(strings[i]) == "ADAPT") {
				itemSubType = NOSUBTYPE;
			} else {
				itemSubType = convertItemSubString(clFile::cut(strings[i]));
			}
		} else if (strings[i].find("item quantity:") != std::string::npos) { //ITEM QUANTITY
			if (clFile::cut(strings[i]) == "ADAPT") {
				itemQuantity = 1;
			} else {
				itemQuantity = stoi(clFile::cut(strings[i]));
			}
		} else if (strings[i].find("item value:") != std::string::npos) { //ITEM VALUE
			if (clFile::cut(strings[i]) != "ADAPT") {
				itemValue = stoi(clFile::cut(strings[i]));
			}
		} else if (strings[i].find("item level:") != std::string::npos) { //ITEM LEVEL
			if (clFile::cut(strings[i]).find("ADAPT") != std::string::npos) {
				itemLevel = referenceLevel;
				if (clFile::cut(strings[i]).find("ADAPT + ") != std::string::npos) {
					itemLevel += stoi(clFile::cut(strings[i]).substr(8, clFile::cut(strings[i]).length() - 8));
				}
			} else {
				itemLevel = stoi(clFile::cut(strings[i]));
			}
		} else if (strings[i].find("item effect:") != std::string::npos) { //ITEM EFFECT
			if (clFile::cut(strings[i]) == "ADAPT")
				itemEffect = -1;
			else 
				itemEffect = stoi(clFile::cut(strings[i]));
		} else if (strings[i].find("}") != std::string::npos) { //END OF DROP
			if (itemValue = -1 & itemType != WEAPON && itemType != ARMOR) {
				itemValue = referenceLevel / 2;
				if (itemValue < 1) { itemValue = 1; }
			}
			itemFortune = eliteness;
			if (itemType == ItemType::WEAPON) {
				drops.push_back(Drop{ CalcManager::createRandomWeapon(WeaponInfo(itemLevel, itemEffect, itemName, itemSubType), itemFortune), itemChance });
			} else if (itemType == ItemType::ARMOR) {
				drops.push_back(Drop{ CalcManager::createRandomArmor(ArmorInfo(itemLevel, itemEffect, itemName, itemSubType), itemFortune), itemChance });
			} else {
				drops.push_back(Drop{ Item(itemType, itemSubType, itemName, itemValue, itemQuantity, itemLevel, itemEffect), itemChance });
			}

			//Default Parameters
			itemChance = 50;
			itemName = "RANDOM";
			itemType = ItemType(rand() % ITEMTYPES + 1);
			itemSubType = NOSUBTYPE;
			itemValue = -1;
			itemLevel = referenceLevel;
			itemEffect = -1;
			itemQuantity = 1;
		}
	}
	return drops;
}
std::vector<Item> CalcManager::convertDropsToItems(std::vector<Drop> drops)
{
	std::vector<Item> items;
	for (int i = 0; i < drops.size(); i++) {
		items.push_back(drops[i].dropItem);
	}
	return items;
}