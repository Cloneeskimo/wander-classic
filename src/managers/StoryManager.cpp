#include "StoryManager.h"

//Functions
StoryInfo StoryManager::loadStoryInfo(std::string storyName) {
	
	//variables
	StoryInfo storyInfo{};
	std::ifstream read;
	std::string nextLine;
	int componentsRead = 0;

	//open file
	read.open("data//stories//" + storyName + "//settings.wstory");
	if (read.fail()) {
		OptionsManager::wError("Unable to open '" + storyName + "//settings.wstory'", "STORYMANAGER_H");
	}

	//parse line
	storyInfo.name = storyName;
	while (!read.eof()) {
		getline(read, nextLine);

		if (nextLine.find("author:") != std::string::npos) { //AUTHOR
			storyInfo.author = clFile::cut(nextLine);
		} else if (nextLine.find("folder:") != std::string::npos) { //FOLDER
			storyInfo.folder = clFile::cut(nextLine);
		} else if (nextLine.find("starting map:") != std::string::npos) { //STARTING MAP
			storyInfo.startingMap = clFile::cut(nextLine);
		} else if (nextLine.find("players:") != std::string::npos) { //PLAYERS
			for (int i = 0; i < nextLine.length(); i++) {
				if (nextLine[i] >= 49 && nextLine[i] <= 57) {
					storyInfo.players.push_back((nextLine[i]) - 48);
				}
			}
		}
	}

	//open levels file
	read.close();
	read.open("data//stories//" + storyName + "//levels.wtxt");
	if (read.fail()) {
		OptionsManager::wError("Unable to open 'levels.wtxt'", "STORYMANAGER_H");
	}

	//load levels
	storyInfo.levels = std::vector<Level>();
	int nextLevel, nextExp;
	while (!read.eof()) {
		nextLevel = stoi(clFile::cut(&read));
		nextExp = stoi(clFile::cut(&read));
		storyInfo.levels.push_back(Level{ nextLevel, nextExp });
	}

	return storyInfo;
}
std::vector<std::string> StoryManager::getStoryList() {
	
	//variables
	std::vector<std::string> storyList;
	std::string nextLine;
	std::ifstream read;

	//open file
	read.open("data//stories//stories.wtxt");
	if (read.fail()) {
		OptionsManager::wError("Unable to open 'stories.wtxt'", "STORYMANAGER_H", true);
	}

	//read stories
	while (!read.eof()) {
		getline(read, nextLine);
		if (nextLine != "{" && nextLine != "}" && nextLine != "" && nextLine != "stories:") {
			storyList.push_back(nextLine);
		}
	}

	//close file
	read.close();
	return storyList;
}