#include "player.h"

//Constructor
void Player::constructPlayerExtras(int level, int gold, int health, int exp, std::vector<Level> levels, std::vector<Item> inventory, std::vector<Item> equipment)
{
	//Construct Player Extras
	this->_level = level;
	this->_gold = gold;
	this->_health = health;
	this->_exp = exp;
	this->_levels = levels;
	this->_inventory = inventory;
	this->_equipment = equipment;

	//Check Health
	if (this->_health < 0) {
		this->_health = this->getMaxHealth();
	}
}
void Player::constructPlayerAttributes(int attributePoints, int endurance, int spirit, int capability)
{
	this->_attributePoints = attributePoints;
	this->_endurance = endurance;
	this->_spirit = spirit;
	this->_capability = capability;
}

//Getters
Item* Player::getWeapon()
{
	for (int i = 0; i < this->_equipment.size(); i++) {
		if (this->_equipment[i].getSubType() > ItemSubType(SUBTYPEWEAPONAFTER)) {
			return &this->_equipment[i];
		}
	}
}
int Player::getWeaponLevel(ItemSubType thisWeapon)
{
	for (int i = 0; i < this->_weaponLevels.size(); i++) {
		if (thisWeapon == this->_weaponLevels[i].weapon) {
			return this->_weaponLevels[i].level;
		}
	}
	return -1;
}
int Player::getMaxHealth()
{
	//Get Base Health
	int health = CalcManager::getBaseHealth(this->_level);

	//Calculate Effective Armor/Endurance
	float effectiveArmor = this->getEffectiveArmor();
	float effectiveEndurance = this->getEffectiveEndurance();

	//Factor in Effective Armor/Endurance
	health += (float)(effectiveArmor * effectiveEndurance) + 0.5f;

	//Return Health
	return health;
}
float Player::getWeaponProficiency()
{
	return 1.0f + ((((float)this->getWeaponLevel(this->getWeapon()->getSubType())) - 1) / (float)100);
}

//Totaling Functions
int Player::getTotalItems()
{
	//Calculate and return total item count
	int totalItems = 0;
	for (int i = 0; i < this->_inventory.size(); i++) {
		totalItems += this->_inventory[i].getQuantity();
	}
	return totalItems;
}
int Player::getTotalArmor()
{
	int totalArmor = 0;
	for (int i = 0; i < this->_equipment.size(); i++) {
		if (this->_equipment[i].getType() != ItemType::WEAPON) {
			totalArmor += this->_equipment[i].getEffect();
		}
	}
	return totalArmor;
}
int Player::getRegeneration()
{
	return (float)this->_level * ((float)((this->_spirit * 2) + 100)/ 100);
}
float Player::getTotalDamage(bool withCapability, bool withProficiency)
{
	int dmg = 0;
	for (int i = 0; i < this->_equipment.size(); i++) {
		if (this->_equipment[i].getType() == ItemType::WEAPON) {
			dmg = this->_equipment[i].getEffect();
			if (withCapability) {
				dmg *= ((float)(2 * this->_capability) + 100) / 100;
			}
			if (withProficiency) {
				dmg *= this->getWeaponProficiency();
			}
		}
	}
	return dmg;
}
int Player::getInventoryValue()
{
	int totalValue = 0;
	for (int i = 0; i < this->_inventory.size(); i++) {
		totalValue += (this->_inventory[i].getValue() * this->_inventory[i].getQuantity());
	}
	return totalValue;
}

