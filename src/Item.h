
#ifndef Item_h
#define Item_h

//Includes
#include <string>
#include <Clfram.h>
#include <Node.h>
#include <managers/OptionsManager.h>

//Item Type Enum
enum ItemType {
	JUNK,
	WEAPON,
	ARMOR,
	POTION,
	MISCELLANEOUS
};

//Item Sub-Type Enum
enum ItemSubType {

	//None
	NOSUBTYPE,

	//Armor
	HELMET,
	SHOULDERPADS,
	NECKLACE,
	CHESTPIECE,
	BRACERS,
	GLOVES,
	BELT,
	LEGGINGS,
	BOOTS,

	//Weapon
	SWORD,
	BOW,
	DAGGER,
	STAFF,
	WAND,
	HAMMER
};

//Static Variables
const static int ITEMSUBTYPES = 16;
const static int SUBTYPEWEAPONAFTER = 9;
const static int ITEMTYPES = 5;

//Item Class
class Item {
public:

	//Operator
	friend bool operator==(const Item& lhs, const Item& rhs) {
		if (const_cast<Item&>(lhs).getName() != const_cast<Item&>(rhs).getName()) { //Check Name
			return false;
		}
		if (const_cast<Item&>(lhs).getType() != const_cast<Item&>(rhs).getType()) { //Check Type
			return false;
		}
		if (const_cast<Item&>(lhs).getSubType() != const_cast<Item&>(rhs).getSubType()) { //Check Subtype
			return false;
		}
		if (const_cast<Item&>(lhs).getValue() != const_cast<Item&>(rhs).getValue()) { //Check Value
			return false;
		}
		return true;
	}

	//Constructors
	//ITEM CREATION:
	//TYPE - The only subtypes are for WEAPON/ARMOR
	//SUBTYPE - If none, set as NOSUBTYPE
	//NAME - Item name
	//VALUE - How much the item can be sold for
	//QUANTITY - How many there is in this itemstack
	//EFFECT - If weapon -> Damage
	//         If armor  -> Armor
	//         If potion -> Amount of Effect on Player
	//         If else   -> -1
	Item(ItemType type, ItemSubType subType, std::string name, int value = 1, int quantity = 1, int level = -1, int effect = -1); //full constructor
	Item() {}; //default constructor
	void construct(ItemType type, ItemSubType subType, std::string name, int value = 1, int quantity = 1, int level = -1, int effect = -1); //later constructor
	
	//Node Conversion Functions
	Node convertToNode();
	static Item createItemFromNode(Node* node);

	//Accessors
	ItemType getType() { return this->_type; }
	ItemSubType getSubType() { return this->_subType; }
	std::string getName() { return this->_name; }
	int getQuantity() { return this->_quantity; }
	int getValue() { return this->_value; }
	int getEffect() { return this->_effect; }
	int getLevel() { return this->_level; }

	//Mutators
	int add(int thisMany = 1);
	int remove(int thisMany = 1);
	void setQuantity(int thisMany) { this->_quantity = thisMany; }

private:

	//Variables
	ItemType _type;
	ItemSubType _subType;
	std::string _name;
	int _quantity;
	int _value;
	int _effect;
	int _level;
};

//WeaponArmor Struct
//WHEN GENERATING RANDOM ITEMS:
//Set what is specifically desired
//Leave default for stat to be randomly generated
struct ArmorInfo {
	ArmorInfo(int level = 1, int effect = -1, std::string name = "RANDOM", ItemSubType piece = NOSUBTYPE) {
		this->level = level;
		this->effect = effect;
		this->name = name;
		this->piece = piece;
	}
	int level = 1;
	int effect = -1;
	std::string name = "RANDOM";
	ItemSubType piece = NOSUBTYPE;
};

//WeaponInfo Struct
struct WeaponInfo {
	WeaponInfo(int level = 1, int effect = -1, std::string name = "RANDOM", ItemSubType type = NOSUBTYPE) {
		this->level = level;
		this->effect = effect;
		this->name = name;
		this->type = type;
	}
	int level = 1;
	int effect = -1;
	std::string name = "RANDOM";
	ItemSubType type = NOSUBTYPE;
};

