
#ifndef Map_h
#define Map_h

//Includes
#include <vector>
#include <string>
#include <fstream>
#include <vector>
#include <Windows.h>
#include <Clfram.h>
#include <entity/Entity.h>
#include <entity/Player.h>
#include <entity/Enemy.h>
#include <managers/OptionsManager.h>
#include <managers/StoryManager.h>
#include <managers/ColorManager.h>

//Gate Struct
struct Gate {

	//Data
	std::vector<Coordinate> nextMapLocation;
	std::vector<Coordinate> location;
	char symbol;
	std::string map;
};

//Enemy Info Struct
struct EnemyInfo {

	//Node Conversion Functions
	Node convertToNode();
	static EnemyInfo createEnemyInfoFromNode(Node* node);

	//Data
	Enemy enemy;
	std::string map;
};

//Map Class
class Map {
public:

	//Constructors
	Map() {};
	Map(std::string fileSource, std::string storyName);
	void construct(std::string fileSource, std::string storyName);

	//Other Functions
	void loadMapInfo();
	void displayMap(bool withEntities = true, std::vector<EnemyInfo> *enemies = new std::vector<EnemyInfo>());
	void updateEntities(std::vector<EnemyInfo> *enemies, std::vector<Player> *players);
	void setPlayerStartingPositions(std::vector<Player> *players);
	void removeDefaultEnemyTiles();
	std::vector<Coordinate> stringToCoordinates(std::string these);
	std::vector<EnemyInfo> loadMapEnemies(int playerLevel);

	//Mutators
	void setStartingPositions(std::vector<Coordinate> startingPositions) { this->_startingPositions = startingPositions; }

	//Accessors
	char getPos(int x, int y) { return this->_entityLayout[y][x]; }
	std::vector<std::string>* getEntityLayout() { return &this->_entityLayout; }
	std::vector<Gate> getGates() { return this->_gates; }
	std::vector<Coordinate> getGatePositions();
	std::string getFileSource() { return this->_fileSource; }
	std::string getName() { return this->_name; }
	int getEntranceEventCode() { return this->_entranceEventCode; }
	int getExitEventCode() { return this->_exitEventCode; }

private:

	//Collection Data
	std::vector<Coordinate> _playerPos;
	std::vector<std::string> _layout;
	std::vector<std::string> _entityLayout;
	std::vector<Coordinate> _startingPositions;
	std::vector<Gate> _gates;

	//Other Data
	std::string _name;
	std::string _fileSource;
	std::string _storyName;
	int _width;
	int _height;
	int _entranceEventCode = -1, _exitEventCode = -1;
	char _space = ' ';
};

#endif