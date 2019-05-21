
#ifndef Entity_h
#define Entity_h

//Includes
#include <string>
#include <vector>
#include <managers/OptionsManager.h>

//Movement and EntityType Enums
enum class Movement { UP, RIGHT, DOWN, LEFT };
enum class EntityType { BASE, PLAYER, ENEMY };

//Collision Struct
struct Collision {
	char collidedTile;
	Coordinate tileLocation;
	int movesMade;
};

//Entity Class
class Entity {
public:

	//Constructors
	Entity() {}; //default
	Entity(EntityType type, std::string name, char symbol = 'O', int x = 1, int y = 1, int speed = 1); //full

	//Mutators
	void setLocation(int x, int y) { this->_x = x; this->_y = y; }
	void setX(int x) { this->_x = x; }
	void setY(int y) { this->_y = y; }

	//Accessors
	int getX() { return this->_x; }
	int getY() { return this->_y; }
	int getSpeed() { return this->_speed; }
	char getSymbol() { return this->_symbol; }
	std::string getName() { return this->_name; }
	EntityType getEntityType() { return this->_type; }

	//Other Functions
	Collision moveEntity(std::vector<std::string> *layout, Movement direction, std::vector<Coordinate> gates);

private:

	//Data
	int _speed;
	int _x;
	int _y;
	char _symbol;
	std::string _name;
	EntityType _type;
};

#endif