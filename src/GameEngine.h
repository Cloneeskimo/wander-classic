
#ifndef GameEngine_h
#define GameEngine_h

//Includes
#include <vector>
#include <thread>
#include <chrono>
#include <Map.h>
#include <entity/Entity.h>
#include <entity/Player.h>
#include <managers/OptionsManager.h>
#include <managers/SaveManager.h>
#include <managers/StoryManager.h>
#include <managers/CalcManager.h>
#include <managers/ColorManager.h>
#include <managers/AttackManager.h>

//Build Number
const static int BUILD_NUMBER = 490;

//GameEngine Class
class GameEngine {
public:

	//Functions
	void init();

private:

	//Collection Data
	std::vector<Map> _archiveMaps;
	std::vector<EnemyInfo> _gameEnemies;
	std::vector<Player> _gamePlayers;
	std::vector<Coordinate> _gateLocs;
	std::vector<int> _pastEvents;

	//Other Data
	std::string _saveName;
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

	//Miscellaneous Functions
	void display(bool displayExtras = false);
	void handleCollision(Collision collision, bool* turnSpent);
	void travelToMap(std::string mapFileSource, std::vector<Coordinate> locations);
	void getBattleParticipants(int x, int y, std::vector<Player*>* players, std::vector<EnemyInfo*>* enemies, bool enemyAttacked);
	bool isPastEvent(int eventCode);
	std::vector<int> getJournalCodes();
};

#endif