
#include "Item.h"

/* Item */

//Constructors
Item::Item(ItemType type, ItemSubType subType, std::string name, int value, int quantity, int level, int effect) {
	this->construct(type, subType, name, value, quantity, level, effect);
}
void Item::construct(ItemType type, ItemSubType subType, std::string name, int value, int quantity, int level, int effect) {
	this->_type = type;
	this->_subType = subType;
	this->_name = name;
	this->_value = value;
	this->_quantity = quantity;
	this->_level = level;
	this->_effect = effect;
}

//Node Conversion Functions
Node Item::convertToNode() {
	Node item("item", "item");
	item.addChild(Node("type", convertItemType(this->_type)));
	item.addChild(Node("subtype", convertItemSubType(this->_subType)));
	item.addChild(Node("name", this->_name));
	item.addChild(Node("value", this->_value));
	item.addChild(Node("quantity", this->_quantity));
	item.addChild(Node("level", this->_level));
	item.addChild(Node("effect", this->_effect));
	return item;
}
Item Item::createItemFromNode(Node* node) {
	return Item(convertItemString(node->getChildByName("type")->getValue()), 
		convertItemSubString(node->getChildByName("subtype")->getValue()),
		node->getChildByName("name")->getValue(),
		node->getChildByName("value")->getValueAsInt(),
		node->getChildByName("quantity")->getValueAsInt(),
		node->getChildByName("level")->getValueAsInt(),
		node->getChildByName("effect")->getValueAsInt());
}

//Mutators
int Item::add(int thisMany) {
	this->_quantity += thisMany;
	if (this->_quantity > 99) {
		int leftOver = this->_quantity - 99;
		this->_quantity = 99;
		return leftOver;
	}
}
int Item::remove(int thisMany) {
	this->_quantity -= thisMany;
	if (this->_quantity < 1) {
		int deficit = this->_quantity;
		this->_quantity = 0;
		return deficit;
	}
}

/* WeaponLevel*/

//Node Conversion Functions
Node WeaponLevel::convertToNode() {
	Node node("weapon level", "weapon level");
	node.addChild(Node("weapon", convertItemSubType(this->weapon)));
	node.addChild(Node("level", this->level));
	return node;
}
WeaponLevel WeaponLevel::createWeaponLevelFromNode(Node* node) {
	WeaponLevel weaponLevel;
	weaponLevel.level = node->getChildByName("level")->getValueAsInt();
	weaponLevel.weapon = convertItemSubString(node->getChildByName("weapon")->getValue());
	return weaponLevel;
}

/* Drop */

//Node Conversion Functions
Node Drop::convertToNode() {
	Node drop("drop", "drop");
	drop.addChild(Node("chance", this->chance));
	drop.addChild(this->dropItem.convertToNode());
	return drop;
}
Drop Drop::createDropFromNode(Node* node) {
	Drop drop;
	drop.dropItem = Item::createItemFromNode(node->getChildByName("item"));
	drop.chance = node->getChildByName("chance")->getValueAsInt();
	return drop;
}