#pragma once
#include <string>
#include "clfram.h"
#include "colorManager.h"

//Structs
struct Coord
{
	int x, y;
};

static class OptionsManager
{
public:

	//Functions
	static void loadOptions();
	static void updateColors();
	static void updateOptions();
	static void wError(std::string message, std::string file, bool fatal = true);
	static void displayLoading(std::string message, bool addOne = true, bool reset = false);
	static std::string getColorOption();
	static std::string getDifficultyOption();

	//Variables
	static bool backgroundWhite;
	static bool fastText;
	static bool fastMovement;
	static bool fastCombat;
	static int difficulty;

	//Loading
	static int loadingProg;
	static int loadingMax;
};