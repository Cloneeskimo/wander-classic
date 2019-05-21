#include "gameEngine.h"

//Initializer
void GameEngine::init()
{
	//Console Title
	system(("Title Wander B:" + to_string(BUILD_NUMBER)).c_str());

	//Load Game Options
	OptionsManager::loadOptions();

	//Define Menu Options
	std::vector<std::string> menuOptions;
	menuOptions.push_back("New Game");
	menuOptions.push_back("Load Game");
	menuOptions.push_back("Options");
	menuOptions.push_back("Quit");

	//Menu Loop
	while (!this->_hasQuit) {
		clCons::cls();
		int menuNav = ColorManager::menu(ColorFlag::GRAY, "[Wander]", ColorFlag::GRAY, "[Build " + to_string(BUILD_NUMBER) + "]", ColorFlag::GRAY, menuOptions);
		switch (menuNav) {

			// NEW GAME
		case 1: { 
			bool returned = false;
			SaveInfo toImplement = SaveManager::newGame(&returned);
			if (!returned) {
				implementSave(toImplement);
			}
			break; }

			// LOAD GAME
		case 2: { 
			bool returned = false;
 			SaveInfo toImplement = SaveManager::loadGame(&returned);
			if (!returned) {
				implementSave(toImplement);
			}
			break; }

			// OPTIONS
		case 3: 
			optionsLoop();
			break;

			// QUIT
		case 4: 
			_hasQuit = true;
			break;
		};
	}
}

//Save Functions
void GameEngine::saveGame()
{
	//Create SaveInfo
	SaveInfo saveInfo{};

	//Add Information
	saveInfo.saveName = this->_saveName;
	saveInfo.currentEnemies = this->_gameEnemies;
	saveInfo.currentPlayers = this->_gamePlayers;
	saveInfo.currentMap = this->_currentMap;
	saveInfo.storyInfo = this->_storyInfo;
	saveInfo.pastEvents = this->_pastEvents;

	//Save
	SaveManager::save(saveInfo);

	//Inform & Pause
	clCons::cls();
	clCons::centerVerhor(1, "Game Saved!");
	clCons::paus("");
}
void GameEngine::promptSave()
{
	//Variables
	std::vector<std::string> menuOptions;
	menuOptions.push_back("Yes");
	menuOptions.push_back("No");

	//Prompt to save before exiting
	clCons::cls();
	int choice = clCons::menu("[Save '" + this->_saveName + "' before exiting?]", "", menuOptions);
	
	//Parse Choice
	if (choice == 1) {
		saveGame();
	} 
}
void GameEngine::implementSave(SaveInfo saveInfo)
{
	//Reset Variables
	this->_isOver = false;
	clCons::cls();

	//Implement Loaded Material
	if (OptionsManager::loadingMax == 0) {
		OptionsManager::loadingMax = 5;
	}
	OptionsManager::displayLoading("Implementing loaded material...", true);
	this->_saveName = saveInfo.saveName;
	this->_currentMap = saveInfo.currentMap;
	this->_gameEnemies = saveInfo.currentEnemies;
	this->_gamePlayers = saveInfo.currentPlayers;
	OptionsManager::displayLoading("Updating entities...", true);
	this->_currentMap.updateEntities(&this->_gameEnemies, &this->_gamePlayers);
	this->_currentMap.removeDefaultEnemyTiles();
	this->_storyInfo = saveInfo.storyInfo;
	this->_gateLocs = this->_currentMap.getGatePositions();
	this->_archiveMaps = std::vector<Map>();
	this->_pastEvents = saveInfo.pastEvents;

	//Load Events
	OptionsManager::displayLoading("Loading events...", true);
	JournalManager::loadEvents(this->_storyInfo.folder);

	//Initialize Random Info
	OptionsManager::displayLoading("Initializing randomization info...", true);
	CalcManager::loadRandomInfo(saveInfo.storyInfo.name);

	//Load Weapon skills
	OptionsManager::displayLoading("Initializing weapon skill info...", true);
	AttackManager::loadWeaponSkills(saveInfo.storyInfo.folder);

	//Entrance Event
	std::vector <Player*> players;
	for (int i = 0; i < this->_gamePlayers.size(); i++) {
		players.push_back(&this->_gamePlayers[i]);
	}
	if (this->_currentMap.getEntranceEventCode() != -1) {
		if (!this->isPastEvent(this->_currentMap.getEntranceEventCode()) || (JournalManager::getEvent(this->_currentMap.getEntranceEventCode())->recurring == true)) {
			JournalManager::enactEvent(this->_currentMap.getEntranceEventCode(), players);
			this->_pastEvents.push_back(this->_currentMap.getEntranceEventCode());
		}
	}

	//Begin Game
	clCons::cls();
	gameLoop();
}

