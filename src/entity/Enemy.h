
#ifndef Enemy_h
#define Enemy_h

//includes
#include <vector>
#include <string>
#include <random>
#include <managers/CalcManager.h>
#include <entity/Entity.h>
#include <Item.h>
#include <entity/Player.h>

//Random Enemy Info Struct
//WHEN GENERATING RANDOM ENEMIES:
//Leave default for random
//Some (commented below) cannot be set to random, they MUST be given values
struct EnemyRandomInfo {

	//Constructor
	EnemyRandomInfo(std::string name = "Unnamed Enemy", float eliteness = 1.0, std::vector<std::string> dropInfo = std::vector<std::string>(), int speed = 1, int level = -1, int damage = -1, int health = -1) {
		this->name = name;
		this->dropInfo = dropInfo;
		this->speed = speed;
		this->level = level;
		this->damage = damage;
		this->health = health;
		this->eliteness = eliteness;
	}

	//Data
	std::string name = "Unnamed Enemy"; //NOT RANDOMIZED
	std::vector<std::string> dropInfo; //NOT RANDOMIZED
	int speed = 1; //NOT RANDOMIZED
	int level = -1; //BECOMES PLAYER LEVEL IF DEFAULT
	char symbol = 1000; //BECOMES FIRST LETTER IF DEFAULT
	int damage = -1;
	int health = -1;
	int agroDistance = -1;
	float eliteness = 1.0;
};

//Dot Struct
struct Dot {

	//Constructors
	Dot() {}; //default
	Dot(int length, int dmg) { //full
		this->length = length;
		this->dmg = dmg;
	}

	//Variables
	int length;
	int dmg;
};

//Enemy Class
class Enemy : public Entity {
public:

	//Constructors
	using Entity::Entity;
	void constructEnemyExtras(std::vector<Drop> drops, int maxHealth, int damage, int level, int agroDistance = 5);

	//Node Conversion Functions
	Node convertToNode();
	static Enemy createEnemyFromNode(Node* node);

	//Other Functions
	ColorString createHealthBar(int width);
	bool takeDamage(int thisMuch);

	//Accessors
	int getHealth() { return this->_health; }
	int getMaxHealth() { return this->_maxHealth; }
	int getDamage() { return this->_damage; }
	int getLevel() { return this->_level; }
	int getAgroDistance() { return this->_agroDistance; }
	int getStunned() { return this->_isStunned; }
	bool getAgroed() { return this->_agroed; }
	std::vector<Dot> getDots() { return this->_dots; }
	std::vector<Drop> getDrops() { return this->_drops; }

	//Mutators
	void setHealth(int newHealth) { this->_health = newHealth; }
	void setAgro(bool toThis) { this->_agroed = toThis; }
	void updateEffects();
	void stun(int turns = 1) { this->_isStunned = turns; }
	void addDot(Dot thisDot) { this->_dots.push_back(thisDot); }

private:

	//Collection Data
	std::vector<Drop> _drops;
	std::vector<Dot> _dots;

	//Other Data
	bool _agroed = false;
	int _isStunned = 0;
	int _health, _maxHealth;
	int _damage;
	int _level;
	int _agroDistance = 10;
};

//ELITENESS OF 2 -> DOUBLE HEALTH & DMG // ELITENESS OF 1/2 -> HALF HEALTH & DMG
static Enemy createRandomEnemy(EnemyRandomInfo info, int playerLevel) {

	//set enemy level
	if (info.level == -1) info.level = playerLevel;

	//set enemy health
	if (info.health == -1) info.health = CalcManager::getEnemyHealth(info.level, info.eliteness);

	//set enemy damage
	if (info.damage == -1) {
		info.damage = CalcManager::getEnemyDamage(info.level, info.eliteness);
		if (info.damage < 1) info.damage = 1;
	}

	//set enemy agro distance
	if (info.agroDistance == -1) info.agroDistance = 5;

	//create enemy drops
	std::vector<Drop> enemyDrops = CalcManager::createDropsFromStrings(info.dropInfo, info.level, info.eliteness);

	//create and return enemy
	Enemy newEnemy = Enemy(EntityType::ENEMY, info.name, info.symbol, 0, 0, info.speed);
	newEnemy.constructEnemyExtras(enemyDrops, info.health, info.damage, info.level);
	return newEnemy;
}

#endif