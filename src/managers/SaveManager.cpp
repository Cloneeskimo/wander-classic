#include "saveManager.h"

//Menus
SaveInfo SaveManager::loadGame(bool* returned)
{
	//Variables
	int choice;

	//Menu Loop
	while (1) {
		//Clear Screen
		clCons::cls();

		//Create List Of Saves
		std::vector<std::string> savesList = getListOfSaves();
		std::vector<std::string> menuOptions = savesList;
		menuOptions.push_back("[Delete Save]");
		menuOptions.push_back("[Return]");

		//Grab Input
		choice = ColorManager::menu(ColorFlag::GRAY, "[Choose a save]", ColorFlag::GRAY, "", ColorFlag::GRAY, menuOptions, '-', '\033', menuOptions.size());

		//Parse Input
		if (choice == menuOptions.size()) { // RETURN
			*returned = true;
			return SaveInfo{};
		} else if (choice == menuOptions.size() - 1) { // DELETE SAVE
			clCons::cls();
			std::vector<std::string> deleteOptions = savesList;
			deleteOptions.push_back("[Return]");
			int choice2 = ColorManager::menu(ColorFlag::GRAY, "[Which save do you wish to delete?]", ColorFlag::GRAY, "", ColorFlag::GRAY, deleteOptions, '-', '\033', deleteOptions.size());
			if (choice2 != deleteOptions.size()) 
				deleteSave(savesList[choice2 - 1]);
		} else { // LOAD GAME
			SaveInfo info = loadSave(savesList[choice - 1]);
			return info;
		}
	}
}
SaveInfo SaveManager::newGame(bool* returned)
{
	//Variables
	SaveInfo saveInfo{};
	std::vector<Player> createdPlayers;
	std::vector<std::string> storyMenu = StoryManager::getStoryList();
	storyMenu.push_back("[Return]");
	std::vector<std::string> saveNames = getListOfSaves();
	std::string createdSaveName, playerName;
	char playerSymbol;
	int storySelection;
	int players;

	//Prompt Story Selection
	clCons::cls();
	storySelection = ColorManager::menu(ColorFlag::GRAY, "[Choose a story to play]", ColorFlag::GRAY, "", ColorFlag::GRAY, storyMenu, '-', 27, storyMenu.size());
	if (storySelection == storyMenu.size()) {
		*returned = true;
		return SaveInfo{};
	}
	saveInfo.storyInfo = StoryManager::loadStoryInfo(storyMenu[storySelection - 1]);

	//Prompt Player Amount Selection
	clCons::cls();
	if (saveInfo.storyInfo.players.size() > 1) {
		std::vector<std::string> playerMenu;
		for (int i = 0; i < saveInfo.storyInfo.players.size(); i++) {
			if (saveInfo.storyInfo.players[i] == 1) {
				playerMenu.push_back("1 Player");
			} else {
				playerMenu.push_back(to_string(saveInfo.storyInfo.players[i]) + " Players");
			}
		}
		players = saveInfo.storyInfo.players[ColorManager::menu(ColorFlag::GRAY, "[Choose the amount of players]", ColorFlag::GRAY, "", ColorFlag::GRAY, playerMenu) - 1];
	} else {
		players = saveInfo.storyInfo.players[0];
	}

	//Create Player Stats
	for (int i = 0; i < players; i++) {
		//Prompt Player Name
		clCons::cls();
		playerName = clCons::advInput("Enter player " + to_string(i + 1) + "'s name:");
		while (!clUtil::containsOnlyLetters(playerName)) {
			clCons::centerVerhor(1, "Your name may only contain letters");
			clCons::paus("");
			playerName = clCons::advInput("Enter player " + to_string(i + 1) + "'s name:");
		}

		//Prompt Player Symbol
		playerSymbol = clCons::advInput("Enter player " + to_string(i + 1) + "'s symbol:")[0];
		while (!(playerSymbol > 32 && playerSymbol < 127)) {
			clCons::centerVerhor(1, "That is not a valid symbol. Try again");
			clCons::paus("");
			playerSymbol = clCons::advInput("Enter player " + to_string(i + 1) + "'s symbol:")[0];
		}

		//Create Player
		Player nextPlayer(EntityType::PLAYER, playerName, playerSymbol, 9, 3);
		nextPlayer.constructPlayerAttributes(0, 0, 0, 0);
		nextPlayer.constructPlayerExtras(1, 0, -1, 0, saveInfo.storyInfo.levels, std::vector<Item>(), std::vector<Item>());
		nextPlayer.setWeaponLevels(AttackManager::getGenericWeaponLevels());
		createdPlayers.push_back(nextPlayer);
	}
	saveInfo.currentPlayers = createdPlayers;

	//Prompt Save Name
	bool validSaveName;
	do {
		validSaveName = true;
		createdSaveName = clCons::advInput("Enter the name of this save:");
		if (!clUtil::containsOnlyLetters(createdSaveName)) {
			clCons::centerVerhor(1, "The save name may only contain letters");
			clCons::paus("");
			validSaveName = false;
		}
		for (int i = 0; i < saveNames.size(); i++) {
			if (createdSaveName == saveNames[i]) {
				clCons::centerVerhor(1, "You already have a save named '" + createdSaveName + "'");
				clCons::paus("");
				validSaveName = false;
			}
		}
	} while (!validSaveName);

	//Create Map
	saveInfo.currentMap = Map(saveInfo.storyInfo.startingMap, saveInfo.storyInfo.name);

	//Place Players
	saveInfo.currentMap.setPlayerStartingPositions(&saveInfo.currentPlayers);

	//Place Enemies
	CalcManager::loadRandomInfo(saveInfo.storyInfo.name);
	saveInfo.currentEnemies = saveInfo.currentMap.loadMapEnemies(1);

	//Create Save
	saveInfo.saveName = createdSaveName;

	//Update Saves File
	std::vector<std::string> savesList = getListOfSaves();
	savesList.push_back(createdSaveName);
	updateSavesFile(savesList);

	//Save Game
	save(saveInfo);

	//Return Info
	return saveInfo;
}

