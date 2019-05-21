#include "OptionsManager.h"

//Static Variables
bool OptionsManager::backgroundWhite = true;
bool OptionsManager::fastText = false;
bool OptionsManager::fastMovement = false;
bool OptionsManager::fastCombat = false;
int OptionsManager::difficulty = 1;
int OptionsManager::loadingProg = 0;
int OptionsManager::loadingMax = 0;

//Functions
void OptionsManager::loadOptions() {

	//variables
	std::string nextLine = "";
	ifstream read;

	//open file
	read.open("data//options.wtxt");

	//parse line
	while (!read.eof()) {
		getline(read, nextLine);

		if (nextLine.find("backgroundWhite:") != std::string::npos) { //BACKGROUND COLOR
			if (clFile::cut(nextLine)[0] == '0') {
				OptionsManager::backgroundWhite = false;
			}
		} else if (nextLine.find("fast text:") != std::string::npos) { //FAST TEST
			if (clFile::cut(nextLine)[0] == '1') {
				OptionsManager::fastText = true;
			}
		} else if (nextLine.find("difficulty:") != std::string::npos) { //DIFFICULTY
			OptionsManager::difficulty = stoi(clFile::cut(nextLine));
		} else if (nextLine.find("fast movement:") != std::string::npos) { //FAST MOVEMENT
			if (clFile::cut(nextLine)[0] == '1') {
				OptionsManager::fastMovement = true;
			}
		} else if (nextLine.find("fast combat:") != std::string::npos) { //FAST COMBAT
			if (clFile::cut(nextLine)[0] == '1') {
				OptionsManager::fastCombat = true;
			}
		}
	}

	//update colors
	updateColors();
}
void OptionsManager::updateColors() {

	//update colors
	if (OptionsManager::backgroundWhite) {
		ColorManager::Background = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED | BACKGROUND_INTENSITY;
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 240);
	} else {
		ColorManager::Background = 0;
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
	}
}
void OptionsManager::updateOptions() {

	//open options file
	ofstream write;
	write.open("data//options.wtxt");

	//write options
	write << "backgroundWhite: " << OptionsManager::backgroundWhite << std::endl;
	write << "fast text: " << OptionsManager::fastText << std::endl;
	write << "fast movement: " << OptionsManager::fastMovement << std::endl;
	write << "fast combat: " << OptionsManager::fastCombat << std::endl;
	write << "difficulty: " << OptionsManager::difficulty << std::endl;

	//close options file
	write.close();
	return;
}
void OptionsManager::wError(std::string message, std::string file, bool fatal) {
	if (fatal) {
		clUtil::error(message, file, 0, "data\\error\\", "ErrorLog", "DO_DISPLAY DO_LOG FATAL_ERROR");
	} else {
		clUtil::error(message, file, 0, "data\\error\\", "ErrorLog");
	}
}
void OptionsManager::displayLoading(std::string message, bool addOne, bool reset) {

	//add one
	if (addOne && loadingProg < loadingMax) {
		loadingProg++;
	}

	//variables
	std::string bar = "<<";
	int barLength = (float)clCons::getConsoleWidth() * ((float)2 / (float)3);
	int barCompletion = (((float)loadingProg / (float)loadingMax) * (float)barLength);
	if (loadingProg == loadingMax) {
		barCompletion = barLength;
	} else if (loadingProg == 0) {
		barCompletion = 0;
	}
	for (int i = 0; i < barCompletion; i++) {
		bar += "|";
	}
	for (int i = 0; i < barLength - barCompletion; i++) {
		bar += "-";
	}
	bar += ">>";
	ColorString loadingBar;
	if (barCompletion == 0) {
		loadingBar = ColorString(bar, ColorFlag::YELLOW, true);
	} else {
		loadingBar = ColorString(bar, ColorFlag::YELLOW, 2, barCompletion + 2, true);
	}

	//display
	clCons::cls();
	clCons::clsNew();
	clCons::centerVerhor(3, message, ' ', 1, false);
	ColorManager::centerHorColor(loadingBar);

	//reset
	if (reset) loadingMax = loadingProg = 0;
}
std::string OptionsManager::getColorOption() {
	if (OptionsManager::backgroundWhite) {
		return "white";
	} else {
		return "black";
	}
}
std::string OptionsManager::getDifficultyOption() {
	if (OptionsManager::difficulty == 0) {
		return "easy";
	} else if (OptionsManager::difficulty == 1) {
		return "medium";
	} else {
		return "hard";
	}
}