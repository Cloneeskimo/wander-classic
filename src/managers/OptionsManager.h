
#ifndef OptionsManager_h
#define OptionsManager_h

//Includes
#include <string>
#include <Clfram.h>
#include <managers/ColorManager.h>

//Coordinate Struct
struct Coordinate {
	int x, y;
};

//OptionsManager Class
static class OptionsManager {
public:

	//Functions
	static void loadOptions();
	static void updateColors();
	static void updateOptions();
	static void wError(std::string message, std::string file, bool fatal = true);
	static void displayLoading(std::string message, bool addOne = true, bool reset = false);
	static std::string getColorOption();
	static std::string getDifficultyOption();

	//Data
	static bool backgroundWhite;
	static bool fastText;
	static bool fastMovement;
	static bool fastCombat;
	static int difficulty;

	//Loading
	static int loadingProg;
	static int loadingMax;
};

#endif