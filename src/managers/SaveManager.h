
#ifndef SaveManager_h
#define SaveManager_h

//Includes
#include <string>
#include <vector>
#include <fstream>
#include <stdio.h>
#include <chrono>
#include <thread>
#include <Clfram.h>
#include <Map.h>
#include <Node.h>
#include <Item.h>
#include <entity/Player.h>
#include <entity/Entity.h>
#include <managers/ColorManager.h>
#include <managers/StoryManager.h>
#include <managers/JournalManager.h>
#include <managers/AttackManager.h>

//SaveInfo Struct
struct SaveInfo {
	StoryInfo storyInfo;
	Map currentMap;
	std::vector<EnemyInfo> currentEnemies;
	std::vector<Player> currentPlayers;
	std::vector<int> pastEvents;
	std::string saveName;
};

//SaveManager Class
static class SaveManager {
public:

	//Menu Functions
	static SaveInfo loadGame(bool *returned);
	static SaveInfo newGame(bool *returned);

	//Save Functions
	static void save(SaveInfo info);
	static void deleteSave(std::string saveName);
	static std::vector<std::string> getListOfSaves();

private:

	//Private Save Functions
	static SaveInfo loadSave(std::string saveName);
	static void updateSavesFile(std::vector<std::string> newSavesList);
};

#endif