//Loops & Menus
void GameEngine::optionsLoop()
{
	//Options menu variables
	bool hasReturned = false;
	int selection;

	//Options menu options
	std::vector<std::string> menuOptions;
	menuOptions.push_back("Background Color: " + OptionsManager::getColorOption());
	menuOptions.push_back("Fast Text: " + clUtil::boolToWord(OptionsManager::fastText, "on", "off"));
	menuOptions.push_back("Fast Movement: " + clUtil::boolToWord(OptionsManager::fastMovement, "on", "off"));
	menuOptions.push_back("Fast Combat: " + clUtil::boolToWord(OptionsManager::fastCombat, "on", "off"));
	menuOptions.push_back("Difficulty: " + OptionsManager::getDifficultyOption());
	menuOptions.push_back("Return");

	//Menu Loop
	while (!hasReturned) {
		clCons::cls();
		selection = ColorManager::menu(ColorFlag::GRAY, "[Options]", ColorFlag::GRAY, "", ColorFlag::GRAY, menuOptions, '-', '\033', menuOptions.size());
		switch (selection) {
		case 1: //Background Color
			OptionsManager::backgroundWhite = !OptionsManager::backgroundWhite;
			menuOptions[0] = "Background Color: " + OptionsManager::getColorOption();
			OptionsManager::updateColors();
			break;
		case 2: //Fast Text
			OptionsManager::fastText = !OptionsManager::fastText;
			menuOptions[1] = ("Fast Text: " + clUtil::boolToWord(OptionsManager::fastText, "on", "off"));
			break;
		case 3: //Fast Movement
			OptionsManager::fastMovement = !OptionsManager::fastMovement;
			menuOptions[2] = ("Fast Movement: " + clUtil::boolToWord(OptionsManager::fastMovement, "on", "off"));
			break;
		case 4: //Fast Combat
			OptionsManager::fastCombat = !OptionsManager::fastCombat;
			menuOptions[3] = ("Fast Combat: " + clUtil::boolToWord(OptionsManager::fastCombat, "on", "off"));
			break;
		case 5: //Difficulty
			OptionsManager::difficulty++;
			if (OptionsManager::difficulty > 2) {
				OptionsManager::difficulty = 0;
			}
			menuOptions[4] = "Difficulty: " + OptionsManager::getDifficultyOption();
			break;
		case 6: //Return
			hasReturned = true;
			break;
		}
	}

	OptionsManager::updateOptions();
	return;
}
void GameEngine::gameLoop()
{
	//Game Loop
	while (!this->_isOver) {
		//Update Entities on Map
		this->_currentMap.updateEntities(&this->_gameEnemies, &this->_gamePlayers);

		//Display Map
		display();

		//Take Entity Turns
		takeEntityTurns();
	}
}
void GameEngine::pauseMenu()
{
	//Variables
	int input;
	bool didReturn = false;
	bool hasSaved = false;
	std::vector<std::string> menuOptions;
	menuOptions.push_back("Character [C]");
	menuOptions.push_back("Inventory [B]");
	menuOptions.push_back("Weapon Skills [K]");
	menuOptions.push_back("Journal [J]");
	menuOptions.push_back("Return [Esc]");
	menuOptions.push_back("Save Game");
	menuOptions.push_back("Exit Game");

	while (!didReturn) {
		clCons::cls();
		
		//Grab Input
		bool finished = false;
		int selection = 1;
		char input;
		while (!finished) {
			if (clOption::USE_CLS_NEW) {
				clCons::clsNew();
			} else {
				clCons::cls();
			}
			clCons::centerVer(menuOptions.size() + 4);
			ColorManager::centerHorColor(ColorString("[Pause Menu]", ColorFlag::GRAY, true, 0, 5000, true), '-', 1);
			cout << endl;
			for (int i = 0; i < menuOptions.size(); i++) {
				if (selection == i + 1) {
					ColorManager::centerHorColor(ColorString(">- " + menuOptions[i] + " -<", ColorFlag::GRAY, true, 0));
				} else {
					clCons::centerHor(menuOptions[i]);
				}
			}
			cout << endl << endl;
			ColorManager::centerHorColor(ColorString("", ColorFlag::GRAY, true, 0, 5000, true), '-', 0);

			input = _getch();
			if (input == 13 || input == 32) {
				finished = true;
			} else if (input == 'w' || input == 'W') { //UP
				if (selection > 1) {
					selection--;
				} else {
					selection = menuOptions.size();
				}
			} else if (input == 's' || input == 'S') { //DOWN
				if (selection < menuOptions.size()) {
					selection++;
				} else {
					selection = 1;
				}
			} else if (input == 'c' || input == 'C') { //CHARACTER
				selection = 1;
				finished = true;
			} else if (input == 'b' || input == 'B') { //INVENTORY
				selection = 2;
				finished = true;
			} else if (input == 'k' || input == 'K') { //SKILLS
				selection = 3;
				finished = true;
			} else if (input == 'j' || input == 'J') { //JOURNAL
				selection = 4;
				finished = true;
			} else if (input == '\033') { //RETURN
				selection = 5;
				finished = true;
			} 
		}

		//Parse Input
		switch (selection) {
		case 1: // CHARACTER
			this->_gamePlayers[this->_entityTurn].openCharacter();
			break;
		case 2: // INVENTORY
			this->_gamePlayers[this->_entityTurn].openInventory();
			break;
		case 3: // WEAPON SKILLS
			AttackManager::openWeaponSkills(&this->_gamePlayers[this->_entityTurn]);
			break;
		case 4: //JOURNAL
			JournalManager::openJournal(this->getJournalCodes());
			break;
		case 5: // RETURN
			didReturn = true;
			break;
		case 6: // SAVE GAME
			saveGame();
			hasSaved = true;
			break;
		case 7: // EXIT GAME
			if (!hasSaved) {
				promptSave();
			}
			this->_isOver = true;
			didReturn = true;
			break;
		}
	}
}

