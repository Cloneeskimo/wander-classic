
#ifndef StoryManager_h
#define StoryManager_h

//Includes
#include <vector>
#include <fstream>
#include <string>
#include <Item.h>
#include <managers/OptionsManager.h>

//Level Struct
struct Level {
	int level;
	int expNeeded;
};

//StoryInfo Struct
struct StoryInfo {
	std::string name;
	std::string folder;
	std::string startingMap;
	std::string author;
	std::vector<int> players;
	std::vector<std::string> mapNames;
	std::vector<Level> levels;
};

//StoryManager Class
static class StoryManager {
public:

	//Functions
	static StoryInfo loadStoryInfo(std::string storyName);
	static std::vector<std::string> getStoryList();
};

#endif