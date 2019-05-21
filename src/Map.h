#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <vector>
#include <Windows.h>
#include "clfram.h"
#include "entity.h"
#include "player.h"
#include "enemy.h"
#include "optionsManager.h"
#include "storyManager.h"
#include "colorManager.h"

//Structs
struct Gate
{
	std::vector<Coord> nextMapLocation;
	std::vector<Coord> location;
	char symbol;
	std::string map;
};
struct EnemyInfo
{
	Enemy enemy;
	std::string map;
};

class Map
{
public:
	//Constructor
	Map() {};
	Map(std::string fileSource, std::string storyName);
	void construct(std::string fileSource, std::string storyName);

	//Functions
	void loadMapInfo();
	void displayMap(bool withEntities = true, std::vector<EnemyInfo> *enemies = new std::vector<EnemyInfo>());
	void updateEntities(std::vector<EnemyInfo> *enemies, std::vector<Player> *players);
	void setPlayerStartingPositions(std::vector<Player> *players);
	void removeDefaultEnemyTiles();
	std::vector<Coord> stringToCoords(std::string these);
	std::vector<EnemyInfo> loadMapEnemies(int playerLevel);

	//Setters
	void setStartingPositions(std::vector<Coord> startingPositions) { this->_startingPositions = startingPositions; }

	//Getters
	char getPos(int x, int y) { return this->_entityLayout[y][x]; }
	std::vector<std::string>* getEntityLayout() { return &this->_entityLayout; }
	std::vector<Gate> getGates() { return this->_gates; }
	std::vector<Coord> getGatePositions();
	std::string getFileSource() { return this->_fileSource; }
	std::string getName() { return this->_name; }
	int getEntranceEventCode() { return this->_entranceEventCode; }
	int getExitEventCode() { return this->_exitEventCode; }

private:

	//Variables
	std::string _name;
	std::string _fileSource;
	std::string _storyName;
	std::vector<Coord> _playerPos;
	std::vector<std::string> _layout;
	std::vector<std::string> _entityLayout;
	std::vector<Coord> _startingPositions;
	std::vector<Gate> _gates;
	int _width;
	int _height;
	int _entranceEventCode = -1, _exitEventCode = -1;
	char _space = ' ';
};