//Entity Turn Taking
void GameEngine::takeEntityTurns()
{
	for (_entityTurn = 0; _entityTurn < this->_gamePlayers.size() + this->_gameEnemies.size(); _entityTurn++) {
		if (!this->_isOver) {
			if (_entityTurn < this->_gamePlayers.size()) { //PLAYERS

				//Display Turn
				display(true);
				takePlayerTurn();

				//Take Turn
				if (!this->_isOver) {
					_gamePlayers[_entityTurn].passiveUpdate();
					this->_currentMap.updateEntities(&this->_gameEnemies, &this->_gamePlayers);
					display(true);
				}
			} else { //ENEMIES
				
				if (_gameEnemies[_entityTurn - this->_gamePlayers.size()].map == this->_currentMap.getName()) {

					//Display Turn
					display(true);

					//Take Turn
					if (!OptionsManager::fastMovement) {
						std::this_thread::sleep_for(std::chrono::milliseconds(200));
					}

					//Calculate Effective Enemies
					_effectiveEnemies = 0;
					for (int i = 0; i < this->_gameEnemies.size(); i++) {
						if (this->_gameEnemies[i].map == this->_currentMap.getName()) {
							_effectiveEnemies++;
						}
					}

					//Take Turn
					takeEnemyTurn();
					
					//Account for possible enemy loss
					int newEnemyCount = 0;
					for (int i = 0; i < this->_gameEnemies.size(); i++) {
						if (this->_gameEnemies[i].map == this->_currentMap.getName()) {
							newEnemyCount++;
						}
					}
					if (newEnemyCount + 1 == this->_effectiveEnemies) {
						this->_entityTurn--;
						this->_effectiveEnemies--;
					}

					//Update
					this->_currentMap.updateEntities(&this->_gameEnemies, &this->_gamePlayers);
					display(true);
				}				
			}
		} 
	}
}
void GameEngine::takePlayerTurn()
{
	//Variables
	bool turnTaken = false;

	while (!turnTaken) {
		//Grab Input
		char input = _getch();

		//Act On Input
		switch (input) {
		case 27: //ESCAPE -> EXIT GAME
			pauseMenu();
			clCons::cls();
			display(true);
			break;
		case 'w': //w - MOVE PLAYER UP
		case 'W':
			handleCollision(this->_gamePlayers[_entityTurn].moveEntity(this->_currentMap.getEntityLayout(), Movement::UP, this->_gateLocs), &turnTaken);
			break;
		case 's': //s - MOVE PLAYER DOWN
		case 'S':
			handleCollision(this->_gamePlayers[_entityTurn].moveEntity(this->_currentMap.getEntityLayout(), Movement::DOWN, this->_gateLocs), &turnTaken);
			break;
		case 'a': //a - MOVE PLAYER LEFT
		case 'A':
			handleCollision(this->_gamePlayers[_entityTurn].moveEntity(this->_currentMap.getEntityLayout(), Movement::LEFT, this->_gateLocs), &turnTaken);
			break;
		case 'd': //d - MOVE PLAYER RIGHT
		case 'D':
			handleCollision(this->_gamePlayers[_entityTurn].moveEntity(this->_currentMap.getEntityLayout(), Movement::RIGHT, this->_gateLocs), &turnTaken);
			break;
		case 'b': //b - OPEN INVENTORY
		case 'B':
			this->_gamePlayers[this->_entityTurn].openInventory();
			break;
		case 'c': //c - OPEN CHARACTER
		case 'C':
			this->_gamePlayers[this->_entityTurn].openCharacter();
			break;
		case 'k': //k - OPEN WEAPON SKILLS
		case 'K':
			AttackManager::openWeaponSkills(&this->_gamePlayers[this->_entityTurn]);
			break;
		case 'j': //j - OPEN JOURNAL
		case 'J':
			JournalManager::openJournal(this->getJournalCodes());
			break;
		default:
			break;
		}
		if (this->_isOver)
			break;
		
		clCons::cls();
		display(true);
	}
}
void GameEngine::takeEnemyTurn()
{
	//Check enemy is on map
	if (this->_gameEnemies[this->_entityTurn - this->_gamePlayers.size()].map != this->_currentMap.getName()) {
		return;
	}

	//Variables
	int j = this->_entityTurn - this->_gamePlayers.size();
	int toWhichPlayer = 0;
	int movesLeft = this->_gameEnemies[j].enemy.getSpeed();
	bool battleHasNotOccured = true;

	//Determine Agro
	this->_gameEnemies[j].enemy.setAgro(false);
	for (int i = 0; i < this->_gamePlayers.size(); i++) {
		if (this->_gamePlayers[i].getX() >= (this->_gameEnemies[j].enemy.getX() - this->_gameEnemies[j].enemy.getAgroDistance()) && this->_gamePlayers[i].getX() <= (this->_gameEnemies[j].enemy.getX() + this->_gameEnemies[j].enemy.getAgroDistance())) { //WITHIN X DISTANCE
			if (this->_gamePlayers[i].getY() >= (this->_gameEnemies[j].enemy.getY() - this->_gameEnemies[j].enemy.getAgroDistance()) && this->_gamePlayers[i].getY() <= (this->_gameEnemies[j].enemy.getY() + this->_gameEnemies[j].enemy.getAgroDistance())) { //WITHIN Y DISTANCE
				this->_gameEnemies[j].enemy.setAgro(true);
				toWhichPlayer = i;
			}
		}
	}

	//Move Towards Player If Agroed
	if (this->_gameEnemies[j].enemy.getAgroed()) {
		while (movesLeft > 0 && battleHasNotOccured) {
			//Determine Direction to Agro
			int xDistance = this->_gamePlayers[toWhichPlayer].getX() - this->_gameEnemies[j].enemy.getX();
			int yDistance = this->_gamePlayers[toWhichPlayer].getY() - this->_gameEnemies[j].enemy.getY();

			//Battle if close enough
			if (clMath::abVal(xDistance) < 2 && clMath::abVal(yDistance) < 2) {
				std::vector<Player*> playerParticipants;
				std::vector<EnemyInfo*> enemyParticipants;
				getBattleParticipants(this->_gamePlayers[toWhichPlayer].getX(), this->_gamePlayers[toWhichPlayer].getY(), &playerParticipants, &enemyParticipants, true);
				if (!battle(playerParticipants, enemyParticipants, true)) 
					this->_isOver = true;	
				battleHasNotOccured = false;
			}

			//Move In Ideal Direction
			else if (clMath::abVal(xDistance) > clMath::abVal(yDistance)) { //Move in X
				if (xDistance < 0) { //Move Left
					this->_gameEnemies[j].enemy.moveEntity(this->_currentMap.getEntityLayout(), Movement::LEFT, this->_gateLocs);
				} else { //Move Right
					this->_gameEnemies[j].enemy.moveEntity(this->_currentMap.getEntityLayout(), Movement::RIGHT, this->_gateLocs);
				}
			} else { //Move in Y
				if (yDistance < 0) { //Move Up
					this->_gameEnemies[j].enemy.moveEntity(this->_currentMap.getEntityLayout(), Movement::UP, this->_gateLocs);
				} else { //Move Down
					this->_gameEnemies[j].enemy.moveEntity(this->_currentMap.getEntityLayout(), Movement::DOWN, this->_gateLocs);
				}
			}
			movesLeft--;
		}
		
	//Move Randomly Otherwise
	} else {
		bool validMove = false;
		while (!validMove) {
			Collision result = this->_gameEnemies[j].enemy.moveEntity(this->_currentMap.getEntityLayout(), Movement(CalcManager::getRandomNumber(4) - 1), this->_gateLocs);
			validMove = (result.collidedTile == ' ');
		}
	}
}
void GameEngine::removeDeadEnemies()
{
	for (int i = _gamePlayers.size(); (i - _gamePlayers.size()) < this->_gameEnemies.size(); i++) {
		if (this->_gameEnemies[i - _gamePlayers.size()].enemy.getHealth() < 1) {
			if (i < this->_entityTurn) {
				this->_entityTurn--;
			}
			this->_gameEnemies.erase(this->_gameEnemies.begin() + (i - _gamePlayers.size()));
			i--;
		}
	}
}