//Save Functions
void SaveManager::save(SaveInfo info)
{
	//Open File
	ofstream write;
	write.open("data//saves//" + info.saveName + ".wsave");

	//Error-Checking
	if (write.fail()) {
		OptionsManager::wError("Unable to write to " + info.saveName + ".wsave", "SAVEMANAGER_H");
	}
	
	//Save Info
	write << "story name: " << info.storyInfo.name << std::endl;
	write << "map file: " << info.currentMap.getFileSource() << std::endl;

	//Past Events
	write << "past events:" << std::endl << "{" << std::endl;
	for (int i = 0; i < info.pastEvents.size(); i++) {
		write << info.pastEvents[i] << std::endl;
	}
	write << "}" << std::endl;

	//Save Player Info
	write << "players:" << std::endl << "{" << std::endl;
	for (int i = 0; i < info.currentPlayers.size(); i++) {
		//Save Player Stats
		write << "\t{" << std::endl;		
		write << "\t\tname: " << info.currentPlayers[i].getName() << std::endl;
		write << "\t\tspeed: " << info.currentPlayers[i].getSpeed() << std::endl;
		write << "\t\tplayer x: " << info.currentPlayers[i].getX() << std::endl;
		write << "\t\tplayer y: " << info.currentPlayers[i].getY() << std::endl;
		write << "\t\tsymbol: '" << info.currentPlayers[i].getSymbol() << "'" << std::endl;
		write << "\t\tlevel: " << info.currentPlayers[i].getLevel() << std::endl;
		write << "\t\tgold: " << info.currentPlayers[i].getGold() << std::endl;
		write << "\t\thealth: " << info.currentPlayers[i].getHealth() << std::endl;
		write << "\t\tattribute points: " << info.currentPlayers[i].getAttributePoints() << std::endl;
		write << "\t\tendurance: " << info.currentPlayers[i].getEndurance() << std::endl;
		write << "\t\tspirit: " << info.currentPlayers[i].getSpirit() << std::endl;
		write << "\t\tcapability: " << info.currentPlayers[i].getCapability() << std::endl;
		write << "\t\texp: " << info.currentPlayers[i].getExp() << std::endl;
		write << "\t\tinventory: " << std::endl << "\t\t{" << std::endl;

		//Save Player Inventory
		for (int j = 0; j < (*info.currentPlayers[i].getInventory()).size() ; j++) {
			write << "\t\t\t{" << std::endl;
			write << "\t\t\t\tname: " << info.currentPlayers[i].getInventoryItem(j).getName() << std::endl;
			write << "\t\t\t\tbasetype: " + convertItemType(info.currentPlayers[i].getInventoryItem(j).getType()) << std::endl;
			write << "\t\t\t\tsubtype: " + convertItemSubType(info.currentPlayers[i].getInventoryItem(j).getSubType()) << std::endl;
			write << "\t\t\t\tquantity: " << info.currentPlayers[i].getInventoryItem(j).getQuantity() << std::endl;
			write << "\t\t\t\tvalue: " << info.currentPlayers[i].getInventoryItem(j).getValue() << std::endl;
			write << "\t\t\t\tlevel: " << info.currentPlayers[i].getInventoryItem(j).getLevel() << std::endl;
			write << "\t\t\t\teffect: " << info.currentPlayers[i].getInventoryItem(j).getEffect() << std::endl;
			write << "\t\t\t}" << std::endl;
		}
		
		//Save Player Equipment
		write << "\t\t}" << std::endl << "\t\tequipment:" << std::endl << "\t\t{" << std::endl;
		for (int k = 0; k < (*info.currentPlayers[i].getEquipment()).size(); k++) {
			write << "\t\t\t{" << std::endl;
			write << "\t\t\t\tname: " << (*info.currentPlayers[i].getEquipment())[k].getName() << std::endl;
			write << "\t\t\t\tbasetype: " + convertItemType((*info.currentPlayers[i].getEquipment())[k].getType()) << std::endl;
			write << "\t\t\t\tsubtype: " + convertItemSubType((*info.currentPlayers[i].getEquipment())[k].getSubType()) << std::endl;
			write << "\t\t\t\tquantity: " << (*info.currentPlayers[i].getEquipment())[k].getQuantity() << std::endl;
			write << "\t\t\t\tvalue: " << (*info.currentPlayers[i].getEquipment())[k].getValue() << std::endl;
			write << "\t\t\t\tlevel: " << (*info.currentPlayers[i].getEquipment())[k].getLevel() << std::endl;
			write << "\t\t\t\teffect: " << (*info.currentPlayers[i].getEquipment())[k].getEffect() << std::endl;
			write << "\t\t\t}" << std::endl;
		}
		write << "\t\t}" << std::endl;

		//Save Player Weapon Levels
		write << "\t\tweapon levels:" << std::endl << "\t\t{" << std::endl;
		for (int k = 0; k < info.currentPlayers[i].getWeaponLevels().size(); k++) {
			write << "\t\t\t" << convertItemSubType(info.currentPlayers[i].getWeaponLevels()[k].weapon) << ": " << info.currentPlayers[i].getWeaponLevels()[k].level << std::endl;
		}
		write << "\t\t}" << std::endl << "\t}" << std::endl;
	}
	write << "}" << std::endl;

	//Save Enemy Info
	write << "enemies: " << std::endl << "{" << std::endl;
	for (int i = 0; i < info.currentEnemies.size(); i++) {
		write << "\t{" << std::endl;
		write << "\t\tenemy name: " << info.currentEnemies[i].enemy.getName() << std::endl;
		write << "\t\tenemy symbol: '" << info.currentEnemies[i].enemy.getSymbol() << "'" << std::endl;
		write << "\t\tenemy health: " << info.currentEnemies[i].enemy.getHealth() << std::endl;
		write << "\t\tenemy max health: " << info.currentEnemies[i].enemy.getMaxHealth() << std::endl;
		write << "\t\tenemy damage: " << info.currentEnemies[i].enemy.getDamage() << std::endl;
		write << "\t\tenemy speed: " << info.currentEnemies[i].enemy.getSpeed() << std::endl;
		write << "\t\tenemy level: " << info.currentEnemies[i].enemy.getLevel() << std::endl;
		write << "\t\tenemy x: " << info.currentEnemies[i].enemy.getX() << std::endl;
		write << "\t\tenemy y: " << info.currentEnemies[i].enemy.getY() << std::endl;
		write << "\t\tenemy agro distance: " << info.currentEnemies[i].enemy.getAgroDistance() << std::endl;
		write << "\t\tenemy map: " << info.currentEnemies[i].map << std::endl;
		write << "\t\tenemy drops:" << std::endl << "\t\t{" << std::endl;
		for (int j = 0; j < info.currentEnemies[i].enemy.getDrops().size(); j++) {
			Drop nextDrop = info.currentEnemies[i].enemy.getDrops()[j];
			write << "\t\t\t{" << std::endl;
			write << "\t\t\t\titem chance: " << nextDrop.chance << std::endl;
			write << "\t\t\t\titem name: " << nextDrop.dropItem.getName() << std::endl;
			write << "\t\t\t\titem basetype: " << convertItemType(nextDrop.dropItem.getType()) << std::endl;
			write << "\t\t\t\titem subtype: " << convertItemSubType(nextDrop.dropItem.getSubType()) << std::endl;
			write << "\t\t\t\titem quantity: " << nextDrop.dropItem.getQuantity() << std::endl;
			write << "\t\t\t\titem level: " << nextDrop.dropItem.getLevel() << std::endl;
			write << "\t\t\t\titem effect: " << nextDrop.dropItem.getEffect() << std::endl;
			write << "\t\t\t\titem value: " << nextDrop.dropItem.getValue() << std::endl;
			write << "\t\t\t}" << std::endl;
		}
		write << "\t\t}" << std::endl << "\t}" << std::endl;
	}
	write << "}" << std::endl;
}
void SaveManager::deleteSave(std::string saveName)
{
	//Get Saves List
	std::vector<std::string> newSaveList = getListOfSaves();

	//Remove Saves w/ Matching Name
	for (int i = 0; i < newSaveList.size(); i++) {
		if (saveName == newSaveList[i]) {

			newSaveList.erase(newSaveList.begin() + i);

			std::string saveDir = "data//saves//" + saveName + ".wsave";

			if (remove(saveDir.c_str()) != 0)
				OptionsManager::wError("Could not delete " + saveName + ".wsave, assuming non-existence", "SAVEMANAGER_H", false);
		}
	}

	//Update Saves List
	updateSavesFile(newSaveList);
}
std::vector<std::string> SaveManager::getListOfSaves()
{
	//Variables
	std::vector<std::string> savesList;
	std::string nextLine = "";
	
	//Open File
	ifstream read;
	read.open("data//saves//saves.wtxt");
	if (read.fail()) {
		OptionsManager::wError("unable to read from 'data/saves/saves.wtxt'", "SAVEMANAGER_H");
	} 

	//Read File
	while (!read.eof()) {
		getline(read, nextLine);
		if (nextLine != "saves:" && nextLine != "{" && nextLine != "}" && nextLine != "") {
			savesList.push_back(nextLine);
		}
	}

	//Close File and Return Value
	read.close();
	return savesList;
}

