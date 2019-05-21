#include "Player.h"

//Constructors
void Player::constructPlayerExtras(int level, int gold, int health, int exp, std::vector<Level> levels, std::vector<Item> inventory, std::vector<Item> equipment) {

	//construct player extras
	this->_level = level;
	this->_gold = gold;
	this->_health = health;
	this->_exp = exp;
	this->_levels = levels;
	this->_inventory = inventory;
	this->_equipment = equipment;

	//check health
	if (this->_health < 0) {
		this->_health = this->getMaxHealth();
	}
}
void Player::constructPlayerAttributes(int attributePoints, int endurance, int spirit, int capability) {
	this->_attributePoints = attributePoints;
	this->_endurance = endurance;
	this->_spirit = spirit;
	this->_capability = capability;
}

//Node Conversion Functions 
Node Player::convertToNode() {

	//basic player info
	Node n("player", "player");
	n.addChild(Node("name", this->getName()));
	n.addChild(Node("speed", this->getSpeed()));
	n.addChild(Node("x", this->getX()));
	n.addChild(Node("y", this->getY()));
	n.addChild(Node("symbol", this->getSymbol()));
	n.addChild(Node("level", this->_level));
	n.addChild(Node("gold", this->_gold));
	n.addChild(Node("health", this->_health));
	n.addChild(Node("attribute points", this->_attributePoints));
	n.addChild(Node("endurance", this->_endurance));
	n.addChild(Node("spirit", this->_spirit));
	n.addChild(Node("capability", this->_capability));
	n.addChild(Node("exp", this->_exp));

	//inventory
	Node inventory("inventory", "inventory");
	for (Item item : this->_inventory) inventory.addChild(item.convertToNode());
	n.addChild(inventory);

	//equipment
	Node equipment("equipment", "equipment");
	for (Item item : this->_equipment) equipment.addChild(item.convertToNode());
	n.addChild(equipment);

	//weapon levels
	Node weaponLevels("weapon levels", "weapon levels");
	for (WeaponLevel weaponLevel : this->_weaponLevels) weaponLevels.addChild(weaponLevel.convertToNode());
	n.addChild(weaponLevels);

	//return node
	return n;
}
Player Player::createPlayerFromNode(Node* node, std::vector<Level> levels) {

	//create player
	Player player(EntityType::PLAYER,
		node->getChildByName("name")->getValue(),
		node->getChildByName("symbol")->getValue()[0],
		node->getChildByName("x")->getValueAsInt(),
		node->getChildByName("y")->getValueAsInt(),
		node->getChildByName("speed")->getValueAsInt());

	//inventory
	std::vector<Item> inventory;
	for (Node item : *(node->getChildByName("inventory")->getChildren())) {
		inventory.push_back(Item::createItemFromNode(&item));
	}

	//equipment
	std::vector<Item> equipment;
	for (Node item : *(node->getChildByName("equipment")->getChildren())) {
		equipment.push_back(Item::createItemFromNode(&item));
	}

	//construct player extras
	player.constructPlayerExtras(node->getChildByName("level")->getValueAsInt(),
		node->getChildByName("gold")->getValueAsInt(),
		node->getChildByName("health")->getValueAsInt(),
		node->getChildByName("exp")->getValueAsInt(),
		levels, inventory, equipment);

	//weapon levels
	std::vector<WeaponLevel> weaponLevels;
	for (Node weaponLevel : *(node->getChildByName("weapon levels")->getChildren())) {
		weaponLevels.push_back(WeaponLevel::createWeaponLevelFromNode(&weaponLevel));
	}
	player.setWeaponLevels(weaponLevels);

	//construct player attributes
	player.constructPlayerAttributes(node->getChildByName("attribute points")->getValueAsInt(),
		node->getChildByName("endurance")->getValueAsInt(),
		node->getChildByName("spirit")->getValueAsInt(),
		node->getChildByName("capability")->getValueAsInt());

	//return player
	return player;
}

//Accessors
Item* Player::getWeapon() {
	for (int i = 0; i < this->_equipment.size(); i++) {
		if (this->_equipment[i].getSubType() > ItemSubType(SUBTYPEWEAPONAFTER)) {
			return &this->_equipment[i];
		}
	}
}
//RETURNS - -1 if invalid
int Player::getWeaponLevel(ItemSubType thisWeapon) {
	for (int i = 0; i < this->_weaponLevels.size(); i++) {
		if (thisWeapon == this->_weaponLevels[i].weapon) {
			return this->_weaponLevels[i].level;
		}
	}
	return -1;
}
int Player::getMaxHealth() {

	//get base health
	int health = CalcManager::getBaseHealth(this->_level);

	//calculate effective armor/endurance
	float effectiveArmor = this->getEffectiveArmor();
	float effectiveEndurance = this->getEffectiveEndurance();

	//factor in effective armor/endurance
	health += (float)(effectiveArmor * effectiveEndurance) + 0.5f;

	//return health
	return health;
}
float Player::getWeaponProficiency() {
	return 1.0f + ((((float)this->getWeaponLevel(this->getWeapon()->getSubType())) - 1) / (float)100);
}