//WeaponLevel Struct
struct WeaponLevel {

	//Constructor
	WeaponLevel() {};
	WeaponLevel(ItemSubType weapon, int level) { this->weapon = weapon; this->level = level; }

	//Node Conversion Functions
	Node convertToNode();
	static WeaponLevel createWeaponLevelFromNode(Node* node);

	//Variables
	ItemSubType weapon = NOSUBTYPE;
	int level = 1;
};

//Drop Struct
struct Drop {

	//Data
	Item dropItem;
	int chance; //OUT OF 100%

	//Node Conversion Functions
	Node convertToNode();
	static Drop createDropFromNode(Node* node);

};

//Item Type Conversions
static std::string convertItemType(ItemType itemType) {
	switch (itemType) {
		case (MISCELLANEOUS):
			return ("MISCELLANEOUS");
		case (WEAPON):
			return ("WEAPON");
		case (ARMOR):
			return ("ARMOR");
		case (POTION):
			return ("POTION");
		case (JUNK):
			return ("JUNK");
	}
};
static std::string convertItemSubType(ItemSubType itemSubType, bool noSubIsSpace = false) {
	switch (itemSubType) {
		case NOSUBTYPE:
			if (noSubIsSpace)
				return "";
			else
				return "NOSUBTYPE";
		case HELMET:
			return "HELMET";
		case SHOULDERPADS:
			return "SHOULDERPADS";
		case NECKLACE:
			return "NECKLACE";
		case CHESTPIECE:
			return "CHESTPIECE";
		case BRACERS:
			return "BRACERS";
		case GLOVES:
			return "GLOVES";
		case BELT:
			return "BELT";
		case LEGGINGS:
			return "LEGGINGS";
		case BOOTS:
			return "BOOTS";
		case SWORD:
			return "SWORD";
		case BOW:
			return "BOW";
		case DAGGER:
			return "DAGGER";
		case STAFF:
			return "STAFF";
		case WAND:
			return "WAND";
		case HAMMER:
			return "HAMMER";
	}
}
static ItemType convertItemString(std::string itemType) {
	std::string uppercaseItemType = clUtil::uppercase(itemType);
	if (uppercaseItemType == "JUNK") {
		return JUNK;
	} else if (uppercaseItemType == "WEAPON") {
		return WEAPON;
	} else if (uppercaseItemType == "ARMOR") {
		return ARMOR;
	} else if (uppercaseItemType == "POTION") {
		return POTION;
	} else if (uppercaseItemType == "MISCELLANEOUS") {
		return MISCELLANEOUS;
	}
}
static ItemSubType convertItemSubString(std::string itemSubType) {
	std::string uppercaseSubItem = clUtil::uppercase(itemSubType);
	if (uppercaseSubItem == "HELMET") {
		return HELMET;
	} else if (uppercaseSubItem == "SHOULDERPADS") {
		return SHOULDERPADS;
	} else if (uppercaseSubItem == "NECKLACE") {
		return NECKLACE;
	} else if (uppercaseSubItem == "CHESTPIECE") {
		return CHESTPIECE;
	} else if (uppercaseSubItem == "BRACERS") {
		return BRACERS;
	} else if (uppercaseSubItem == "GLOVES") {
		return GLOVES;
	} else if (uppercaseSubItem == "BELT") {
		return BELT;
	} else if (uppercaseSubItem == "LEGGINGS") {
		return LEGGINGS;
	} else if (uppercaseSubItem == "BOOTS") {
		return BOOTS;
	} else if (uppercaseSubItem == "SWORD") {
		return SWORD;
	} else if (uppercaseSubItem == "BOW") {
		return BOW;
	} else if (uppercaseSubItem == "DAGGER") {
		return DAGGER;
	} else if (uppercaseSubItem == "STAFF") {
		return STAFF;
	} else if (uppercaseSubItem == "WAND") {
		return WAND;
	} else if (uppercaseSubItem == "HAMMER") {
		return HAMMER;
	} else {
		return NOSUBTYPE;
	}
}

#endif