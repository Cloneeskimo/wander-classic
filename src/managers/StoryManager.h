#pragma once
#include <vector>
#include <fstream>
#include <string>
#include "optionsManager.h"
#include "item.h"

//Structs
struct Level
{
	int level;
	int expNeeded;
};
struct StoryInfo
{
	std::string name;
	std::string folder;
	std::string startingMap;
	std::string author;
	std::vector<int> players;
	std::vector<std::string> mapNames;
	std::vector<Level> levels;
};

static class StoryManager
{
public:

	//Functions
	static StoryInfo loadStoryInfo(std::string storyName);
	static std::vector<std::string> getStoryList();
};