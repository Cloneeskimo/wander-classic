
#include "Map.h"

/* EnemyInfo */

//Node Conversion Functions
Node EnemyInfo::convertToNode() {

	//create and return node
	Node enemyInfo("enemy info", "enemy info");
	enemyInfo.addChild(this->enemy.convertToNode());
	enemyInfo.addChild(Node("map", this->map));
	return enemyInfo;
}
EnemyInfo EnemyInfo::createEnemyInfoFromNode(Node* node) {

	//create and return enemy info
	EnemyInfo enemyInfo;
	enemyInfo.enemy = Enemy::createEnemyFromNode(node->getChildByName("enemy"));
	enemyInfo.map = node->getChildByName("map")->getValue();
	return enemyInfo;
}

/* Map */

//Constructors
Map::Map(std::string fileSource, std::string storyName) {
	this->construct(fileSource, storyName);
}
void Map::construct(std::string fileSource, std::string storyName) {

	//set map info
	this->_fileSource = fileSource;
	this->_storyName = storyName;
	this->_width = this->_height = 0;

	//load map info
	loadMapInfo();
}

//Other Functions
void Map::loadMapInfo() {

	//variables
	std::string nextLine;
	ifstream read;

	//open file
	read.open("data//stories//" + StoryManager::loadStoryInfo(this->_storyName).folder + "//maps//" + _fileSource);
	if (read.fail()) {
		OptionsManager::wError("unable to open '" + this->_fileSource + "'", "MAP_H");
	}

	//parse line
	while (!read.eof()) {
		getline(read, nextLine);

		if (nextLine.find("layout:") != std::string::npos) { //LAYOUT
			for (int k = 0; nextLine != "}"; k++) {
				getline(read, nextLine);
				if (nextLine != "}" && nextLine != "{") {
					_layout.push_back(nextLine);
					if (nextLine.length() > this->_width) {
						this->_width = nextLine.length();
					}
				}
			}
			for (int l = 0; l < this->_layout.size(); l++) {
				while (_layout[l].length() < this->_width) {
					_layout[l] += ' ';
				}
			}
			this->_height = _layout.size();
		} else if (nextLine.find("entrance event:") != std::string::npos) {
			this->_entranceEventCode = stoi(clFile::cut(nextLine));
		} else if (nextLine.find("exit event:") != std::string::npos) {
			this->_exitEventCode = stoi(clFile::cut(nextLine));
		} else if (nextLine.find("space:") != std::string::npos) { //SPACE
			this->_space = clFile::cut(nextLine)[1];
		} else if (nextLine.find("map name:") != std::string::npos) { //NAME
			this->_name = clFile::cut(nextLine);
		} else if (nextLine.find("start:") != std::string::npos) { //START
			this->_startingPositions = stringToCoordinates(clFile::cut(nextLine));
		} else if (nextLine.find("gates:") != std::string::npos) { //GATES
			Gate nextGate{};

			//parse line
			while (nextLine != "}") {
				getline(read, nextLine);

				if (nextLine == "\t}") {
					this->_gates.push_back(nextGate);
				} else if (nextLine.find("tile:") != std::string::npos) { //TILE
					nextGate.symbol = clFile::cut(nextLine)[1];
				} else if (nextLine.find("map:") != std::string::npos) { //MAP
					nextGate.map = clFile::cut(nextLine);
				} else if (nextLine.find("at:") != std::string::npos) { //NEXT MAP LOCATION
					nextGate.nextMapLocation = stringToCoordinates(clFile::cut(nextLine));
				}
			}
		}
	}

	//set spaces
	for (int i = 0; i < this->_layout.size(); i++) {
		for (int j = 0; j < this->_layout[i].length(); j++) {
			if (this->_layout[i][j] == this->_space) {
				this->_layout[i][j] = ' ';
			}
			for (int k = 0; k < this->_gates.size(); k++) {
				if (this->_layout[i][j] == this->_gates[k].symbol) {
					this->_layout[i][j] = ' ';
					this->_gates[k].location.push_back(Coordinate{ j, i });
				}
			}
		}
	}

	//close file
	read.close();
}
void Map::displayMap(bool withEntities, std::vector<EnemyInfo> *enemies) {

	//clear screen
	clCons::clsNew();

	//calculate lines
	int emptyLines = clCons::getConsoleHeight() - this->_height - 2;
	int topEmptyLines = emptyLines / 2;

	//display
	for (int i = 0; i < topEmptyLines; i++)
		std::cout << std::endl;
	if (withEntities) {
		for (int j = 0; j < _entityLayout.size(); j++) {
			bool displayEachSpace = false;
			for (int k = 0; k < _playerPos.size(); k++) {
				if (_playerPos[k].y == j) {
					displayEachSpace = true;
				}
			}
			for (int k = 0; k < enemies->size(); k++) {
				if ((*enemies)[k].enemy.getY() == j) {
					displayEachSpace = true;
				}
			}
			if (displayEachSpace) {
				int beforeSpaces = (int)((clCons::getConsoleWidth() - (double)_entityLayout[j].size()) / 2);
				for (int k = 0; k < beforeSpaces; k++) {
					cout << ' ';
				}
				for (int k = 0; k < this->_entityLayout[j].length(); k++) {
					bool displayRegular = true;
					for (int l = 0; l < _playerPos.size(); l++) {
						if (j == _playerPos[l].y && k == _playerPos[l].x) {
							ColorManager::setForegroundColor(ColorFlag::BLUE, true);
							std::cout << this->_entityLayout[j][k];
							ColorManager::resetForegroundColor();
							displayRegular = false;
						}
					}
					for (int l = 0; l < enemies->size(); l++) {
						if ((j == (*enemies)[l].enemy.getY() && k == (*enemies)[l].enemy.getX()) && (*enemies)[l].map == this->_name) {
							if ((*enemies)[l].enemy.getAgroed()) {
								ColorManager::setForegroundColor(ColorFlag::RED, true);
							} else {
								ColorManager::setForegroundColor(ColorFlag::RED, false);
							}
							std::cout << this->_entityLayout[j][k];
							ColorManager::resetForegroundColor();
							displayRegular = false;
						}
					}
					if (displayRegular) {
						std::cout << this->_entityLayout[j][k];
					}
				}
				std::cout << std::endl;
			} else {
				clCons::centerHor(_entityLayout[j], ' ');
			}
		}
	} else {
		for (int j = 0; j < _layout.size(); j++)
			clCons::centerHor(_layout[j], ' ');
	}
	std::cout << std::endl << std::endl;
	ColorManager::centerHorColor(ColorFlag::GREEN, false, this->_name, ' ', 2);
}
void Map::updateEntities(std::vector<EnemyInfo> *enemies, std::vector<Player> *players) {
	
	//reset positions
	this->_playerPos = std::vector<Coordinate>();

	//load players
	this->_entityLayout = this->_layout;
	for (int i = 0; i < players->size(); i++) {
		this->_entityLayout[(*players)[i].getY()][(*players)[i].getX()] = (*players)[i].getSymbol();
		_playerPos.push_back(Coordinate{ (*players)[i].getX(), (*players)[i].getY() });
	}

	//load enemies
	for (int i = 0; i < enemies->size(); i++) {
		if ((*enemies)[i].map == this->_name) {
			this->_entityLayout[(*enemies)[i].enemy.getY()][(*enemies)[i].enemy.getX()] = (*enemies)[i].enemy.getSymbol();
		}
	}
}
void Map::setPlayerStartingPositions(std::vector<Player> *entities) {

	//assign starting positions
	for (int i = 0; i < entities->size(); i++) {
		(*entities)[i].setLocation(this->_startingPositions[i].x, this->_startingPositions[i].y);
	}
}
void Map::removeDefaultEnemyTiles() {

	//variables
	std::vector<char> enemyTiles;
	std::string nextLine;
	ifstream scan;

	//open file
	scan.open("data//stories//" + StoryManager::loadStoryInfo(this->_storyName).folder + "//maps//" + _fileSource);
	if (scan.fail()) {
		OptionsManager::wError("Unable to open " + _fileSource, "MAP_H");
	}

	//scan file
	while (!scan.eof()) {
		getline(scan, nextLine);

		//identify all enemy symbols
		if (nextLine.find("enemy symbol:") != std::string::npos) {
			bool exists = false;
			for (int i = 0; i < enemyTiles.size(); i++) {
				if (clFile::cut(nextLine)[1] == enemyTiles[i]) {
					exists = true;
				}
			}
			if (!exists) {
				enemyTiles.push_back(clFile::cut(nextLine)[1]);
			}
		}
	}

	for (int i = 0; i < this->_layout.size(); i++) {
		for (int j = 0; j < this->_layout[i].length(); j++) {
			for (int k = 0; k < enemyTiles.size(); k++) {
				if (this->_layout[i][j] == enemyTiles[k]) {
					this->_layout[i][j] = ' ';
				}
			}
		}
	}
}
std::vector<Coordinate> Map::stringToCoordinates(std::string these) {
	std::string nextNum;
	std::vector<Coordinate> Coordinates;
	int nextX, nextY;
	bool onY = false;
	for (int i = 0; i < these.length() + 1; i++) {
		if (these[i] >= 48 && these[i] <= 57) {
			nextNum.push_back(these[i]);
		} else if (these[i] == ',' || i == these.length()) {
			if (onY) {
				nextY = stoi(nextNum);
			} else {
				nextX = stoi(nextNum);
			}
			nextNum = "";
			onY = !onY;
			if (!onY) {
				Coordinates.push_back(Coordinate{ nextX, nextY });
			}
		}
	}
	return Coordinates;
}
std::vector<EnemyInfo> Map::loadMapEnemies(int playerLevel) {
	
	//variables
	std::string nextLine = "";
	std::ifstream read;
	bool framework = false;
	int currentLine = 0;

	//open file
	read.open("data//stories//" + StoryManager::loadStoryInfo(this->_storyName).folder + "//maps//" + _fileSource);
	if (read.fail()) {
		OptionsManager::wError("Unable to open '" + _fileSource + "'", "MAP_H");
	}

	//skip to enemy section
	while (!(nextLine.find("enemies:") != std::string::npos)) {
		getline(read, nextLine);
		currentLine++;
		if (read.eof()) {
			return std::vector<EnemyInfo>();
		}
	}

	//read enemies
	std::vector<EnemyInfo> enemies;
	EnemyRandomInfo nextEnemy = EnemyRandomInfo();
	Enemy newEnemy = Enemy();
	getline(read, nextLine);
	while (nextLine != "}") {
		getline(read, nextLine);
		if (!framework)
			currentLine++;

		//interpret lines
		if (nextLine == "\t{" || (nextLine == "{" && framework)) { //NEW ENEMY
			nextEnemy = EnemyRandomInfo();
		} else if ((nextLine == "\t}" && !framework) || (nextLine == "}" && framework)) { //FINISHED WITH ENEMY
			if (nextEnemy.symbol == -24)
				nextEnemy.symbol = nextEnemy.name[0];
			for (int i = 0; i < this->_layout.size(); i++) {
				for (int j = 0; j < this->_layout[i].length(); j++) {
					if (this->_layout[i][j] == nextEnemy.symbol) {
						newEnemy = createRandomEnemy(nextEnemy, playerLevel);
						newEnemy.setLocation(j, i);
						this->_layout[i][j] = ' ';
						enemies.push_back(EnemyInfo{ newEnemy, this->_name });
					}
				}
			}
			if (framework) {
				framework = false;
				read.close();
				read.open("data//stories//" + StoryManager::loadStoryInfo(this->_storyName).folder + "//maps//" + _fileSource);
				if (read.fail()) {
					OptionsManager::wError("Unable to open '" + _fileSource + "'", "MAP_H");
				}
				for (int i = 0; i < currentLine + 1; i++) {
					getline(read, nextLine);
				}
			}
		} else if (nextLine.find("enemy frame:") != std::string::npos) { //PRE-BUILT ENEMY FRAME
			framework = true;
			read.close();
			read.open("data//stories//" + StoryManager::loadStoryInfo(this->_storyName).folder + "//enemies//" + clFile::cut(nextLine));
			if (read.fail()) {
				OptionsManager::wError("Unable to open '" + clFile::cut(nextLine) + "'", "MAP_H");
			}
		} else if (nextLine.find("enemy name:") != std::string::npos) { //NAME
			nextEnemy.name = clFile::cut(nextLine);
		} else if (nextLine.find("enemy symbol:") != std::string::npos) { //SYMBOL
			if (clFile::cut(nextLine) != "ADAPT")
				nextEnemy.symbol = clFile::cut(nextLine)[1];
		} else if (nextLine.find("enemy health:") != std::string::npos) { //HEALTH
			if (clFile::cut(nextLine) != "ADAPT")
				nextEnemy.health = stoi(clFile::cut(nextLine));
		} else if (nextLine.find("enemy damage:") != std::string::npos) { //HEALTH
			if (clFile::cut(nextLine) != "ADAPT")
				nextEnemy.damage = stoi(clFile::cut(nextLine));
		} else if (nextLine.find("enemy eliteness:") != std::string::npos) { //ELITENESS
			if (clFile::cut(nextLine) != "ADAPT")
				nextEnemy.eliteness = stof(clFile::cut(nextLine));
		} else if (nextLine.find("enemy drops:") != std::string::npos) { //DROPS
			std::vector<std::string> dropInfo;
			while (!((nextLine == "\t\t}" && !framework) || (framework && nextLine == "\t}"))) {
				getline(read, nextLine);
				dropInfo.push_back(nextLine);
			}
			dropInfo.erase(dropInfo.begin());
			dropInfo.pop_back();
			nextEnemy.dropInfo = dropInfo;
		} else if (nextLine.find("enemy speed:") != std::string::npos) { //SPEED
			nextEnemy.speed = stoi(clFile::cut(nextLine));
		} else if (nextLine.find("enemy level:") != std::string::npos) { //LEVEL
			if (clFile::cut(nextLine) != "ADAPT")
				if (nextLine.find("ADAPT") != std::string::npos) {
					nextLine = clFile::cut(nextLine);
					nextEnemy.level = playerLevel + stoi(nextLine.substr(8, nextLine.length() - 8));
				} else {
					nextEnemy.level = stoi(clFile::cut(nextLine));
				}
		} else if (nextLine.find("enemy agro distance:") != std::string::npos) { //AGRO DISTANCE
			if (clFile::cut(nextLine) != "ADAPT") {
				nextEnemy.agroDistance = stoi(clFile::cut(nextLine));
			}
		}
	}

	return enemies;
}

//Accessors
std::vector<Coordinate> Map::getGatePositions() {
	std::vector<Coordinate> gateLocs;
	for (int i = 0; i < this->_gates.size(); i++) {
		for (int j = 0; j < this->_gates[i].location.size(); j++) {
			gateLocs.push_back(this->_gates[i].location[j]);
		}
	}
	return gateLocs;
}