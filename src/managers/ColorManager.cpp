#include "colorManager.h"

//Variables
WORD ColorManager::Attributes = 0;
WORD ColorManager::Background = 0;

//Color Functions
void ColorManager::setForegroundColor(ColorFlag thisColor, bool intense)
{	
	//Get Console Handle
	HANDLE hstdout = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(hstdout, &csbi);

	//Store Previous Attributes	
	ColorManager::Attributes = csbi.wAttributes;

	//Set Color
	switch (thisColor) {
		//RGB
	case ColorFlag::RED:
		if (intense)
			SetConsoleTextAttribute(hstdout, FOREGROUND_RED | FOREGROUND_INTENSITY | ColorManager::Background);
		else
			SetConsoleTextAttribute(hstdout, FOREGROUND_RED | ColorManager::Background);
		break;
	case ColorFlag::BLUE:
		if (intense)
			SetConsoleTextAttribute(hstdout, FOREGROUND_BLUE | FOREGROUND_INTENSITY | ColorManager::Background);
		else
			SetConsoleTextAttribute(hstdout, FOREGROUND_BLUE | ColorManager::Background);
		break;
	case ColorFlag::GREEN:
		if (intense)
			SetConsoleTextAttribute(hstdout, FOREGROUND_GREEN | FOREGROUND_INTENSITY | ColorManager::Background);
		else
			SetConsoleTextAttribute(hstdout, FOREGROUND_GREEN | ColorManager::Background);
		break;

		//Combinations
	case ColorFlag::YELLOW:
		if (intense)
			SetConsoleTextAttribute(hstdout, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY | ColorManager::Background);
		else
			SetConsoleTextAttribute(hstdout, FOREGROUND_RED | FOREGROUND_GREEN | ColorManager::Background);
		break;
	case ColorFlag::CYAN:
		if (intense)
			SetConsoleTextAttribute(hstdout, FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY | ColorManager::Background);
		else
			SetConsoleTextAttribute(hstdout, FOREGROUND_GREEN | FOREGROUND_BLUE | ColorManager::Background);
		break;
	case ColorFlag::PURPLE:
		if (intense)
			SetConsoleTextAttribute(hstdout, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY | ColorManager::Background);
		else
			SetConsoleTextAttribute(hstdout, FOREGROUND_RED | FOREGROUND_BLUE | ColorManager::Background);
		break;
	case ColorFlag::GRAY:
		SetConsoleTextAttribute(hstdout, FOREGROUND_INTENSITY | ColorManager::Background);
		break;
	};
}
void ColorManager::resetForegroundColor()
{
	//Reset color to before last set
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), ColorManager::Attributes);
}
void ColorManager::centerHorColor(ColorFlag thisColor, bool intense, std::string toDisplay, char spaces, int afterlines)
{
	//Set Color
	setForegroundColor(thisColor, intense);

	//Display
	clCons::centerHor(toDisplay, spaces, afterlines);

	//Reset Color;
	resetForegroundColor();
}
void ColorManager::centerHorColor(ColorString toDisplay, char spaces, int afterlines)
{
	int width = clCons::getConsoleWidth();
	if (toDisplay.length() > width) {
		cout << toDisplay.content;
		return;
	}
	int beforeSpaces = (int)((width - (double)toDisplay.length()) / 2);
	for (int i = 0; i < beforeSpaces; i++) {
		cout << spaces;
	}
	csout(toDisplay);
	for (int j = 0; j < width - beforeSpaces - toDisplay.length(); j++) {
		cout << spaces;
	}
	for (int k = 0; k < afterlines; k++) {
		cout << endl;
	}
}
void ColorManager::displayWithColor(ColorFlag thisColor, std::string toDisplay, bool endLine)
{
	setForegroundColor(thisColor);
	std::cout << toDisplay;
	if (endLine) {
		std::cout << std::endl;
	}
	resetForegroundColor();
}
void ColorManager::csout(ColorString display)
{
	bool colorReverted = false;

	//Word Type Display
	if (display.wordTypeDisplay) {
		int atWord = 0;
		int coloringFor = 0;
		bool setToColor = false;
		bool hasBegun = false;
		for (int i = 0; i < display.length(); i++) {
			if (display.content[i] != ' ' && !hasBegun) {
				hasBegun = true;
			}
			if (atWord == display.wordBegin && !setToColor) {
				setForegroundColor(display.color);
				setToColor = true;
			}
			if (display.content[i] == ' ' && hasBegun) {
				atWord++;
				if (setToColor) {
					coloringFor++;
				}
			}
			if (coloringFor == display.wordLength && setToColor) {
				setToColor = false;
				resetForegroundColor();
				colorReverted = true;
			}
			std::cout << display.content[i];
		}

	//Normal Type Display
	} else {
		for (int i = 0; i < display.length(); i++) {
			if (i == display.colorBegin) {
				setForegroundColor(display.color, display.intense);
			} else if (i == display.colorEnd) {
				resetForegroundColor();
				colorReverted = true;
			}
			std::cout << display.content[i];
		}
	}
	if (!colorReverted) {
		resetForegroundColor();
	}
}
int ColorManager::menu(ColorFlag selectColor, string topMsg, ColorFlag topColor, string bottomMsg, ColorFlag bottomColor, vector<string> menuItems, char compliment, char returnInput, int returnOption)
{
		/*Creates a menu UI, returns what choice is chosen by user*/
		int selection = 1;
		char input;
		while (1) {
			if (clOption::USE_CLS_NEW) {
				clCons::clsNew();
			} else {
				clCons::cls();
			}
			clCons::centerVer(menuItems.size() + 4);
			ColorManager::centerHorColor(ColorString(topMsg, topColor, true, 0, 5000, true), compliment, 1);
			cout << endl;
			for (int i = 0; i < menuItems.size(); i++) {
				if (selection == i + 1) {
					ColorManager::centerHorColor(ColorString(">- " + menuItems[i] + " -<", selectColor, true, 0));
				} else {
					clCons::centerHor(menuItems[i]);
				}
			}
			cout << endl << endl;
			ColorManager::centerHorColor(ColorString(bottomMsg, bottomColor, true, 0, 5000, true), compliment, 0);

			input = _getch();
			if (input == 13 || input == 32) {
				return selection;
			} else if (input == 'w' || input == 'W') {
				if (selection > 1) {
					selection--;
				} else {
					selection = menuItems.size();
				}
			} else if (input == 's' || input == 'S') {
				if (selection < menuItems.size()) {
					selection++;
				} else {
					selection = 1;
				}
			} else if (input == returnInput && returnOption != -1) {
				return returnOption;
			}
		}
	}