//Private Safe Functions
SaveInfo SaveManager::loadSave(std::string saveName)
{
	//Open File
	ifstream read;
	read.open("data//saves//" + saveName + ".wsave");
	if (read.fail()) {
		OptionsManager::wError("Unable to open '" + saveName + ".wsave'", "SAVEMANAGER_H");
	}

	//Load Data
	dataSegment saveData = clFile::readDataSegment(&read);

	//Parse Data
	SaveInfo saveInfo;
	for (int i = 0; i < saveData.cs(); i++) {

		//Story Name
		if (*saveData.c(i) == "STORY NAME") {
			saveInfo.storyInfo = StoryManager::loadStoryInfo(saveData.cv(i));
		}

		//Map File
		else if (*saveData.c(i) == "MAP FILE") {
			saveInfo.currentMap = Map(saveData.cv(i), saveInfo.storyInfo.name);
		}

		//Past Events
		else if (*saveData.c(i) == "PAST EVENTS") {
			for (int j = 0; j < saveData.c(i)->cs(); j++) {
				saveInfo.pastEvents.push_back(saveData.c(i)->c(j)->vint());
			}
		}

		//Players
		else if (*saveData.c(i) == "PLAYERS") {

			//Load Each Player
			for (int j = 0; j < saveData.c(i)->cs(); j++) {

				//Player Info
				std::vector<Item> inventory, equipment;
				std::vector<WeaponLevel> weaponLevels;
				std::string name;
				char symbol;
				int x, y, speed, level, gold, health, endurance, spirit, capability, attPoints, exp;				
				
				//Info Loading
				for (int k = 0; k < saveData.c(i)->c(j)->cs(); k++) {
					if (*saveData.c(i)->c(j)->c(k) == "NAME") {
						name = saveData.c(i)->c(j)->cv(k);
					} else if (*saveData.c(i)->c(j)->c(k) == "SYMBOL") {
						symbol = saveData.c(i)->c(j)->cv(k)[1];
					} else if (*saveData.c(i)->c(j)->c(k) == "PLAYER X") {
						x = saveData.c(i)->c(j)->c(k)->vint();
					} else if (*saveData.c(i)->c(j)->c(k) == "PLAYER Y") {
						y = saveData.c(i)->c(j)->c(k)->vint();
					} else if (*saveData.c(i)->c(j)->c(k) == "SPEED") {
						speed = saveData.c(i)->c(j)->c(k)->vint();
					} else if (*saveData.c(i)->c(j)->c(k) == "LEVEL") {
						level = saveData.c(i)->c(j)->c(k)->vint();
					} else if (*saveData.c(i)->c(j)->c(k) == "GOLD") {
						gold = saveData.c(i)->c(j)->c(k)->vint();
					} else if (*saveData.c(i)->c(j)->c(k) == "HEALTH") {
						health = saveData.c(i)->c(j)->c(k)->vint();
					} else if (*saveData.c(i)->c(j)->c(k) == "ATTRIBUTE POINTS") {
						attPoints = saveData.c(i)->c(j)->c(k)->vint();
					} else if (*saveData.c(i)->c(j)->c(k) == "ENDURANCE") {
						endurance = saveData.c(i)->c(j)->c(k)->vint();
					} else if (*saveData.c(i)->c(j)->c(k) == "SPIRIT") {
						spirit = saveData.c(i)->c(j)->c(k)->vint();
					} else if (*saveData.c(i)->c(j)->c(k) == "CAPABILITY") {
						capability = saveData.c(i)->c(j)->c(k)->vint();
					} else if (*saveData.c(i)->c(j)->c(k) == "WEAPON LEVELS") {

					}
				}
				
				//Player Creation
				Player nextPlayer = Player(EntityType::PLAYER, name, symbol, x, y, speed);
				nextPlayer.constructPlayerExtras(level, gold, health, exp, saveInfo.storyInfo.levels, inventory, equipment);
				nextPlayer.constructPlayerAttributes(attPoints, endurance, spirit, capability);
				nextPlayer.setWeaponLevels(weaponLevels);
				saveInfo.currentPlayers.push_back(nextPlayer);
			}
		}
	}

				} else if (nextLine.find("weapon levels:") != std::string::npos) { //WEAPON LEVELS
					WeaponLevel nextWeaponLevel;
					while (nextLine != "\t\t}") {
						getline(read, nextLine);
						std::string cutLine = nextLine;

						if (cutLine.find("{") == std::string::npos && cutLine.find("}") == std::string::npos) {
							for (int i = 0; i < cutLine.length(); i++) {
								if (cutLine[i] == '\t') {
									cutLine.erase(cutLine.begin() + i);
									i--;				
								} else if (cutLine[i] == ':') {
									cutLine = cutLine.substr(0, i);
								}
							}
							nextWeaponLevel.weapon = convertItemSubString(cutLine);
							nextWeaponLevel.level = stoi(clFile::cut(nextLine));
							weaponLevels.push_back(nextWeaponLevel);
						}
					}
				} else if (nextLine.find("exp:") != std::string::npos) { //EXP
					exp = stoi(clFile::cut(nextLine));
				} else if (nextLine.find("inventory:") != std::string::npos || nextLine.find("equipment:") != std::string::npos) { //INVENTORY & EQUIPMENT
					//Determine Inventory or Equipment
					bool inventoryNotEquipment = true;
					if (nextLine.find("equipment:") != std::string::npos)
						inventoryNotEquipment = false;

					//Item Stats
					std::string itemName;
					ItemType itemType;
					ItemSubType itemSubType;
					int itemQuantity, itemValue, itemLevel, itemEffect;

					//Parse Line		
					while (nextLine != "\t\t}") {
						getline(read, nextLine);

						if (nextLine == "\t\t\t}") {
							if (inventoryNotEquipment)
								inventory.push_back(Item(itemType, itemSubType, itemName, itemValue, itemQuantity, itemLevel, itemEffect));
							else
								equipment.push_back(Item(itemType, itemSubType, itemName, itemValue, itemQuantity, itemLevel, itemEffect));
						} else if (nextLine.find("name:") != std::string::npos) { //NAME
							itemName = clFile::cut(nextLine);
						} else if (nextLine.find("basetype:") != std::string::npos) { //TYPE
							itemType = convertItemString(clFile::cut(nextLine));
						} else if (nextLine.find("subtype:") != std::string::npos) { //SUBTYPE
							itemSubType = convertItemSubString(clFile::cut(nextLine));
						} else if (nextLine.find("quantity:") != std::string::npos) { //QUANTITY
							itemQuantity = stoi(clFile::cut(nextLine));
						} else if (nextLine.find("value:") != std::string::npos) { //VALUE
							itemValue = stoi(clFile::cut(nextLine));
						} else if (nextLine.find("level:") != std::string::npos) { //LEVEL
							itemLevel = stoi(clFile::cut(nextLine));
						} else if (nextLine.find("effect:") != std::string::npos) { //EFFECT
							itemEffect = stoi(clFile::cut(nextLine));
						}
					}
				}
			}
			saveInfo.currentPlayers = players;
		} else if (nextLine.find("enemies:") != std::string::npos) { //ENEMIES
			//Variables
			std::string enemyName, enemyMap;
			char enemySymbol;
			int enemyHealth, enemyMaxHealth, enemyDamage, enemySpeed, enemyLevel, enemyX, enemyY, enemyAgroDistance;
			std::vector<Drop> enemyDrops;

			//Parse Line
			OptionsManager::displayLoading("Loading enemies...", true);
			while (nextLine != "}") {
				getline(read, nextLine);

				if (nextLine == "\t{") { //NEW ENEMY
					enemyDrops = std::vector<Drop>();
				} else if (nextLine == "\t}") { //DONE WITH ENEMY
					Enemy nextEnemy(EntityType::ENEMY, enemyName, enemySymbol, enemyX, enemyY, enemySpeed);
					nextEnemy.constructEnemyExtras(enemyDrops, enemyMaxHealth, enemyDamage, enemyLevel);
					nextEnemy.setHealth(enemyHealth);
					saveInfo.currentEnemies.push_back(EnemyInfo{ nextEnemy, enemyMap });
				} else if (nextLine.find("enemy name:") != std::string::npos) { //ENEMY NAME
					enemyName = clFile::cut(nextLine);
				} else if (nextLine.find("enemy symbol:") != std::string::npos) { //ENEMY SYMBOL
					enemySymbol = clFile::cut(nextLine)[1];
				} else if (nextLine.find("enemy health:") != std::string::npos) { //ENEMY HEALTH
					enemyHealth = stoi(clFile::cut(nextLine));
				} else if (nextLine.find("enemy max health:") != std::string::npos) { //ENEMY MAX HEALTh
					enemyMaxHealth = stoi(clFile::cut(nextLine));
				} else if (nextLine.find("enemy damage:") != std::string::npos) { //ENEMY DAMAGE
					enemyDamage = stoi(clFile::cut(nextLine));
				} else if (nextLine.find("enemy speed:") != std::string::npos) { //ENEMY SPEED
					enemySpeed = stoi(clFile::cut(nextLine));
				} else if (nextLine.find("enemy level:") != std::string::npos) { //ENEMY LEVEL
					enemyLevel = stoi(clFile::cut(nextLine));
				} else if (nextLine.find("enemy x:") != std::string::npos) { //ENEMY X
					enemyX = stoi(clFile::cut(nextLine));
				} else if (nextLine.find("enemy y:") != std::string::npos) { //ENEMY Y
					enemyY = stoi(clFile::cut(nextLine));
				} else if (nextLine.find("enemy map:") != std::string::npos) { //ENEMY MAP
					enemyMap = clFile::cut(nextLine);
				} else if (nextLine.find("enemy agro distance:") != std::string::npos) { //ENEMY AGRO DISTANCE
					enemyAgroDistance = stoi(clFile::cut(nextLine));
				} else if (nextLine.find("enemy drops:") != std::string::npos) { //ENEMY DROPS
					//Variables
					int itemChance, itemLevel, itemEffect, itemQuantity, itemValue;
					std::string itemName;
					ItemType itemType;
					ItemSubType itemSubType;

					//Parse Line
					while (nextLine != "\t\t}") {
						getline(read, nextLine);

						if (nextLine == "\t\t\t}") {//DONE WITH DROP
							Item nextItem;
							nextItem.construct(itemType, itemSubType, itemName, itemValue, itemQuantity, itemLevel, itemEffect);
							enemyDrops.push_back(Drop{ nextItem, itemChance });
						} else if (nextLine.find("item chance:") != std::string::npos) { //ITEM CHANCE
							itemChance = stoi(clFile::cut(nextLine));
						} else if (nextLine.find("item name:") != std::string::npos) { //ITEM NAME
							itemName = clFile::cut(nextLine);
						} else if (nextLine.find("item basetype:") != std::string::npos) { //ITEM TYPE
							itemType = convertItemString(clFile::cut(nextLine));
						} else if (nextLine.find("item subtype:") != std::string::npos) { //ITEM SUBTYPE
							itemSubType = convertItemSubString(clFile::cut(nextLine));
						} else if (nextLine.find("item quantity:") != std::string::npos) { //ITEM QUANTITY
							itemQuantity = stoi(clFile::cut(nextLine));
						} else if (nextLine.find("item level:") != std::string::npos) { //ITEM LEVEL
							itemLevel = stoi(clFile::cut(nextLine));
						} else if (nextLine.find("item effect:") != std::string::npos) { //ITEM EFFECT
							itemEffect = stoi(clFile::cut(nextLine));
						} else if (nextLine.find("item value:") != std::string::npos) { //ITEM VALUE
							itemValue = stoi(clFile::cut(nextLine));
						}
					}
				}
			}
		}
	}

	//Load Map	
	OptionsManager::displayLoading("Loading map...", true);
	saveInfo.currentMap = Map(mapFile, saveInfo.storyInfo.name);
	return saveInfo;
}
void SaveManager::updateSavesFile(std::vector<std::string> newSavesList)
{
	//Open File
	ofstream write;
	write.open("data//saves//saves.wtxt");
	if (write.fail()) {
		OptionsManager::wError("unable to write to 'data/saves/saves.wtxt'", "SAVEMANAGER_H");
	} 

	//Write Saves
	write << "saves:" << std::endl << "{" << std::endl;
	for (int i = 0; i < newSavesList.size(); i++) {
		write << newSavesList[i] << std::endl;
	}
	write << "}" << std::endl;

	//Close File
	write.close();
}