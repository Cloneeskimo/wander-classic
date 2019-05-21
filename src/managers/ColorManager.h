
#ifndef ColorManager_h
#define ColorManager_h

//Includes
#include <windows.h>
#include <string>
#include <iostream>
#include <Clfram.h>

//ColorFlag Enum
enum class ColorFlag {

	//RGB
	RED,
	GREEN,
	BLUE,

	//YCP
	YELLOW,
	CYAN,
	PURPLE,

	//OTHER
	GRAY
};

//ColorString Struct
struct ColorString {

	//Constructors
	ColorString(std::string content, ColorFlag color, int colorBegin = 0, int colorEnd = 10000, bool intense = false) { //normal type display
		construct(content, color, colorBegin, colorEnd, intense);
	}
	ColorString(std::string content, ColorFlag color, bool wordTypeDisplay = true, int wordBegin = 5000, int wordLength = 5000, bool intense = false) { //word type display
		this->content = content;
		this->color = color;
		this->wordTypeDisplay = wordTypeDisplay;
		this->wordBegin = wordBegin;
		this->wordLength = wordLength;
		this->intense = intense;
	}
	ColorString() {}; //default
	void construct(std::string content, ColorFlag color, int colorBegin = 0, int colorEnd = 10000, bool intense = false) {
		this->content = content;
		this->color = color;
		this->colorBegin = colorBegin;
		this->colorEnd = colorEnd;
		this->intense = intense;
	}

	//Functions
	int length() { return this->content.length(); }
	void configureWordTypeDisplay(int wordBegin = 0, int wordLength = 1, bool enable = true) {
		this->wordTypeDisplay = enable;
		this->wordBegin = wordBegin;
		this->wordLength = wordLength;
	}

	//Data
	std::string content;
	ColorFlag color;
	int colorBegin;
	int colorEnd;
	bool intense;

	//Word Type Display
	bool wordTypeDisplay = false;
	int wordBegin, wordLength;
};

//ColorManager Class
static class ColorManager {
public:

	//Data
	static WORD Attributes;
	static WORD Background;

	//Functions
	static void setForegroundColor(ColorFlag thisColor, bool intense = false);
	static void resetForegroundColor();
	static void centerHorColor(ColorFlag thisColor, bool intense, std::string toDisplay, char spaces = ' ', int afterlines = 0);
	static void centerHorColor(ColorString toDisplay, char spaces = ' ', int afterlines = 0);
	static void displayWithColor(ColorFlag thisColor, std::string toDisplay, bool endLine = false);
	static void csout(ColorString display);
	static int menu(ColorFlag selectColor, string topMsg, ColorFlag topColor, string bottomMsg, ColorFlag bottomColor, vector<string> menuItems, char compliment = '-', char returnInput = 27, int returnOption = -1);
};

#endif