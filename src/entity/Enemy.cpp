#include "enemy.h"

//Constructors
void Enemy::constructEnemyExtras(std::vector<Drop> drops, int maxHealth, int damage, int level, int agroDistance)
{
	this->_drops = drops;
	this->_maxHealth = this->_health = maxHealth;
	this->_damage = damage;
	this->_level = level;
	this->_agroDistance = agroDistance;
}

//Functions
ColorString Enemy::createHealthBar(int width)
{
	//Variables
	ColorString healthBar;
	std::string text = "HP: " + to_string(this->_health) + "/" + to_string(this->getMaxHealth());
	std::string content = text + " <";
	int contentWidth = width - 2 - text.length();
	int fullness = (((float)this->_health / (float)this->getMaxHealth()) * (float)contentWidth);

	//Hard Borders
	if (this->_health == 0) {
		fullness = 0;
	} else if (this->_health == this->_maxHealth) {
		fullness = contentWidth;
	}

	//Fill String
	for (int i = 0; i < fullness; i++) {
		content += "|";
	}
	for (int i = 0; i < contentWidth - fullness; i++) {
		content += "-";
	}
	content += ">";

	//Colorize
	healthBar = ColorString(content, ColorFlag::RED, text.length() + 2, fullness + text.length() + 2, true);
	healthBar.configureWordTypeDisplay(0, 0, false);
	return healthBar;
}
bool Enemy::takeDamage(int thisMuch)
{
	//Returns whether or not enemy died after taking damage
	this->_health -= thisMuch;
	if (this->_health < 1) {
		this->_health = 0;
		return true;
	} else {
		return false;
	}
}
void Enemy::updateEffects()
{
	//Stun Effect
	if (this->_isStunned > 0) 
		this->_isStunned--; 

	//Damage Over Time Effect
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