//Battle Functions
bool GameEngine::battle(std::vector<Player*> players, std::vector<EnemyInfo*> enemies, bool enemyAttackedFirst)
{
	//Clear Screen
	clCons::cls();

	//Variables
	std::vector<ColorString> leftDisplay;
	std::vector<ColorString> rightDisplay;
	std::vector<Item> drops;
	std::vector<EnemyInfo*> deadEnemies;
	char input = 0;
	int turn = 0;
	int attackSelection = 0;
	int result = 0; //1 - ENEMIES WON & 2 - PLAYERS WON
	int playerTarget = 0;
	int enemyTarget = 0;
	int sectionWidth = clCons::getConsoleWidth() / 3;
	int middleWidth = clCons::getConsoleWidth() - (sectionWidth * 2);
	int enemySize = enemies.size();
	if (enemyAttackedFirst)
		turn = players.size();

	//Battle Loop
	while (result == 0) {

		//Update Variables
		bool turnTaken = false;
		std::vector<std::string> attackOptions;
		std::vector<IndicatorInfo> indicators = std::vector<IndicatorInfo>();
		if (enemySize != enemies.size()) {
			playerTarget = 0;
			enemySize = enemies.size();
			clCons::cls();
		}

		//Set Enemy Target
		setEnemyTarget(players, &enemyTarget);

		//Display
		displayBattleState(&turn, &enemyTarget, &playerTarget, &attackSelection, players, enemies, &attackOptions);	
		
		//Take Player Turn
		if (turn < players.size()) {
			input = _getch();

			switch (input) {
			case 72: //UP ARROW
				playerTarget--;
				if (playerTarget < 0)
					playerTarget = enemies.size() - 1;
				break;
			case 80: //DOWN ARROW
				playerTarget++;
				if (playerTarget >= enemies.size())
					playerTarget = 0;
				break;
			case 'w':
			case 'W': //W
				attackSelection--;
				if (attackSelection < 0) {
					attackSelection = attackOptions.size() - 1;
				}
				break;
			case 's':
			case 'S': //S
				attackSelection++;
				if (attackSelection >= attackOptions.size()) {
					attackSelection = 0;
				}
				break;
			case 13: //ENTER
			case 32: //SPACE
				if (attackOptions[attackSelection][attackOptions[attackSelection].length() - 2] == '0') { //IF OPTION ISN'T ON COOLDOWN
					std::vector<Enemy*> enemyPtrs;
					for (int i = 0; i < enemies.size(); i++) {
						enemyPtrs.push_back(&enemies[i]->enemy);
					}
					AttackManager::applyWeaponSkill(&indicators, players, turn, enemyPtrs, playerTarget, attackSelection);
					turnTaken = true;
					AttackManager::updateCooldowns(players[turn]);
				}
				break;
			};
		}

		//Take Enemy Turn
		if (!(turn < players.size())) {
			for (int i = 0; i < enemies[turn - players.size()]->enemy.getDots().size(); i++) {
				indicators.push_back(IndicatorInfo(true, ColorString("-" + to_string(enemies[turn - players.size()]->enemy.getDots()[i].dmg), ColorFlag::RED, 0, 10000, false), true, ColorString("", ColorFlag::GRAY, true)));
			}
			if (enemies[turn - players.size()]->enemy.getStunned() == 0) {
				int dmg = enemies[turn - players.size()]->enemy.getDamage();
				players[enemyTarget]->takeDamage(dmg);
				indicators.push_back(IndicatorInfo(false, ColorString("-" + to_string(dmg), ColorFlag::RED, 0, 10000, true), true, ColorString(enemies[turn - players.size()]->enemy.getName() + " attacks " + players[enemyTarget]->getName() + " for " + to_string(dmg) + " damage!", ColorFlag::RED, 0, 10000, true)));
			}
			enemies[turn - players.size()]->enemy.updateEffects();
			turnTaken = true;
		}

		//Indicate
		for (int i = 0; i < indicators.size(); i++) {
			if (indicators[i].displayIndicator && !OptionsManager::fastCombat) {
				displayBattleState(&turn, &enemyTarget, &playerTarget, &attackSelection, players, enemies, &attackOptions);
				AttackManager::displayIndicators(indicators[i]);
			}
		}

		//Check for Deaths
		for (int i = 0; i < players.size(); i++) {
			if (players[i]->getHealth() == 0) {
				players.erase(players.begin() + i);
				if (turn >= i) { turn--; }
			}
		}
		for (int i = 0; i < enemies.size(); i++) {
			if (enemies[i]->enemy.getHealth() == 0) {
				deadEnemies.push_back(enemies[i]);
				enemies.erase(enemies.begin() + i);
				if (turn >= i + players.size()) { turn--; }
			}
		}
		if (players.size() == 0) {
			result = 1;
		}
		if (enemies.size() == 0) {
			result = 2;
		}

		//Update Turn
		if (turnTaken) {
			turn++;
			attackSelection = 0;
			if (turn >= players.size() + enemies.size())
				turn = 0;
		}
	}

	clCons::cls();

	//Post-Battle
	if (result == 2) { //PLAYERS WON

		//Create Post-Battle Notifications
		std::vector<std::string> pbn;
		
		pbn.push_back("");
		for (int i = 0; i < players.size(); i++) {
			for (int j = 0; j < deadEnemies.size(); j++) {
				for (int k = 0; k < deadEnemies[j]->enemy.getDrops().size(); k++) {
					if (CalcManager::getRandomNumber(100) <= deadEnemies[j]->enemy.getDrops()[k].chance) {
						pbn.push_back(players[i]->getName() + " loots [" + deadEnemies[j]->enemy.getDrops()[k].dropItem.getName() + "] x" + to_string(deadEnemies[j]->enemy.getDrops()[k].dropItem.getQuantity()));
						players[i]->addItemToInventory(deadEnemies[j]->enemy.getDrops()[k].dropItem);
					}
				}
			}
		}

		//Exp Checks
		int runningExp = 0;
		for (int i = 0; i < deadEnemies.size(); i++) {
			float eliteness = (float)deadEnemies[i]->enemy.getMaxHealth() / (float)CalcManager::getAverageEnemyHealth(deadEnemies[i]->enemy.getLevel());
			runningExp += CalcManager::getExp(deadEnemies[i]->enemy.getLevel(), eliteness);
		}
		for (int i = 0; i < players.size(); i++) {
			pbn.push_back(players[i]->getName() + " has earned " + to_string(runningExp) + " exp!");
			int prevLevel = players[i]->getLevel();
			players[i]->addXp(runningExp);
			if (prevLevel != players[i]->getLevel()) {
				pbn.push_back(players[i]->getName() + " has leveled up (to level " + to_string(players[i]->getLevel()) + ")!");
			}
		}

		//Weapon Level Checks
		std::vector<std::string> wlcpbn = AttackManager::weaponLevelCheck(players);
		for (int i = 0; i < wlcpbn.size(); i++) {
			pbn.push_back(wlcpbn[i]);
		}
		pbn.push_back("");

		//Display
		clCons::centerVer(pbn.size() + 3);
		ColorManager::centerHorColor(ColorFlag::GREEN, true, "Victory!");
		clCons::centerHor("", '-');
		for (int i = 0; i < pbn.size(); i++) {
			if (!OptionsManager::fastText) {
				std::this_thread::sleep_for(std::chrono::milliseconds(250));
			}
			clCons::centerHor(pbn[i]);
		}
		clCons::centerHor("", '-');
		clCons::paus("");
		clCons::cls();
		this->removeDeadEnemies();
	} else if (result == 1) { //PLAYERS LOST
		clCons::centerVer(5);
		ColorManager::centerHorColor(ColorFlag::RED, true, "Failure!");	
		clCons::centerHor("", '-', 2);
		clCons::centerHor("You have died! You may reload to when you last saved.");
		std::cout << std::endl << std::endl;;
		clCons::centerHor("", '-');
		clCons::paus("");
	}
	return result - 1;
}
void GameEngine::displayBattleState(int* turn, int* enemyTarget, int* playerTarget, int* attackSelection, std::vector<Player*> players, std::vector<EnemyInfo*> enemies, std::vector<std::string>* attackOptions)
{
		//Variables
		int sectionWidth = clCons::getConsoleWidth() / 3;
		int middleWidth = clCons::getConsoleWidth() - (sectionWidth * 2);
		std::vector<ColorString> leftDisplay;
		std::vector<ColorString> rightDisplay;

		//Display Top
		clCons::clsNew();
		std::cout << std::endl;
		if (*turn >= players.size()) {
			ColorManager::centerHorColor(ColorString("[" + enemies[*turn - players.size()]->enemy.getName() + "]'s turn", ColorFlag::RED, 1, enemies[*turn - players.size()]->enemy.getName().length() + 1));
		} else {
			ColorManager::centerHorColor(ColorString("[" + players[*turn]->getName() + "]'s turn", ColorFlag::BLUE, 1, players[*turn]->getName().length() + 1));
		}
		std::cout << std::endl << std::endl;

		//Set Left Display
		for (int i = 0; i < players.size(); i++) {
			//NAME
			std::string prefix = "    ", suffix = "    ";
			if (*enemyTarget == i) {
				prefix = suffix = " ** ";
			}
			if (*turn == i) {
				prefix = "-=[ ";
				suffix = " ]=-";
			}
			std::string content = clCons::centerStringIn(sectionWidth, prefix + players[i]->getName() + suffix);
			leftDisplay.push_back(ColorString(content, ColorFlag::BLUE, content.length() - players[i]->getName().length() -4, content.length() - 4, true));

			//HEALTH BAR
			leftDisplay.push_back(players[i]->createHealthBar(sectionWidth - 1));
		}

		//Set Right Display
		for (int i = 0; i < enemies.size(); i++) {
			//NAME
			std::string prefix = "    ", suffix = "    ";
			if (*playerTarget == i) {
				prefix = suffix = " ** ";
			}
			if (*turn == i + players.size()) {
				prefix = "-=[ ";
				suffix = " ]=-";
			}
			std::string content = "";
			std::string conditions = "";
			if (enemies[i]->enemy.getStunned() > 0) 
				conditions += " (St. " + to_string(enemies[i]->enemy.getStunned()) + ")";
			for (int j = 0; j < enemies[i]->enemy.getDots().size(); j++) {	
				conditions += " (Dot. " + to_string(enemies[i]->enemy.getDots()[j].length) + ")";
			}
			content = clCons::centerStringIn(sectionWidth, prefix + enemies[i]->enemy.getName() + conditions + suffix);
			rightDisplay.push_back(ColorString(content, ColorFlag::RED, content.length() - (enemies[i]->enemy.getName().length() + conditions.length() + 4), content.length() - 4, true));

			//HEALTH BAR
			rightDisplay.push_back(enemies[i]->enemy.createHealthBar(sectionWidth - 1));
		}

		//Display Right & Left
		for (int i = 0; i < rightDisplay.size() || i < leftDisplay.size(); i++) {
			//LEFT
			if (i < leftDisplay.size()) {
				ColorManager::csout(leftDisplay[i]);
				for (int j = 0; j < sectionWidth - leftDisplay[i].length(); j++) {
					std::cout << " ";
				}
			} else {
				for (int j = 0; j < sectionWidth; j++) {
					std::cout << " ";
				}
			}

			//MIDDLE
			for (int j = 0; j < middleWidth; j++) {
				std::cout << " ";
			}

			//RIGHT
			if (i < rightDisplay.size()) {
				ColorManager::csout(rightDisplay[i]);
				for (int j = 0; j < sectionWidth - rightDisplay[i].length(); j++) {
					std::cout << " ";
				}
			} else {
				for (int j = 0; j < sectionWidth; j++) {
					std::cout << " ";
				}
			}
		}

		//Pre-Middle/Bottom Display		
		int middleHeight = 0;
		int bottomLines = 0;
		if (*turn < players.size()) {
			*attackOptions = AttackManager::getWeaponSkillsList(players[*turn]);
			bottomLines = attackOptions->size() + 2;
		}
		if (leftDisplay.size() > rightDisplay.size()) {
			middleHeight = clCons::getConsoleHeight() - (4 + leftDisplay.size()) - bottomLines;
		} else {
			middleHeight = clCons::getConsoleHeight() - (4 + rightDisplay.size()) - bottomLines;
		}

		//Display Middle
		for (int i = 0; i < middleHeight; i++) {
			std::cout << std::endl;
		}

		//Display Bottom (PLAYER TURN)
		if (*turn < players.size()) {
			ColorManager::csout(ColorString("Choose an attack option:", ColorFlag::GRAY, 0, 10000, true));
			std::cout << std::endl;
			for (int i = 0; i < attackOptions->size(); i++) {
				std::string attackPrefix = "    ", attackSuffix = "    ";
				if (*attackSelection == i) {
					attackPrefix = "  -[";
					attackSuffix = "]-  ";
				}
				std::cout << attackPrefix << (*attackOptions)[i] << attackSuffix << std::endl;
			}
			std::cout << std::endl;
			clCons::centerHor("[Enter - Attack]-[W/S - Change Attack Option]-[Arrow Keys - Change Target Enemy]", '-');
		} 
}
void GameEngine::setEnemyTarget(std::vector<Player*> players, int* enemyTarget)
{
	for (int i = 0; i < players.size(); i++) {
		if (OptionsManager::difficulty == 0) {				
			if (players[i]->getHealth() > players[*enemyTarget]->getHealth()) {
				*enemyTarget = i;
			}
		} else {
			if (players[i]->getHealth() < players[*enemyTarget]->getHealth()) {
				*enemyTarget = i;
			}
		}
	}
}

