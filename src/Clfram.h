
#ifndef Clfram_h
#define Clfram_h

//Includes
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <conio.h>
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#include <Windows.h>
#endif

using namespace std;

//Static Global Options
namespace clOption {
	static bool USE_CLS_NEW = true;
}

//Utility Functions
namespace clUtil {
	void error(string message, string tag = "SYS", int code = 0, string location = "", string fileName = "ErrorLog", string flags = "DO_DISPLAY DO_LOG");
	string implode(vector<string> toImplode, bool addNewLine);
	vector<string> explode(string toExplode, char atThis = ',', bool removeSpaces = true);
	string uppercase(string toConvert, int lengthToUpper = 10000);
	string lowercase(string toConvert, int lengthToLower = 10000);
	string boolToWord(bool toConvert, string isTrue = "true", string isFalse = "false");
	void quit();
	bool containsOnlyLetters(string doesThis);
	int wordCount(string howMany);
}

//Display Functions
namespace clCons {
	void paus(string pauseMessage = "LEAVE AS DEFAULT PARAMETER FOR DEFAULT MSG");
	void cls();
	void clsNew();
	int getConsoleWidth();
	int getConsoleHeight();
	int menu(string topMsg, string bottomMsg, vector<string> menuItems, char compliment = '-', char returnInput = 27, int returnOption = -1);
	int numberChooser(string prompt, int bottomMargin, int topMargin);
	void advDisp(string toDisplay, int newLines = 0, int afterSpaces = 0, int inBetweenSpaces = 0);
	void centerHor(string toDisplay, char spaces = ' ', int afterlines = 0);
	void centerVer(int linesToBeCentered);
	void centerVerhor(int linesToBeCentered, string toDisplay, char spaces = ' ', int afterlines = 0, bool cls = 1);
	string centerStringIn(int width, string centerThis, int rightOffSet = 0);
	string advInput(string prompt = "LEAVE DEFAULT FOR NONE");
}

//File IO Functions
namespace clFile {
	string read(string location, int y, int x, bool readLineRatherThanSpace = true);
	string cut(ifstream *read);
	string cut(string cutThis);
	vector<string> readBlock(string location, int y, bool untilChar, char until, int lines, bool entireFile = false);
}

//Mathematical Functions
namespace clMath {
	int abVal(int abThis, bool returnNegative = false);
}

#endif