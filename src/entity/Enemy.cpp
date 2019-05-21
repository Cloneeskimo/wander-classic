#include "Enemy.h"

//Constructors
void Enemy::constructEnemyExtras(std::vector<Drop> drops, int maxHealth, int damage, int level, int agroDistance) {
	this->_drops = drops;
	this->_maxHealth = this->_health = maxHealth;
	this->_damage = damage;
	this->_level = level;
	this->_agroDistance = agroDistance;
}

//Node Conversion Functions
Node Enemy::convertToNode() {

	//basic enemy attributes
	Node enemy("enemy", "enemy");
	enemy.addChild(Node("name", this->getName()));
	enemy.addChild(Node("symbol", this->getSymbol()));
	enemy.addChild(Node("health", this->getHealth()));
	enemy.addChild(Node("max health", this->getMaxHealth()));
	enemy.addChild(Node("damage", this->getDamage()));
	enemy.addChild(Node("speed", this->getSpeed()));
	enemy.addChild(Node("level", this->getLevel()));
	enemy.addChild(Node("x", this->getX()));
	enemy.addChild(Node("y", this->getY()));
	enemy.addChild(Node("agro distance", this->_agroDistance));

	//enemy drops
	Node drops("drops", "drops");
	for (Drop drop : this->_drops) drops.addChild(drop.convertToNode());
	enemy.addChild(drops);

	//return node
	return enemy;
}
Enemy Enemy::createEnemyFromNode(Node* node) {

	//construct enemy
	Enemy enemy(EntityType::ENEMY, node->getChildByName("name")->getValue(),
		node->getChildByName("symbol")->getValue()[0],
		node->getChildByName("x")->getValueAsInt(),
		node->getChildByName("y")->getValueAsInt(),
		node->getChildByName("speed")->getValueAsInt());

	//load drops
	std::vector<Drop> drops;
	for (Node drop : *(node->getChildByName("drops")->getChildren())) {
		drops.push_back(Drop::createDropFromNode(&drop));
	}

	//construct enemy extras
	enemy.constructEnemyExtras(drops,
		node->getChildByName("max health")->getValueAsInt(),
		node->getChildByName("damage")->getValueAsInt(),
		node->getChildByName("level")->getValueAsInt(),
		node->getChildByName("agro distance")->getValueAsInt());

	//set health and return
	enemy.setHealth(node->getChildByName("health")->getValueAsInt());
	return enemy;
}

//Other Functions
ColorString Enemy::createHealthBar(int width) {

	//variables
	ColorString healthBar;
	std::string text = "HP: " + to_string(this->_health) + "/" + to_string(this->getMaxHealth());
	std::string content = text + " <";
	int contentWidth = width - 2 - text.length();
	int fullness = (((float)this->_health / (float)this->getMaxHealth()) * (float)contentWidth);

	//hard borders
	if (this->_health == 0) {
		fullness = 0;
	} else if (this->_health == this->_maxHealth) {
		fullness = contentWidth;
	}

	//fill string
	for (int i = 0; i < fullness; i++) {
		content += "|";
	}
	for (int i = 0; i < contentWidth - fullness; i++) {
		content += "-";
	}
	content += ">";

	//colorize
	healthBar = ColorString(content, ColorFlag::RED, text.length() + 2, fullness + text.length() + 2, true);
	healthBar.configureWordTypeDisplay(0, 0, false);
	return healthBar;
}
// RETURN - whether or not enemy died after taking damage
bool Enemy::takeDamage(int thisMuch) {
	this->_health -= thisMuch;
	if (this->_health < 1) {
		this->_health = 0;
		return true;
	} else {
		return false;
	}
}
void Enemy::updateEffects() {

	//stun effect
	if (this->_isStunned > 0)
		this->_isStunned--;

	//damage over time effect
	for (int i = 0; i < this->_dots.size(); i++) {
		if (this->_dots[i].length > 1) {
			this->takeDamage(this->_dots[i].dmg);
			this->_dots[i].length--;
		} else {
			this->_dots.erase(this->_dots.begin() + i);
			i--;
		}
	}
}