//Setters
void Player::addItemToInventory(Item thisItem)
{
	bool existsAlready = false;
	for (int i = 0; i < this->_inventory.size(); i++) {
		if (thisItem == this->_inventory[i]) {
			this->_inventory[i].add(thisItem.getQuantity());
			existsAlready = true;
			if (this->_inventory[i].getQuantity() > MAX_STACK_SIZE) {
				thisItem.setQuantity(this->_inventory[i].getQuantity() - MAX_STACK_SIZE);
				this->_inventory[i].setQuantity(99);
				this->_inventory.push_back(thisItem);
			}
		}
	}
	if (!existsAlready) 
		this->_inventory.push_back(thisItem);
}
void Player::addWeaponLevel(ItemSubType thisType)
{
	for (int i = 0; i < this->_weaponLevels.size(); i++) {
		if (this->_weaponLevels[i].weapon == thisType) {
			this->_weaponLevels[i].level++;
		}
	}
}
void Player::addAttribute(int choice)
{
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
void Player::heal(int thisMuch)
{
	this->_health += thisMuch;
	if (this->_health > this->getMaxHealth()) {
		this->_health = this->getMaxHealth();
	}
}

//Functions
void Player::passiveUpdate()
{
	//Regenerate Health
	if (this->_health != this->getMaxHealth()) {
		this->_health += this->getRegeneration();
		if (this->_health > this->getMaxHealth()) {
			this->_health = this->getMaxHealth();
		}
	}
}
void Player::openInventory()
{
	//If Inventory Is Empty
	if (this->_inventory.size() == 0) {
		clCons::cls();
		clCons::centerVerhor(1, "Your inventory is empty!");
		clCons::paus("");
		return;
	}

	//Grab Screen Properties
	int screenWidth = clCons::getConsoleWidth();
	int screenHeight = clCons::getConsoleHeight();

	//Extra Variables
	int input = 0;
	int selection = 0;

	//Dual Display Vectors
	std::vector<std::string> leftDisplay;
	std::vector<ColorString> rightDisplay;
	ColorString blankString;
	blankString.content = "";

	//Display Variables
	int leftDisplayWidth = 10;
	int LEFT_DISPLAY_EXTRAS = 12;
	int rightDisplayWidth;
	int COMPLETE_DISPLAY_EXTRAS = 3;
	int displayHeight = 10;
	int DISPLAY_HEIGHT_EXTRAS = 4;

	//Determine Left Display Width
	for (int i = 0; i < this->_inventory.size(); i++) {
		int nextLength = this->_inventory[i].getName().length();
		if (nextLength > (leftDisplayWidth - LEFT_DISPLAY_EXTRAS)) {
			leftDisplayWidth = this->_inventory[i].getName().size() + LEFT_DISPLAY_EXTRAS;
		}
	}

	//Inventory Display Settings
	int bottomMargin = 0;

	//Inventory Stats
	int totalItems = getTotalItems();
	int totalValue = getInventoryValue();

	//Pre-Loop
	clCons::cls();

	//Loop
	while (!(input == 27 || input == 'b' || input == 'B')) {
		
		//Determine Left Display Width
		for (int i = 0; i < this->_inventory.size(); i++) {
			int nextLength = this->_inventory[i].getName().length();
			if (nextLength > (leftDisplayWidth - LEFT_DISPLAY_EXTRAS)) {
				leftDisplayWidth = this->_inventory[i].getName().size() + LEFT_DISPLAY_EXTRAS;
			}
		}

		//Determine Adaptable Display Variables
		screenWidth = clCons::getConsoleWidth();
		int oldScreenHeight = screenHeight;
		screenHeight = clCons::getConsoleHeight();
		if (oldScreenHeight != screenHeight)
			clCons::cls();
		rightDisplayWidth = screenWidth - leftDisplayWidth - COMPLETE_DISPLAY_EXTRAS;
		displayHeight = 12;
		if (screenHeight < displayHeight + DISPLAY_HEIGHT_EXTRAS) {
			displayHeight = screenHeight - DISPLAY_HEIGHT_EXTRAS;
		} else {
			displayHeight = 12;
		}
		
		//Check Selection
		if (selection >= bottomMargin + displayHeight) {
			selection = bottomMargin + displayHeight - 1;
		}

		//Set Left Display------------------------------------------------------------------------------------
		leftDisplay = std::vector<std::string>();
		std::string nextLine = "";
 		for (int i = bottomMargin; i < this->_inventory.size(); i++) {	
			nextLine = "";
			if (i + 1 <= 9) {
				nextLine  += to_string(i + 1) + " ";
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

		//Set Right Display-----------------------------------------------------------------------------------
		rightDisplay = std::vector<ColorString>();
		ColorString toDisplay;	
		for (int i = 0; i < displayHeight / 4; i++) {
			rightDisplay.push_back(blankString);
		}	

		//Item Name
		toDisplay = ColorString(clCons::centerStringIn(rightDisplayWidth, "-=[ " + this->_inventory[selection].getName() + " ]=-"), ColorFlag::GREEN, true, 1, clUtil::wordCount(clCons::centerStringIn(rightDisplayWidth, "-=[ " + this->_inventory[selection].getName() + " ]=-")) - 2, true);
		rightDisplay.push_back(toDisplay);
		rightDisplay.push_back(blankString);

		//WEAPON
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

		//ARMOR
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

		//OTHER
		} else {
			toDisplay = ColorString(clCons::centerStringIn(rightDisplayWidth, "Type: " + convertItemType(this->_inventory[selection].getType())), ColorFlag::GRAY, true, 1);
			rightDisplay.push_back(toDisplay);
			toDisplay = ColorString(clCons::centerStringIn(rightDisplayWidth, "Value: " + to_string(this->_inventory[selection].getValue()) + "(" + to_string(this->_inventory[selection].getValue() * this->_inventory[selection].getQuantity()) + ")g"), ColorFlag::YELLOW, true, 1);
			rightDisplay.push_back(toDisplay);
		}

		//Pre-Display-----------------------------------------------------------------------------------------
		clCons::centerVerhor(displayHeight + DISPLAY_HEIGHT_EXTRAS, this->getName() + "'s Inventory", ' ', 2);

		//Display Top
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

		//Display Middle
		for (int i = 0; i < displayHeight; i++) {
			//Display Left
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
			
			//Display Right
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

		//Display Bottom
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

		//Gather Input
		input = _getch();

		//Parse Input
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

		//Adjust Bottom Margin
		if (selection > bottomMargin + displayHeight - 1) {
			bottomMargin++;
		} else if (selection < bottomMargin) {
			bottomMargin = selection;
		}
	}
}
void Player::openCharacter()
{
	//Configure Armor Representations
	struct armorPieceRep
	{
		int location;
		ItemSubType piece;
		std::string display;
	};

	//Variables
	int selection = 0;
	int input = 0;
	std::vector<armorPieceRep> armorPieceReps;	
	std::vector<ColorString> rightDisplay;
	clCons::cls();

	//Display
	while (!(input == 27 || input == 'c' || input == 'C')) {

		//Reload Pieces	
		armorPieceReps = std::vector<armorPieceRep>();
		for (int i = 1; i <= SUBTYPEWEAPONAFTER; i++) {
			armorPieceReps.push_back(armorPieceRep{ i, ItemSubType(i), ""} );
		}
		armorPieceReps.push_back(armorPieceRep{ (int)armorPieceReps.size(), ItemSubType::SWORD, ""} );

		//Sort Equipment Displays
		for (int i = 0; i < this->_equipment.size(); i++) {
			for (int j = 0; j < armorPieceReps.size(); j++) {
				if (_equipment[i].getSubType() == armorPieceReps[j].piece) {
					armorPieceReps[j].display = _equipment[i].getName() + " [" + to_string(_equipment[i].getLevel()) + "]";
				} else if (_equipment[i].getSubType() > ItemSubType(SUBTYPEWEAPONAFTER)) {
					armorPieceReps[armorPieceReps.size() - 1].display = _equipment[i].getName() + " [" + to_string(_equipment[i].getLevel()) + "]";
				}
			}
		}
	
		//Fill In Empty Slots
		for (int i = 0; i < SUBTYPEWEAPONAFTER; i++) {
			if (armorPieceReps[i].display == "") {
				armorPieceReps[i].display = clUtil::lowercase("[" + convertItemSubType(armorPieceReps[i].piece) +"]");
			}
		}
		if (armorPieceReps[(int)armorPieceReps.size() - 1].display == "")
			armorPieceReps[(int)armorPieceReps.size() - 1].display = "[weapon]";

		//Screen Properties
		int leftDisplayWidth = 10;
		int LEFT_DISPLAY_EXTRAS = 6;
		int consoleWidth = clCons::getConsoleWidth();
		int rightDisplayWidth = consoleWidth - leftDisplayWidth - LEFT_DISPLAY_EXTRAS - 2;
		for (int i = 0; i < armorPieceReps.size(); i++) {
			if (armorPieceReps[i].display.length() > leftDisplayWidth) {
				leftDisplayWidth = armorPieceReps[i].display.length();
			} 
		}

		//Pre Display
		int oldConsoleWidth = consoleWidth;
		consoleWidth = clCons::getConsoleWidth();
		clCons::clsNew();
		rightDisplayWidth = consoleWidth - leftDisplayWidth - LEFT_DISPLAY_EXTRAS - 2;
		clCons::centerVer(armorPieceReps.size() + 6);

		//Top Display
		for (int k = 0; k < leftDisplayWidth + LEFT_DISPLAY_EXTRAS; k++) {
			if (k == 0) {
				std::cout << "  +";
			} else if (k == leftDisplayWidth + LEFT_DISPLAY_EXTRAS - 1) {
				std::cout << "+";
			} else {
				std::cout << "-";
			}
		}

		//Set Right Display
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


		//IF NO PIECE EQUIPPED
		if (armorPieceReps[selection].display == ("[" + clUtil::lowercase(convertItemSubType(armorPieceReps[selection].piece)) + "]") || (armorPieceReps[selection].display == "[weapon]")) {
			rightDisplay.push_back(ColorString("", ColorFlag::GRAY, 0, 0));			
			toDisplay = ColorString(clCons::centerStringIn(rightDisplayWidth, "No piece equipped here."), ColorFlag::GRAY, true, 0);
			rightDisplay.push_back(toDisplay);
		}
		//IF PIECE EQUIPPED
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

		//Middle Display
		for (int i = 0; i < armorPieceReps.size(); i++) {

			//Left Display
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

			//Right Display
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

		//Bottom Display
		for (int k = 0; k < leftDisplayWidth + LEFT_DISPLAY_EXTRAS; k++) {
			if (k == 0) {
				std::cout << "  +";
			} else if (k == leftDisplayWidth + LEFT_DISPLAY_EXTRAS - 1) {
				std::cout << "+";
			} else {
				std::cout << "-";
			}
		}

		//Exp Bar Display
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

		//Grab Input
		input = _getch();

		//Parse Input
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
void Player::openAttributes()
{
	//Variables
	int input = 0;
	int selection = 0;
	ColorString emptyLine = ColorString("", ColorFlag::GRAY, 0, 0, false);

	//Input
	while (input != '\033' && input != 'k' && input != 'K') {

		//Calculate Display
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
		toDisplay.push_back(ColorString("  Base Regeneration (" + to_string(this->getLevel()) + ") x Effective Spirit (" + to_string((float)((this->_spirit * 2) + 100)/ 100) + ")", ColorFlag::GRAY, true, 0, 100, true));	
		toDisplay.push_back(ColorString("= Total Regeneration: " + to_string((int)this->getRegeneration()), ColorFlag::CYAN, true, 0, 100, true));		
		toDisplay.push_back(emptyLine);
		toDisplay.push_back(ColorString("  Effective Capability (" + to_string((float)this->getTotalDamage(true, false) - (float)this->getTotalDamage(false, false)) + ") + Weapon Damage (" + to_string(this->getWeapon()->getEffect()) + ") x Weapon Proficiency (" + to_string(this->getWeaponProficiency()) + ")", ColorFlag::GRAY, true, 0, 100, true));	
		toDisplay.push_back(ColorString("= Total Damage: " + to_string((int)this->getTotalDamage()), ColorFlag::PURPLE, true, 0, 100, true));

		//Display
		clCons::cls();
		clCons::centerVerhor(toDisplay.size() + 4, "[Attributes & Stats]", '-', 2);
		for (int i = 0; i < toDisplay.size(); i++) {
			ColorManager::centerHorColor(toDisplay[i]);
		}
		std::cout << std::endl << std::endl;
		clCons::centerHor("[E - Spend Point]-[Esc - Return]", '-');

		//Gather Input
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
void Player::promptRemove(int atIndex)
{
	//Variables
	std::vector<std::string> removeOptions;
	removeOptions.push_back("Yes");
	removeOptions.push_back("Nevermind");

	//Number Chooser
	int quantity = 1;
	if (this->_inventory[atIndex].getQuantity() > 1)
		quantity = clCons::numberChooser("How Many?", 1, this->_inventory[atIndex].getQuantity());

	//Pre-Menu
	clCons::cls();
	if (clCons::menu("[Are you sure you want to remove " + to_string(quantity) + " " + this->_inventory[atIndex].getName() + "(s)?]", "", removeOptions) == 1) {
		removeFromInventory(atIndex, quantity);
	}
}
void Player::removeFromInventory(int atIndex, int quantity)
{
	if (quantity == this->_inventory[atIndex].getQuantity())
		this->_inventory.erase(this->_inventory.begin() + atIndex);
	else
		this->_inventory[atIndex].remove(quantity);
}
void Player::addXp(int thisMuch)
{
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
void Player::equip(Item equipThis, int index)
{
	//Prevalidate
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

	//Unequip Current Piece
	this->unequip(equipThis.getSubType());

	//Equip New Piece
	this->_equipment.push_back(equipThis);

	//Remove From Inventory
	if (index != -1) {
		this->_inventory.erase(this->_inventory.begin() + index);
	}
}
void Player::unequip(ItemSubType thisPiece)
{
	//Unequip Armor
	if (thisPiece <= ItemSubType(SUBTYPEWEAPONAFTER)) {
		for (int i = 0; i < this->_equipment.size(); i++) {
			if (this->_equipment[i].getSubType() == thisPiece) {
				this->addItemToInventory(this->_equipment[i]);
				this->_equipment.erase(this->_equipment.begin() + i);
			}
		}

	//Unequip Weapon
	} else {
		for (int i = 0; i < this->_equipment.size(); i++) {
			if (this->_equipment[i].getSubType() > ItemSubType(SUBTYPEWEAPONAFTER)) {
				this->addItemToInventory(this->_equipment[i]);
				this->_equipment.erase(this->_equipment.begin() + i);
			}			
		}
	}
}
void Player::updateHealth()
{
	if (this->_health > this->getMaxHealth()) {
		this->_health = this->getMaxHealth();
	}
}
bool Player::getPiece(ItemSubType thisType, Item* thisItem)
{
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
Item Player::getPiece(ItemSubType thisType, bool* equipped)
{
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
Item Player::getWeapon(bool* equipped)
{
	*equipped = false;
	for (int i = 0; i < this->_equipment.size(); i++) {
		if (this->_equipment[i].getSubType() > ItemSubType(SUBTYPEWEAPONAFTER)) {
			*equipped = true;
			return this->_equipment[i];
		}
	}
}
bool Player::getWeapon(Item* thisItem)
{
	for (int i = 0; i < this->_equipment.size(); i++) {
		if (this->_equipment[i].getSubType() > ItemSubType(SUBTYPEWEAPONAFTER)) {
			*thisItem = this->_equipment[i];
			return true;
		}
	}
}
bool Player::takeDamage(int thisMuch)
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
ColorString Player::createHealthBar(int width)
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
	} else if (this->_health == this->getMaxHealth()) {
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