//Totaling Functions
int Player::getTotalItems() {

	//calculate and return total item count
	int totalItems = 0;
	for (int i = 0; i < this->_inventory.size(); i++) {
		totalItems += this->_inventory[i].getQuantity();
	}
	return totalItems;
}
int Player::getTotalArmor() {
	int totalArmor = 0;
	for (int i = 0; i < this->_equipment.size(); i++) {
		if (this->_equipment[i].getType() != ItemType::WEAPON) {
			totalArmor += this->_equipment[i].getEffect();
		}
	}
	return totalArmor;
}
int Player::getRegeneration() {
	return (float)this->_level * (this->getEffectiveSpirit());
}
float Player::getTotalDamage(bool withCapability, bool withProficiency) {
	int dmg = 0;
	for (int i = 0; i < this->_equipment.size(); i++) {
		if (this->_equipment[i].getType() == ItemType::WEAPON) {
			dmg = this->_equipment[i].getEffect();
			if (withCapability) {
				dmg *= this->getEffectiveCapability();
			}
			if (withProficiency) {
				dmg *= this->getWeaponProficiency();
			}
		}
	}
	return dmg;
}
int Player::getInventoryValue() {
	int totalValue = 0;
	for (int i = 0; i < this->_inventory.size(); i++) {
		totalValue += (this->_inventory[i].getValue() * this->_inventory[i].getQuantity());
	}
	return totalValue;
}

//Mutators
void Player::addItemToInventory(Item thisItem) {
	bool existsAlready = false;
	for (int i = 0; i < this->_inventory.size(); i++) {
		if (thisItem == this->_inventory[i]) {
			this->_inventory[i].add(thisItem.getQuantity());
			existsAlready = true;
			while (this->_inventory[i].getQuantity() > MAX_STACK_SIZE) {
				thisItem.setQuantity(this->_inventory[i].getQuantity() - MAX_STACK_SIZE);
				this->_inventory[i].setQuantity(99);
				this->_inventory.push_back(thisItem);
			}
		}
	}
	if (!existsAlready)
		this->_inventory.push_back(thisItem);
}
void Player::addWeaponLevel(ItemSubType thisType) {
	for (int i = 0; i < this->_weaponLevels.size(); i++) {
		if (this->_weaponLevels[i].weapon == thisType) {
			this->_weaponLevels[i].level++;
		}
	}
}
void Player::addAttribute(int choice) {
	if (this->_attributePoints > 0) {
		if (choice == 0)
			this->_endurance++;
		else if (choice == 1)
			this->_spirit++;
		else if (choice == 2)
			this->_capability++;
		this->_attributePoints--;
	} else {
		clCons::cls();
		clCons::centerVerhor(1, "You don't have any attribute points!");
		clCons::paus("");
	}
}
void Player::heal(int thisMuch) {
	this->_health += thisMuch;
	if (this->_health > this->getMaxHealth()) {
		this->_health = this->getMaxHealth();
	}
}

