#pragma once
#include <vector>
#include <thread>
#include <chrono>
#include "map.h"
#include "entity.h"
#include "player.h"
#include "optionsManager.h"
#include "saveManager.h"
#include "storyManager.h"
#include "calcManager.h"
#include "colorManager.h"
#include "attackManager.h"

const static int BUILD_NUMBER = 483;

class GameEngine
{
public:

	//Functions
	void init();

private:

	//Variables
	std::string _saveName;
	std::vector<Map> _archiveMaps;
	std::vector<EnemyInfo> _gameEnemies;
	std::vector<Player> _gamePlayers;
	std::vector<Coord> _gateLocs;
	std::vector<int> _pastEvents;
	StoryInfo _storyInfo;
	Map _currentMap;
	bool _isOver = false;
	bool _hasQuit = false;
	int _entityTurn;
	int _effectiveEnemies = 0;

	//Save Functions
	void saveGame();
	void promptSave();
	void implementSave(SaveInfo saveInfo);

	//Loops & Menus
	void optionsLoop();
	void gameLoop();
	void pauseMenu();

	//Entity Turn Taking
	void takeEntityTurns();
	void takePlayerTurn();
	void takeEnemyTurn();
	void removeDeadEnemies();

	//Battle Functions
	void displayBattleState(int* turn, int* enemyTarget, int* playerTarget, int* attackSelection, std::vector<Player*> players, std::vector<EnemyInfo*> enemies, std::vector<std::string>* attackOptions);
	bool battle(std::vector<Player*> players, std::vector<EnemyInfo*> enemies, bool enemyAttackedFirst = false); //Returns True -> Battle Won
	void setEnemyTarget(std::vector<Player*> players, int* enemyTarget);
	
	//Miscellaneous
	void display(bool displayExtras = false);
	void handleCollision(Collision collision, bool* turnSpent);
	void travelToMap(std::string mapFileSource, std::vector<Coord> locations);
	void getBattleParticipants(int x, int y, std::vector<Player*>* players, std::vector<EnemyInfo*>* enemies, bool enemyAttacked);
	bool isPastEvent(int eventCode);
	std::vector<int> getJournalCodes();
};