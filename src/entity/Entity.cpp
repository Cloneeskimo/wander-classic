#include "entity.h"

//Constructor
Entity::Entity(EntityType type, std::string name, char symbol, int x, int y, int speed)
{
	this->_x = x;
	this->_y = y;
	this->_symbol = symbol;
	this->_name = name;
	this->_speed = speed;
	this->_type = type;
}

//Functions
Collision Entity::moveEntity(std::vector<std::string> *layout, Movement direction, std::vector<Coord> gates)
{
	//Establish amount of squares intended to move
	int movesRemaining = this->_speed;
	int movesMade = 0;
	bool movementInBounds = true;

	//Identify target of movement
	while (movesRemaining > 0) {
		int targetX = this->_x;
		int targetY = this->_y;
		switch (direction) {
		case Movement::UP:
			targetY--;
			break;
		case Movement::DOWN:
			targetY++;
			break;
		case Movement::RIGHT:
			targetX++;
			break;
		case Movement::LEFT:
			targetX--;
			break;
		}

		//Check if target is within map bounds
		if (!((*layout).size() > targetY && (*layout)[0].length() > targetX && targetX >= 0 && targetY >= 0)) {
			movementInBounds = false;
		}

		//Move if valid
		if ((*layout)[targetY][targetX] == ' ' & movementInBounds) {
			for (int i = 0; i < gates.size(); i++) {
				if (gates[i].x == targetX && gates[i].y == targetY) {
					Collision collision{ -1, Coord{ targetX, targetY}, movesMade };
					return collision;
				}
			}
			this->setLocation(targetX, targetY);
			movesMade++;
		} else {
			movesRemaining = 0;
			Collision collision{ (*layout)[targetY][targetX],  Coord{ targetX, targetY }, movesMade };
			return collision;
		}
		movesRemaining--;
	}
	Collision collision { ' ', Coord{ this->_x, this->_y }, movesMade };
	return collision;
}