//Other Functions
void Player::passiveUpdate() {

	//regenerate health
	if (this->_health != this->getMaxHealth()) {
		this->_health += this->getRegeneration();
		if (this->_health > this->getMaxHealth()) {
			this->_health = this->getMaxHealth();
		}
	}
}
void Player::openInventory() {

	//if inventory is empty
	if (this->_inventory.size() == 0) {
		clCons::cls();
		clCons::centerVerhor(1, "Your inventory is empty!");
		clCons::paus("");
		return;
	}

	//grab screen properties
	int screenWidth = clCons::getConsoleWidth();
	int screenHeight = clCons::getConsoleHeight();

	//extra variables
	int input = 0;
	int selection = 0;

	//dual display vectors
	std::vector<std::string> leftDisplay;
	std::vector<ColorString> rightDisplay;
	ColorString blankString;
	blankString.content = "";

	//display variables
	int leftDisplayWidth = 10;
	int LEFT_DISPLAY_EXTRAS = 12;
	int rightDisplayWidth;
	int COMPLETE_DISPLAY_EXTRAS = 3;
	int displayHeight = 10;
	int DISPLAY_HEIGHT_EXTRAS = 4;

	//determine left display width
	for (int i = 0; i < this->_inventory.size(); i++) {
		int nextLength = this->_inventory[i].getName().length();
		if (nextLength > (leftDisplayWidth - LEFT_DISPLAY_EXTRAS)) {
			leftDisplayWidth = this->_inventory[i].getName().size() + LEFT_DISPLAY_EXTRAS;
		}
	}

	//inventory display settings
	int bottomMargin = 0;

	//inventory stats
	int totalItems = getTotalItems();
	int totalValue = getInventoryValue();

	//pre-loop
	clCons::cls();

	//loop
	while (!(input == 27 || input == 'b' || input == 'B')) {

		//determine left display width
		for (int i = 0; i < this->_inventory.size(); i++) {
			int nextLength = this->_inventory[i].getName().length();
			if (nextLength > (leftDisplayWidth - LEFT_DISPLAY_EXTRAS)) {
				leftDisplayWidth = this->_inventory[i].getName().size() + LEFT_DISPLAY_EXTRAS;
			}
		}

		//determine adaptable display variables
		screenWidth = clCons::getConsoleWidth();
		int oldScreenHeight = screenHeight;
		screenHeight = clCons::getConsoleHeight();
		if (oldScreenHeight != screenHeight) clCons::cls();
		rightDisplayWidth = screenWidth - leftDisplayWidth - COMPLETE_DISPLAY_EXTRAS;
		displayHeight = 12;
		if (screenHeight < displayHeight + DISPLAY_HEIGHT_EXTRAS) displayHeight = screenHeight - DISPLAY_HEIGHT_EXTRAS;
		else displayHeight = 12;

		//check selection
		if (selection >= bottomMargin + displayHeight) selection = bottomMargin + displayHeight - 1;

		//set left display
		leftDisplay = std::vector<std::string>();
		std::string nextLine = "";
		for (int i = bottomMargin; i < this->_inventory.size(); i++) {
			nextLine = "";
			if (i + 1 <= 9) {
				nextLine += to_string(i + 1) + " ";
			} else {
				nextLine += to_string(i + 1);
			}
			if (i == selection) {
				nextLine += " -[" + this->_inventory[i].getName() + "]-";
			} else {
				nextLine += "   " + this->_inventory[i].getName() + "   ";
			}
			int quantSpaces = leftDisplayWidth - nextLine.length() - 4;
			for (int i = 0; i < quantSpaces; i++) {
				nextLine += " ";
			}
			if (this->_inventory[i].getQuantity() < 10) {
				nextLine += " x0" + to_string(this->_inventory[i].getQuantity());
			} else {
				nextLine += " x" + to_string(this->_inventory[i].getQuantity());
			}
			leftDisplay.push_back(nextLine);
		}

		//set right display
		rightDisplay = std::vector<ColorString>();
		ColorString toDisplay;
		for (int i = 0; i < displayHeight / 4; i++) {
			rightDisplay.push_back(blankString);
		}

		//item name
		toDisplay = ColorString(clCons::centerStringIn(rightDisplayWidth, "-=[ " + this->_inventory[selection].getName() + " ]=-"), ColorFlag::GREEN, true, 1, clUtil::wordCount(clCons::centerStringIn(rightDisplayWidth, "-=[ " + this->_inventory[selection].getName() + " ]=-")) - 2, true);
		rightDisplay.push_back(toDisplay);
		rightDisplay.push_back(blankString);

		//weapon display
		if (this->_inventory[selection].getType() == WEAPON) {

			toDisplay = ColorString(clCons::centerStringIn(rightDisplayWidth, "Type: " + convertItemSubType(this->_inventory[selection].getSubType())), ColorFlag::GRAY, true);
			rightDisplay.push_back(toDisplay);

			if (this->_inventory[selection].getLevel() > this->_level) { //LEVEL
				toDisplay = ColorString(clCons::centerStringIn(rightDisplayWidth, "Item Level: " + to_string(this->_inventory[selection].getLevel())), ColorFlag::RED, true, 2);
			} else {
				toDisplay = ColorString(clCons::centerStringIn(rightDisplayWidth, "Item Level: " + to_string(this->_inventory[selection].getLevel())), ColorFlag::GREEN, true, 2);
			}
			rightDisplay.push_back(toDisplay);

			Item* currentWeapon = &Item(); //DAMAGE
			if (this->getPiece(this->_inventory[selection].getSubType(), currentWeapon)) {
				int comparison = this->_inventory[selection].getEffect() - currentWeapon->getEffect();
				string comparisonString = "(+" + to_string(comparison) + ")";
				ColorFlag comparisonColor = ColorFlag::GRAY;
				if (comparison > 0) {
					comparisonColor = ColorFlag::GREEN;
				} else if (comparison < 0) {
					comparisonColor = ColorFlag::RED;
					comparisonString = "(" + to_string(comparison) + ")";
				}
				toDisplay = ColorString(clCons::centerStringIn(rightDisplayWidth, "Damage: " + to_string(this->_inventory[selection].getEffect()) + " " + comparisonString), comparisonColor, true, 2);
			} else {
				toDisplay = ColorString(clCons::centerStringIn(rightDisplayWidth, "Damage: " + to_string(this->_inventory[selection].getEffect())), ColorFlag::GRAY, true);
			}
			rightDisplay.push_back(toDisplay);

			toDisplay = ColorString(clCons::centerStringIn(rightDisplayWidth, "Value: " + to_string(this->_inventory[selection].getValue()) + "g"), ColorFlag::YELLOW, true, 1);
			rightDisplay.push_back(toDisplay);

			//armor display
		} else if (this->_inventory[selection].getType() == ARMOR) {

			toDisplay = ColorString(clCons::centerStringIn(rightDisplayWidth, "Type: " + convertItemSubType(this->_inventory[selection].getSubType())), ColorFlag::GRAY, true);
			toDisplay.configureWordTypeDisplay(5000);
			rightDisplay.push_back(toDisplay);

			if (this->_inventory[selection].getLevel() > this->_level) { //LEVEL
				toDisplay = ColorString(clCons::centerStringIn(rightDisplayWidth, "Item Level: " + to_string(this->_inventory[selection].getLevel())), ColorFlag::RED, true, 2);
			} else {
				toDisplay = ColorString(clCons::centerStringIn(rightDisplayWidth, "Item Level: " + to_string(this->_inventory[selection].getLevel())), ColorFlag::GREEN, true, 2);
			}
			rightDisplay.push_back(toDisplay);

			Item* currentArmor = &Item(); //ARMOR
			if (this->getPiece(this->_inventory[selection].getSubType(), currentArmor)) {
				int comparison = this->_inventory[selection].getEffect() - currentArmor->getEffect();
				string comparisonString = "(+" + to_string(comparison) + ")";
				ColorFlag comparisonColor = ColorFlag::GRAY;
				if (comparison > 0) {
					comparisonColor = ColorFlag::GREEN;
				} else if (comparison < 0) {
					comparisonColor = ColorFlag::RED;
					comparisonString = "(" + to_string(comparison) + ")";
				}
				toDisplay = ColorString(clCons::centerStringIn(rightDisplayWidth, "Armor: " + to_string(this->_inventory[selection].getEffect()) + " " + comparisonString), comparisonColor, true, 2);
			} else {
				toDisplay = ColorString(clCons::centerStringIn(rightDisplayWidth, "Armor: " + to_string(this->_inventory[selection].getEffect())), ColorFlag::GRAY, true);
			}
			rightDisplay.push_back(toDisplay);

			toDisplay = ColorString(clCons::centerStringIn(rightDisplayWidth, "Value: " + to_string(this->_inventory[selection].getValue()) + "g"), ColorFlag::YELLOW, true, 1);
			rightDisplay.push_back(toDisplay);

			//other item type display
		} else {
			toDisplay = ColorString(clCons::centerStringIn(rightDisplayWidth, "Type: " + convertItemType(this->_inventory[selection].getType())), ColorFlag::GRAY, true, 1);
			rightDisplay.push_back(toDisplay);
			toDisplay = ColorString(clCons::centerStringIn(rightDisplayWidth, "Value: " + to_string(this->_inventory[selection].getValue()) + "(" + to_string(this->_inventory[selection].getValue() * this->_inventory[selection].getQuantity()) + ")g"), ColorFlag::YELLOW, true, 1);
			rightDisplay.push_back(toDisplay);
		}

		//pre-display
		clCons::centerVerhor(displayHeight + DISPLAY_HEIGHT_EXTRAS, this->getName() + "'s Inventory", ' ', 2);

		//display top
		std::string topMessage = "";
		while (topMessage.length() < screenWidth) {
			if (topMessage.length() == 0 || topMessage.length() == screenWidth - 1 || topMessage.length() == leftDisplayWidth + 1) {
				topMessage += "+";
			} else if (topMessage.length() == leftDisplayWidth + 3) {
				topMessage += "[" + to_string(totalItems) + " items]-[0 gold]-[" + to_string(totalValue) + " total value]";
			} else {
				topMessage += "-";
			}
		}
		std::cout << topMessage;

		//display middle
		for (int i = 0; i < displayHeight; i++) {

			//display left
			std::cout << "|";
			if (leftDisplay.size() > i) {
				std::cout << leftDisplay[i];
				for (int j = 0; j < leftDisplayWidth - leftDisplay[i].length(); j++) {
					std::cout << " ";
				}
			} else {
				for (int j = 0; j < leftDisplayWidth; j++) {
					std::cout << " ";
				}
			}

			//display right
			std::cout << "|";
			if (rightDisplay.size() > i) {
				if (rightDisplay[i].content != "") {
					ColorManager::csout(rightDisplay[i]);
					for (int j = 0; j < rightDisplayWidth - rightDisplay[i].length(); j++) {
						std::cout << " ";
					}
				} else {
					for (int j = 0; j < rightDisplayWidth; j++) {
						std::cout << " ";
					}
				}
			} else {
				for (int j = 0; j < rightDisplayWidth; j++) {
					std::cout << " ";
				}
			}
			std::cout << "|";
		}

		//display bottom
		std::string bottomMessage = "";
		while (bottomMessage.length() < screenWidth) {
			if (bottomMessage.length() == 0 || bottomMessage.length() == screenWidth - 1 || bottomMessage.length() == leftDisplayWidth + 1) {
				bottomMessage += "+";
			} else if (bottomMessage.length() == leftDisplayWidth + 3) {
				bottomMessage += "[ESC or I - Return]-[R - Remove]-[E - Equip]";
			} else {
				bottomMessage += "-";
			}
		}
		std::cout << bottomMessage;

		//gather input
		input = _getch();

		//parse input
		switch (input) {
			case 'W':
			case 'w':
				selection--;
				if (selection < 0) {
					selection = this->_inventory.size() - 1;
					bottomMargin = this->_inventory.size() - 1 - displayHeight;
					if (bottomMargin < 0)
						bottomMargin = 0;
				}
				break;
			case 'S':
			case 's':
				selection++;
				if (selection > this->_inventory.size() - 1) {
					selection = 0;
					bottomMargin = 0;
				}
				break;
			case 'R':
			case 'r':
				promptRemove(selection);
				totalItems = getTotalItems();
				totalValue = getInventoryValue();
				if (selection > this->_inventory.size() - 1)
					selection = this->_inventory.size() - 1;
				break;
			case 'E':
			case 'e':
				this->equip(this->_inventory[selection], selection);
				if (selection > this->_inventory.size() - 1)
					selection = this->_inventory.size() - 1;
				break;
		}

		if (this->_inventory.size() == 0) {
			clCons::cls();
			clCons::centerVerhor(1, "Your inventory is empty!");
			clCons::paus("");
			return;
		}

		//adjust bottom margin
		if (selection > bottomMargin + displayHeight - 1) {
			bottomMargin++;
		} else if (selection < bottomMargin) {
			bottomMargin = selection;
		}
	}
}
void Player::openCharacter() {

	//configure armor representations
	struct armorPieceRep {
		int location;
		ItemSubType piece;
		std::string display;
	};

	//variables
	int selection = 0;
	int input = 0;
	std::vector<armorPieceRep> armorPieceReps;
	std::vector<ColorString> rightDisplay;
	clCons::cls();

	//display
	while (!(input == 27 || input == 'c' || input == 'C')) {

		//reload pieces	
		armorPieceReps = std::vector<armorPieceRep>();
		for (int i = 1; i <= SUBTYPEWEAPONAFTER; i++) {
			armorPieceReps.push_back(armorPieceRep{ i, ItemSubType(i), "" });
		}
		armorPieceReps.push_back(armorPieceRep{ (int)armorPieceReps.size(), ItemSubType::SWORD, "" });

		//sort equipment displays
		for (int i = 0; i < this->_equipment.size(); i++) {
			for (int j = 0; j < armorPieceReps.size(); j++) {
				if (_equipment[i].getSubType() == armorPieceReps[j].piece) {
					armorPieceReps[j].display = _equipment[i].getName() + " [" + to_string(_equipment[i].getLevel()) + "]";
				} else if (_equipment[i].getSubType() > ItemSubType(SUBTYPEWEAPONAFTER)) {
					armorPieceReps[armorPieceReps.size() - 1].display = _equipment[i].getName() + " [" + to_string(_equipment[i].getLevel()) + "]";
				}
			}
		}

		//fill in empty slots
		for (int i = 0; i < SUBTYPEWEAPONAFTER; i++) {
			if (armorPieceReps[i].display == "") {
				armorPieceReps[i].display = clUtil::lowercase("[" + convertItemSubType(armorPieceReps[i].piece) + "]");
			}
		}
		if (armorPieceReps[(int)armorPieceReps.size() - 1].display == "")
			armorPieceReps[(int)armorPieceReps.size() - 1].display = "[weapon]";

		//screen properties
		int leftDisplayWidth = 10;
		int LEFT_DISPLAY_EXTRAS = 6;
		int consoleWidth = clCons::getConsoleWidth();
		int rightDisplayWidth = consoleWidth - leftDisplayWidth - LEFT_DISPLAY_EXTRAS - 2;
		for (int i = 0; i < armorPieceReps.size(); i++) {
			if (armorPieceReps[i].display.length() > leftDisplayWidth) {
				leftDisplayWidth = armorPieceReps[i].display.length();
			}
		}

		//pre-display
		int oldConsoleWidth = consoleWidth;
		consoleWidth = clCons::getConsoleWidth();
		clCons::clsNew();
		rightDisplayWidth = consoleWidth - leftDisplayWidth - LEFT_DISPLAY_EXTRAS - 2;
		clCons::centerVer(armorPieceReps.size() + 6);

		//top display
		for (int k = 0; k < leftDisplayWidth + LEFT_DISPLAY_EXTRAS; k++) {
			if (k == 0) {
				std::cout << "  +";
			} else if (k == leftDisplayWidth + LEFT_DISPLAY_EXTRAS - 1) {
				std::cout << "+";
			} else {
				std::cout << "-";
			}
		}

		//set right display
		rightDisplay = std::vector<ColorString>();
		ColorString toDisplay;

		ColorManager::csout(ColorString(clCons::centerStringIn(rightDisplayWidth, "--==[ " + this->getName() + " ]==--"), ColorFlag::BLUE, true, 1, clUtil::wordCount(this->getName()), true));
		std::cout << std::endl;
		rightDisplay.push_back(ColorString("", ColorFlag::GRAY, 0, 0, false));
		rightDisplay.push_back(ColorString(clCons::centerStringIn(rightDisplayWidth, "Level: " + to_string(this->_level)), ColorFlag::PURPLE, true, 1));
		rightDisplay.push_back(ColorString(clCons::centerStringIn(rightDisplayWidth, "Health: " + to_string(this->getHealth()) + "/" + to_string(this->getMaxHealth())), ColorFlag::RED, true, 1));
		rightDisplay.push_back(ColorString(clCons::centerStringIn(rightDisplayWidth, "Armor: " + to_string(this->getTotalArmor())), ColorFlag::GREEN, true, 1));
		rightDisplay.push_back(ColorString(clCons::centerStringIn(rightDisplayWidth, "Damage: " + to_string((int)this->getTotalDamage())), ColorFlag::GREEN, true, 1));

		//

		for (int i = 0; i < armorPieceReps.size() - (rightDisplay.size() + 1); i++) {
			rightDisplay.push_back(ColorString("", ColorFlag::GRAY, 0, 0));
		}


		//if no piece equipped
		if (armorPieceReps[selection].display == ("[" + clUtil::lowercase(convertItemSubType(armorPieceReps[selection].piece)) + "]") || (armorPieceReps[selection].display == "[weapon]")) {
			rightDisplay.push_back(ColorString("", ColorFlag::GRAY, 0, 0));
			toDisplay = ColorString(clCons::centerStringIn(rightDisplayWidth, "No piece equipped here."), ColorFlag::GRAY, true, 0);
			rightDisplay.push_back(toDisplay);
		}

		//if piece equipped
		else {
			bool equipped = true;
			if (armorPieceReps[selection].piece > ItemSubType(SUBTYPEWEAPONAFTER)) {
				toDisplay = ColorString(clCons::centerStringIn(rightDisplayWidth, "Damage: " + to_string(getWeapon(&equipped).getEffect())), ColorFlag::GRAY, true, 1);
				rightDisplay.push_back(toDisplay);
			} else {
				toDisplay = ColorString(clCons::centerStringIn(rightDisplayWidth, "Armor: " + to_string(getPiece(armorPieceReps[selection].piece, &equipped).getEffect())), ColorFlag::GRAY, true, 1);
				rightDisplay.push_back(toDisplay);
			}
			toDisplay = ColorString(clCons::centerStringIn(rightDisplayWidth, armorPieceReps[selection].display), ColorFlag::GREEN, true, 0, clUtil::wordCount(clCons::centerStringIn(rightDisplayWidth, armorPieceReps[selection].display)) - 1);
			rightDisplay.push_back(toDisplay);
		}

		//middle display
		for (int i = 0; i < armorPieceReps.size(); i++) {

			//left display
			std::cout << "  |";
			if (armorPieceReps[i].display == ("[" + clUtil::lowercase(convertItemSubType(armorPieceReps[i].piece)) + "]") || (armorPieceReps[i].display == "[weapon]")) {
				ColorManager::setForegroundColor(ColorFlag::GRAY, true);
			}
			if (i == selection) {
				std::cout << "-<" << armorPieceReps[i].display << ">-";
			} else {
				std::cout << "  " << armorPieceReps[i].display << "  ";
			}
			if (armorPieceReps[i].display == ("[" + clUtil::lowercase(convertItemSubType(armorPieceReps[i].piece)) + "]") || (armorPieceReps[i].display == "[weapon]")) {
				ColorManager::resetForegroundColor();
			}
			for (int k = 0; k < leftDisplayWidth - armorPieceReps[i].display.length(); k++) {
				std::cout << " ";
			}
			std::cout << "|";

			//right display
			if (rightDisplay.size() > i) {
				if (rightDisplay[i].content != "") {
					ColorManager::csout(rightDisplay[i]);
					for (int j = 0; j < rightDisplayWidth - rightDisplay[i].length(); j++) {
						std::cout << " ";
					}
				} else {
					for (int j = 0; j < rightDisplayWidth; j++) {
						std::cout << " ";
					}
				}
			} else {
				for (int j = 0; j < rightDisplayWidth; j++) {
					std::cout << " ";
				}
			}
		}

		//bottom display
		for (int k = 0; k < leftDisplayWidth + LEFT_DISPLAY_EXTRAS; k++) {
			if (k == 0) {
				std::cout << "  +";
			} else if (k == leftDisplayWidth + LEFT_DISPLAY_EXTRAS - 1) {
				std::cout << "+";
			} else {
				std::cout << "-";
			}
		}

		//experience bar display
		int barWidth = consoleWidth - 6;
		int barFullness = (int)(((double)this->_exp / (double)this->_levels[this->_level - 1].expNeeded) * (double)barWidth);
		std::cout << std::endl;
		clCons::centerHor("[Exp: " + to_string(this->_exp) + "/" + to_string(this->_levels[this->_level - 1].expNeeded) + "]");
		std::cout << std::endl << "  <";
		ColorManager::setForegroundColor(ColorFlag::PURPLE);
		for (int i = 0; i < barWidth; i++) {
			if (i <= barFullness) {
				std::cout << "|";
			} else {
				std::cout << "-";
			}
		}
		ColorManager::resetForegroundColor();
		std::cout << ">" << std::endl << std::endl;
		clCons::centerHor("[ESC or C - Return]-[E - Unequip]-[K - Attributes & Stats]-", '-');

		//grab input
		input = _getch();

		//parse input
		switch (input) {
			case 'W':
			case 'w':
				selection--;
				if (selection < 0) {
					selection = armorPieceReps.size() - 1;
				}
				break;
			case 'S':
			case 's':
				selection++;
				if (selection > armorPieceReps.size() - 1) {
					selection = 0;
				}
				break;
			case 'x':
			case 'X':
				this->addXp(1);
				break;
			case 'e':
			case 'E':
				this->unequip(armorPieceReps[selection].piece);
				break;
			case 'k':
			case 'K':
				this->openAttributes();
				break;
		}
	}
}
void Player::openAttributes() {

	//variables
	int input = 0;
	int selection = 0;
	ColorString emptyLine = ColorString("", ColorFlag::GRAY, 0, 0, false);

	//input
	while (input != '\033' && input != 'k' && input != 'K') {

		//calculate display
		std::vector<ColorString> toDisplay;
		toDisplay.push_back(ColorString("Attribute Points: " + to_string(this->_attributePoints), ColorFlag::GRAY, true, 0, 3, true));
		toDisplay.push_back(emptyLine);
		std::string enduranceContent = "Endurance: " + to_string(this->_endurance), spiritContent = "Spirit: " + to_string(this->_spirit), capabilityContent = "Capability: " + to_string(this->_capability);
		if (selection == 0)
			enduranceContent = "-[" + enduranceContent + "]-";
		if (selection == 1)
			spiritContent = "-[" + spiritContent + "]-";
		if (selection == 2)
			capabilityContent = "-[" + capabilityContent + "]-";
		toDisplay.push_back(ColorString(enduranceContent, ColorFlag::CYAN, true, 1, 1, true));
		toDisplay.push_back(ColorString(spiritContent, ColorFlag::CYAN, true, 1, 1, true));
		toDisplay.push_back(ColorString(capabilityContent, ColorFlag::CYAN, true, 1, 1, true));
		toDisplay.push_back(emptyLine);
		toDisplay.push_back(ColorString("  Effective Armor (" + to_string(this->getEffectiveArmor()) + ") x Effective Endurance (" + to_string(this->getEffectiveEndurance()) + ")", ColorFlag::GRAY, true, 0, 100, true));
		toDisplay.push_back(ColorString("+ Base Health (" + to_string(CalcManager::getBaseHealth(this->_level)) + ")", ColorFlag::GRAY, true, 0, 100, true));
		toDisplay.push_back(ColorString("= Total HP: " + to_string(this->getMaxHealth()), ColorFlag::RED, true, 0, 100, true));
		toDisplay.push_back(emptyLine);
		toDisplay.push_back(ColorString("  Base Regeneration (" + to_string(this->getLevel()) + ") x Effective Spirit (" + to_string(this->getEffectiveSpirit()) + ")", ColorFlag::GRAY, true, 0, 100, true));
		toDisplay.push_back(ColorString("= Total Regeneration: " + to_string((int)this->getRegeneration()), ColorFlag::CYAN, true, 0, 100, true));
		toDisplay.push_back(emptyLine);
		toDisplay.push_back(ColorString("  Effective Capability (" + to_string(this->getEffectiveCapability()) + ") x Weapon Damage (" + to_string(this->getWeapon()->getEffect()) + ") x Weapon Proficiency (" + to_string(this->getWeaponProficiency()) + ")", ColorFlag::GRAY, true, 0, 100, true));
		toDisplay.push_back(ColorString("= Total Damage: " + to_string((int)this->getTotalDamage()), ColorFlag::PURPLE, true, 0, 100, true));

		//display
		clCons::cls();
		clCons::centerVerhor(toDisplay.size() + 4, "[Attributes & Stats]", '-', 2);
		for (int i = 0; i < toDisplay.size(); i++) {
			ColorManager::centerHorColor(toDisplay[i]);
		}
		std::cout << std::endl << std::endl;
		clCons::centerHor("[E - Spend Point]-[Esc - Return]", '-');

		//gather input
		input = _getch();
		switch (input) {
			case 'w':
			case 'W':
				selection--;
				if (selection < 0) {
					selection = 2;
				}
				break;
			case 's':
			case 'S':
				selection++;
				if (selection > 2) {
					selection = 0;
				}
				break;
			case 'e':
			case 'E':
				if (this->_attributePoints > 0) {
					this->addAttribute(selection);
				}
		}
	}
	clCons::cls();
}
void Player::promptRemove(int atIndex) {

	//variables
	std::vector<std::string> removeOptions;
	removeOptions.push_back("Yes");
	removeOptions.push_back("Nevermind");

	//number chooser
	int quantity = 1;
	if (this->_inventory[atIndex].getQuantity() > 1)
		quantity = clCons::numberChooser("How Many?", 1, this->_inventory[atIndex].getQuantity());

	//pre-menu
	clCons::cls();
	if (clCons::menu("[Are you sure you want to remove " + to_string(quantity) + " " + this->_inventory[atIndex].getName() + "(s)?]", "", removeOptions) == 1) {
		removeFromInventory(atIndex, quantity);
	}
}
void Player::removeFromInventory(int atIndex, int quantity) {
	if (quantity == this->_inventory[atIndex].getQuantity())
		this->_inventory.erase(this->_inventory.begin() + atIndex);
	else
		this->_inventory[atIndex].remove(quantity);
}
void Player::addXp(int thisMuch) {
	this->_exp += thisMuch;
	while (this->_exp >= this->_levels[this->_level - 1].expNeeded) {
		this->_exp -= this->_levels[this->_level - 1].expNeeded;
		this->_level++;
		clCons::cls();
		clCons::centerVer(4);
		clCons::centerHor(this->getName() + " has become level " + to_string(this->getLevel()) + "!", ' ', 2);
		clCons::centerHor("+ " + to_string(CalcManager::getBaseHealth(this->getLevel()) - CalcManager::getBaseHealth(this->getLevel() - 1)) + " HP");
		clCons::centerHor("+ 2 Attribute Points");
		clCons::paus("");
		clCons::cls();
		this->_attributePoints += 2;
	}
}
void Player::equip(Item equipThis, int index) {

	//prevalidate
	if (!(equipThis.getType() == WEAPON || equipThis.getType() == ARMOR))
		return;
	if (equipThis.getLevel() > this->_level && index != -1) {
		clCons::cls();
		clCons::centerVerhor(1, "You are too low level to equip this item");
		clCons::paus("");
		return;
	} else if ((equipThis.getLevel() > this->_level) && index == -1) {
		this->addItemToInventory(equipThis);
		clCons::cls();
		clCons::centerVerhor(1, "You were given [" + equipThis.getName() + "]");
		clCons::paus("");
		return;
	}

	//unequip current piece
	this->unequip(equipThis.getSubType());

	//equip new piece
	this->_equipment.push_back(equipThis);

	//remove from inventory
	if (index != -1) {
		this->_inventory.erase(this->_inventory.begin() + index);
	}
}
void Player::unequip(ItemSubType thisPiece) {

	//unequip armor
	if (thisPiece <= ItemSubType(SUBTYPEWEAPONAFTER)) {
		for (int i = 0; i < this->_equipment.size(); i++) {
			if (this->_equipment[i].getSubType() == thisPiece) {
				this->addItemToInventory(this->_equipment[i]);
				this->_equipment.erase(this->_equipment.begin() + i);
			}
		}

	//unequip weapon
	} else {
		for (int i = 0; i < this->_equipment.size(); i++) {
			if (this->_equipment[i].getSubType() > ItemSubType(SUBTYPEWEAPONAFTER)) {
				this->addItemToInventory(this->_equipment[i]);
				this->_equipment.erase(this->_equipment.begin() + i);
			}
		}
	}
}
void Player::updateHealth() {
	if (this->_health > this->getMaxHealth()) {
		this->_health = this->getMaxHealth();
	}
}
bool Player::getPiece(ItemSubType thisType, Item* thisItem) {
	for (int i = 0; i < this->_equipment.size(); i++) {
		if (this->_equipment[i].getSubType() == thisType) {
			*thisItem = this->_equipment[i];
			return true;
		} else if (this->_equipment[i].getSubType() > ItemSubType(SUBTYPEWEAPONAFTER) && thisType > ItemSubType(SUBTYPEWEAPONAFTER)) {
			*thisItem = this->_equipment[i];
			return true;
		}
	}
	return false;
}
Item Player::getPiece(ItemSubType thisType, bool* equipped) {
	for (int i = 0; i < this->_equipment.size(); i++) {
		if (this->_equipment[i].getSubType() == thisType) {
			return this->_equipment[i];
		} else if (this->_equipment[i].getSubType() > ItemSubType(SUBTYPEWEAPONAFTER) && (thisType > ItemSubType(SUBTYPEWEAPONAFTER))) {
			return this->_equipment[i];
		}
	}

	*equipped = false;
	return Item();
}
Item Player::getWeapon(bool* equipped) {
	*equipped = false;
	for (int i = 0; i < this->_equipment.size(); i++) {
		if (this->_equipment[i].getSubType() > ItemSubType(SUBTYPEWEAPONAFTER)) {
			*equipped = true;
			return this->_equipment[i];
		}
	}
}
bool Player::getWeapon(Item* thisItem) {
	for (int i = 0; i < this->_equipment.size(); i++) {
		if (this->_equipment[i].getSubType() > ItemSubType(SUBTYPEWEAPONAFTER)) {
			*thisItem = this->_equipment[i];
			return true;
		}
	}
}
//RETURNS - whether or not player died after taking damage
bool Player::takeDamage(int thisMuch) {
	this->_health -= thisMuch;
	if (this->_health < 1) {
		this->_health = 0;
		return true;
	} else {
		return false;
	}
}
ColorString Player::createHealthBar(int width) {

	//variables
	ColorString healthBar;
	std::string text = "HP: " + to_string(this->_health) + "/" + to_string(this->getMaxHealth());
	std::string content = text + " <";
	int contentWidth = width - 2 - text.length();
	int fullness = (((float)this->_health / (float)this->getMaxHealth()) * (float)contentWidth);

	//hard Borders
	if (this->_health == 0) {
		fullness = 0;
	} else if (this->_health == this->getMaxHealth()) {
		fullness = contentWidth;
	}

	//fill String
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