//Miscellaneous
void GameEngine::display(bool displayExtras)
{
	//Do normal display
	if (!this->_isOver) {
		this->_currentMap.displayMap(true, &this->_gameEnemies);

		if (displayExtras) {
			if (_entityTurn < this->_gamePlayers.size()) {
				clCons::centerHor("[" + to_string(_gamePlayers[_entityTurn].getLevel()) + "] " + _gamePlayers[_entityTurn].getName() + "'s turn");
				ColorManager::centerHorColor(_gamePlayers[_entityTurn].createHealthBar((float)clCons::getConsoleWidth() * ((float)3 / (float)4)));
			} else {
				clCons::centerHor("[" + to_string(_gameEnemies[_entityTurn - this->_gamePlayers.size()].enemy.getLevel()) + "] " + _gameEnemies[_entityTurn - this->_gamePlayers.size()].enemy.getName() + "'s turn");
				ColorManager::centerHorColor(_gameEnemies[_entityTurn - this->_gamePlayers.size()].enemy.createHealthBar((float)clCons::getConsoleWidth() * ((float)3 / (float)4)));
			}
		}
	}
}
void GameEngine::handleCollision(Collision collision, bool* turnSpent)
{
	//Gate Collisions
	bool specialCollide = false;
	std::vector<Gate> gateTiles = this->_currentMap.getGates();
	for (int i = 0; i < gateTiles.size(); i++) {
		for (int j = 0; j < gateTiles[i].location.size(); j++) {
			if ((collision.tileLocation.x == gateTiles[i].location[j].x) && (collision.tileLocation.y == gateTiles[i].location[j].y)) {
				specialCollide = true;
				travelToMap(gateTiles[i].map, gateTiles[i].nextMapLocation);
			}
		}
	}

	//Enemy Collisions
	for (int i = 0; i < this->_gameEnemies.size(); i++) {
		if (collision.tileLocation.x == this->_gameEnemies[i].enemy.getX() && collision.tileLocation.y == this->_gameEnemies[i].enemy.getY() && this->_currentMap.getName() == this->_gameEnemies[i].map) {
			specialCollide = true;
			std::vector<Player*> playerParticipants;
			std::vector<EnemyInfo*> enemyParticipants;
			getBattleParticipants(collision.tileLocation.x, collision.tileLocation.y, &playerParticipants, &enemyParticipants, false);
			if (!battle(playerParticipants, enemyParticipants)) 
				this->_isOver = true;
			*turnSpent = true;
			clCons::cls();
		}
	}

	//Wall Collisions
	if (collision.collidedTile != ' ' && collision.movesMade == 0 && !specialCollide) {
		*turnSpent = false;
	} 
	
	//Air Collisions
	else {
		*turnSpent = true;
	}
}
void GameEngine::travelToMap(std::string mapFileSource, std::vector<Coord> locations)
{
	//Exit Event
	std::vector <Player*> players;
	for (int i = 0; i < this->_gamePlayers.size(); i++) {
		players.push_back(&this->_gamePlayers[i]);
	}
	if (this->_currentMap.getExitEventCode() != -1) {
		if (!this->isPastEvent(this->_currentMap.getExitEventCode()) || (JournalManager::getEvent(this->_currentMap.getExitEventCode())->recurring == true)) {
			JournalManager::enactEvent(this->_currentMap.getExitEventCode(), players);
			this->_pastEvents.push_back(this->_currentMap.getExitEventCode());
		}
	}

	//Variables
	bool archiveMap = false;
	
	//Save current map for later usage
	this->_archiveMaps.push_back(this->_currentMap);

	//Check if map is in archives
	for (int i = 0; i < this->_archiveMaps.size(); i++) {
		if (mapFileSource == this->_archiveMaps[i].getFileSource()) {
			archiveMap = true;
			this->_currentMap = this->_archiveMaps[i];
			this->_archiveMaps.erase(this->_archiveMaps.begin() + i);
			this->_currentMap.setStartingPositions(locations);
			this->_currentMap.setPlayerStartingPositions(&this->_gamePlayers);
		}
	}

	if (!archiveMap) {
		//Load new map
		this->_currentMap = Map(mapFileSource, this->_storyInfo.name);

		//Set player positions
		this->_currentMap.setStartingPositions(locations);
		this->_currentMap.setPlayerStartingPositions(&this->_gamePlayers);

		//Load Enemies
		bool loadMoreEnemies = true;
		for (int i = 0; i < this->_gameEnemies.size(); i++) {
			if (this->_gameEnemies[i].map == this->_currentMap.getName()) {
				loadMoreEnemies = false;
			}
		}
		if (loadMoreEnemies) {
			float averageLevel = 0;
			for (int i = 0; i < this->_gamePlayers.size(); i++) {
				averageLevel += this->_gamePlayers[i].getLevel();
			}
			averageLevel /= this->_gamePlayers.size();
			std::vector<EnemyInfo> nextEnemies = _currentMap.loadMapEnemies((int)averageLevel);
			for (int i = 0; i < nextEnemies.size(); i++) {
				this->_gameEnemies.push_back(nextEnemies[i]);
			}
		} else {
			this->_currentMap.removeDefaultEnemyTiles();
		}
	}

	//Entrance Event
	if (this->_currentMap.getEntranceEventCode() != -1) {
		if (!this->isPastEvent(this->_currentMap.getEntranceEventCode()) || (JournalManager::getEvent(this->_currentMap.getEntranceEventCode())->recurring == true)) {
			JournalManager::enactEvent(this->_currentMap.getEntranceEventCode(), players);
			this->_pastEvents.push_back(this->_currentMap.getEntranceEventCode());
		}
	}

	//Update Gate Positions
	this->_gateLocs = this->_currentMap.getGatePositions();

	//Clear Screen
	clCons::cls();
}
void GameEngine::getBattleParticipants(int x, int y, std::vector<Player*>* players, std::vector<EnemyInfo*>* enemies, bool enemyAttacked)
{
	//Get all players
	for (int i = 0; i < this->_gamePlayers.size(); i++) {
		if (this->_gamePlayers[i].getX() >= x - 1 && this->_gamePlayers[i].getX() <= x + 1) {
			if (this->_gamePlayers[i].getY() >= y - 1 && this->_gamePlayers[i].getY() <= y + 1) {
				(*players).push_back(&this->_gamePlayers[i]);
			}
		}
	}

	//Get all enemies
	for (int i = 0; i < this->_gameEnemies.size(); i++) {
		if (this->_gameEnemies[i].map == this->_currentMap.getName()) {
			if (this->_gameEnemies[i].enemy.getX() >= x - 1 && this->_gameEnemies[i].enemy.getX() <= x + 1) {
				if (this->_gameEnemies[i].enemy.getY() >= y - 1 && this->_gameEnemies[i].enemy.getY() <= y + 1) {
					(*enemies).push_back(&this->_gameEnemies[i]);
				}
			}
		}
	}
}
bool GameEngine::isPastEvent(int eventCode)
{
	this->_pastEvents;
	for (int i = 0; i < this->_pastEvents.size(); i++) {
		if (this->_pastEvents[i] == eventCode) {
			return true;
		}
	}
	return false;
}
std::vector<int> GameEngine::getJournalCodes()
{
	std::vector<int> eventCodes;
	for (int i = 0; i < this->_pastEvents.size(); i++) {
		if (JournalManager::getEvent(this->_pastEvents[i])->journalEntry) {
			eventCodes.push_back(this->_pastEvents[i]);
		}
	}
	return eventCodes;
}