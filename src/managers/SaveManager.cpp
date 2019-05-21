#include "SaveManager.h"

//Menu Functions
SaveInfo SaveManager::loadGame(bool* returned) {

	//variables
	int choice;

	//menu loop
	while (1) {

		//clear screen
		clCons::cls();

		//create list of saves
		std::vector<std::string> savesList = getListOfSaves();
		std::vector<std::string> menuOptions = savesList;
		menuOptions.push_back("[Delete Save]");
		menuOptions.push_back("[Return]");

		//grab input
		choice = ColorManager::menu(ColorFlag::GRAY, "[Choose a save]", ColorFlag::GRAY, "", ColorFlag::GRAY, menuOptions, '-', '\033', menuOptions.size());

		//parse input
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
SaveInfo SaveManager::newGame(bool* returned) {

	//variables
	SaveInfo saveInfo{};
	std::vector<Player> createdPlayers;
	std::vector<std::string> storyMenu = StoryManager::getStoryList();
	storyMenu.push_back("[Return]");
	std::vector<std::string> saveNames = getListOfSaves();
	std::string createdSaveName, playerName;
	char playerSymbol;
	int storySelection;
	int players;

	//prompt story selection
	clCons::cls();
	storySelection = ColorManager::menu(ColorFlag::GRAY, "[Choose a story to play]", ColorFlag::GRAY, "", ColorFlag::GRAY, storyMenu, '-', 27, storyMenu.size());
	if (storySelection == storyMenu.size()) {
		*returned = true;
		return SaveInfo{};
	}
	saveInfo.storyInfo = StoryManager::loadStoryInfo(storyMenu[storySelection - 1]);

	//prompt player amount selection
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

	//create player stats
	for (int i = 0; i < players; i++) {

		//prompt player name
		clCons::cls();
		playerName = clCons::advInput("Enter player " + to_string(i + 1) + "'s name:");
		while (!clUtil::containsOnlyLetters(playerName)) {
			clCons::centerVerhor(1, "Your name may only contain letters");
			clCons::paus("");
			playerName = clCons::advInput("Enter player " + to_string(i + 1) + "'s name:");
		}

		//prompt player symbol
		playerSymbol = clCons::advInput("Enter player " + to_string(i + 1) + "'s symbol:")[0];
		while (!(playerSymbol > 32 && playerSymbol < 127)) {
			clCons::centerVerhor(1, "That is not a valid symbol. Try again");
			clCons::paus("");
			playerSymbol = clCons::advInput("Enter player " + to_string(i + 1) + "'s symbol:")[0];
		}

		//create player
		Player nextPlayer(EntityType::PLAYER, playerName, playerSymbol, 9, 3);
		nextPlayer.constructPlayerAttributes(0, 0, 0, 0);
		nextPlayer.constructPlayerExtras(1, 0, -1, 0, saveInfo.storyInfo.levels, std::vector<Item>(), std::vector<Item>());
		nextPlayer.setWeaponLevels(AttackManager::getGenericWeaponLevels());
		createdPlayers.push_back(nextPlayer);
	}
	saveInfo.currentPlayers = createdPlayers;

	//prompt save name
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

	//create map
	saveInfo.currentMap = Map(saveInfo.storyInfo.startingMap, saveInfo.storyInfo.name);

	//place players
	saveInfo.currentMap.setPlayerStartingPositions(&saveInfo.currentPlayers);

	//place enemies
	CalcManager::loadRandomInfo(saveInfo.storyInfo.name);
	saveInfo.currentEnemies = saveInfo.currentMap.loadMapEnemies(1);

	//create save
	saveInfo.saveName = createdSaveName;

	//update saves file
	std::vector<std::string> savesList = getListOfSaves();
	savesList.push_back(createdSaveName);
	updateSavesFile(savesList);

	//save game
	save(saveInfo);

	//return info
	return saveInfo;
}

//Save Functions
void SaveManager::save(SaveInfo info) {

	//create master node
	Node save("save", "save");

	//story name and map file
	save.addChild(Node("story name", info.storyInfo.name));
	save.addChild(Node("map file", info.currentMap.getFileSource()));

	//past events
	Node pastEvents("past events", "past events");
	for (int pastEvent : info.pastEvents) pastEvents.addChild(Node(std::to_string(pastEvent), std::to_string(pastEvent)));
	save.addChild(pastEvents);

	//player info
	Node players("players", "players");
	for (Player player : info.currentPlayers) players.addChild(player.convertToNode());
	save.addChild(players);

	//enemy info
	Node enemyInfos("enemy infos", "enemy infos");
	for (EnemyInfo enemyInfo : info.currentEnemies) enemyInfos.addChild(enemyInfo.convertToNode());
	save.addChild(enemyInfos);

	//save node
	Node::writeNode("data//saves//" + info.saveName + ".wsave", &save);
}
void SaveManager::deleteSave(std::string saveName) {

	//get saves list
	std::vector<std::string> newSaveList = getListOfSaves();

	//remove saves w/ matching name
	for (int i = 0; i < newSaveList.size(); i++) {
		if (saveName == newSaveList[i]) {
			newSaveList.erase(newSaveList.begin() + i);
			std::string saveDir = "data//saves//" + saveName + ".wsave";
			if (remove(saveDir.c_str()) != 0) OptionsManager::wError("Could not delete " + saveName + ".wsave, assuming non-existence", "SAVEMANAGER_H", false);
		}
	}

	//update saves list
	updateSavesFile(newSaveList);
}
std::vector<std::string> SaveManager::getListOfSaves() {

	//variables
	std::vector<std::string> savesList;
	std::string nextLine = "";

	//open file
	ifstream read;
	read.open("data//saves//saves.wtxt");
	if (read.fail()) {
		OptionsManager::wError("unable to read from 'data/saves/saves.wtxt'", "SAVEMANAGER_H");
	}

	//read file
	while (!read.eof()) {
		getline(read, nextLine);
		if (nextLine != "saves:" && nextLine != "{" && nextLine != "}" && nextLine != "") {
			savesList.push_back(nextLine);
		}
	}

	//close file and return value
	read.close();
	return savesList;
}

//Private Safe Functions
SaveInfo SaveManager::loadSave(std::string saveName) {

	//create save info and load node
	SaveInfo saveInfo;
	saveInfo.saveName = saveName;
	Node node = Node::readNode("data//saves//" + saveName + ".wsave");

	//go through each child
	for (Node child : *(node.getChildren())) {

		//story name and map file
		if (child == "story name") saveInfo.storyInfo = StoryManager::loadStoryInfo(child.getValue());
		if (child == "map file") {
			if (saveInfo.storyInfo.name == "") OptionsManager::wError("save corrupted: map file provided before story name", "SAVEMANAGER_H");
			else saveInfo.currentMap = Map(child.getValue(), saveInfo.storyInfo.name);
		}

		//players
		if (child == "players") {
			if (saveInfo.storyInfo.levels.size() < 1) OptionsManager::wError("save corrupted: players provided before story name", "SAVEMANAGER_H");
			for (Node child_ : *(child.getChildren())) {
				saveInfo.currentPlayers.push_back(Player::createPlayerFromNode(&child_, saveInfo.storyInfo.levels));
			}
		}

		//enemy infos
		if (child == "enemy infos") {
			for (Node child_ : *(child.getChildren())) {
				saveInfo.currentEnemies.push_back(EnemyInfo::createEnemyInfoFromNode(&child_));
			}
		}
	}

	return saveInfo;
}
void SaveManager::updateSavesFile(std::vector<std::string> newSavesList) {

	//open file
	ofstream write;
	write.open("data//saves//saves.wtxt");
	if (write.fail()) {
		OptionsManager::wError("unable to write to 'data/saves/saves.wtxt'", "SAVEMANAGER_H");
	}

	//write saves
	write << "saves:" << std::endl << "{" << std::endl;
	for (int i = 0; i < newSavesList.size(); i++) {
		write << newSavesList[i] << std::endl;
	}
	write << "}" << std::endl;

	//close file
	write.close();
}