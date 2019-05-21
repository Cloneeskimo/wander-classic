#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <stdio.h>
#include <chrono>
#include <thread>
#include "clfram.h"
#include "player.h"
#include "entity.h"
#include "map.h"
#include "item.h"
#include "colorManager.h"
#include "storyManager.h"
#include "journalManager.h"
#include "attackManager.h"

//Structs
struct SaveInfo
{
	StoryInfo storyInfo;
	Map currentMap;
	std::vector<EnemyInfo> currentEnemies;
	std::vector<Player> currentPlayers;
	std::vector<int> pastEvents;
	std::string saveName;
};

static class SaveManager
{
public:
	//Menus
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