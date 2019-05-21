#include "item.h"

//Class Functions
Item::Item(ItemType type, ItemSubType subType, std::string name, int value, int quantity, int level, int effect)
{
	this->construct(type, subType, name, value, quantity, level, effect);
}
void Item::construct(ItemType type, ItemSubType subType, std::string name, int value, int quantity, int level, int effect)
{
	//Construct Item
	this->_type = type;
	this->_subType = subType;
	this->_name = name;
	this->_value = value;
	this->_quantity = quantity;
	this->_level = level;
	this->_effect = effect;
}
int Item::add(int thisMany)
{
	this->_quantity += thisMany; 
	if (this->_quantity > 99) {
		int leftOver = this->_quantity - 99;
		this->_quantity = 99;
		return leftOver;
	}
}
int Item::remove(int thisMany)
{
	this->_quantity -= thisMany;
	if (this->_quantity < 1) {
		int deficit = this->_quantity;
		this->_quantity = 0;
		return deficit;
	}
}