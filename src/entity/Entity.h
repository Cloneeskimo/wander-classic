#pragma once
#include <string>
#include <vector>
#include "optionsManager.h"

//Enums & Structs
enum class Movement
{
	UP, RIGHT, DOWN, LEFT
};
enum class EntityType
{
	BASE, PLAYER, ENEMY
};
struct Collision
{
	char collidedTile;
	Coord tileLocation;
	int movesMade;
};

class Entity
{
public:

	//Constructor
	Entity() {};
	Entity(EntityType type, std::string name, char symbol = 'O', int x = 1, int y = 1, int speed = 1);

	//Setters
	void setLocation(int x, int y) { this->_x = x; this->_y = y; }
	void setX(int x) { this->_x = x; }
	void setY(int y) { this->_y = y; }
	
	//Getters
	int getX() { return this->_x; }
	int getY() { return this->_y; }
	int getSpeed() { return this->_speed; }
	char getSymbol() { return this->_symbol; }
	std::string getName() { return this->_name; }
	EntityType getEntityType() { return this->_type; }

	//Functions
	Collision moveEntity(std::vector<std::string> *layout, Movement direction, std::vector<Coord> gates);

private:

	//Variables
	int _speed;
	int _x;
	int _y;
	char _symbol;
	std::string _name;
	EntityType _type;
};