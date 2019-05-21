#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <conio.h>
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#include <Windows.h>
#endif

using namespace std;

/*Static Global Options*/
namespace clOption
{
	static bool USE_CLS_NEW = true;
}

/*Utility Functions*/
namespace clUtil {
	void error(string message, string tag = "SYS", int code = 0, string location = "", string fileName = "errorLog", string flags = "DO_DISPLAY DO_LOG");
	string implode(vector<string> toImplode, bool addNewLine);
	vector<string> explode(string toExplode, char atThis = ',', bool removeSpaces = true);
	string uppercase(string toConvert, int lengthToUpper = 10000);
	string lowercase(string toConvert, int lengthToLower = 10000);
	string boolToWord(bool toConvert, string isTrue = "true", string isFalse = "false");
	void quit();
	bool containsOnlyLetters(string doesThis);
	int wordCount(string howMany);
}

/*Display Functions*/
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

struct dataSegment
{
	//Constructor
	dataSegment(std::string title, std::string value = "")
	{
		this->title = title;
		this->value = value;
	}
		
	//Operator
	friend bool operator==(const dataSegment& lhs, const std::string& rhs)
	{
		if (clUtil::uppercase(const_cast<dataSegment&>(lhs).title) == const_cast<std::string&>(rhs)) { //Check Name
			return true;
		} else {
			return false;
		}
	}

	//Functions
	void addChildValue(std::string title, std::string value)
	{
		this->childSegments.push_back(dataSegment(title, value));
	}
	void addChild(dataSegment newSegment) { this->childSegments.push_back(newSegment); }
	int cs() { return this->childSegments.size(); }
	int vint() { return stoi(this->value); }
	double vdbl() { return stod(this->value); }
	float vflt() { return stof(this->value); }
	std::string v() { return this->value; }
	std::string cv(int index) { return this->childSegments[index].value; }
	dataSegment* c(int index) { return &(this->childSegments[index]); }
	dataSegment* getChildByTitle(std::string thisTitle)
	{
		for (int i = 0; i < this->childSegments.size(); i++) {
			if (this->childSegments[i].title == thisTitle) {
				return &(this->childSegments[i]);
			}
		}
	}

	//Variables
	std::string title;
	std::string value;
	std::vector<dataSegment> childSegments;
};

/*File IO Functions*/
namespace clFile {
	string read(string location, int y, int x, bool readLineRatherThanSpace = true);
	string cut(ifstream *read);
	string cut(string cutThis);
	vector<string> readBlock(string location, int y, bool untilChar, char until, int lines, bool entireFile = false);
	void layoutDataSegment(ofstream* write, dataSegment segment, int tabs = 0);
	dataSegment readDataSegment(ifstream* read, int tabs = 0, bool* endingBracket = false);
}

/*Mathematical Functions*/
namespace clMath {
	int abVal(int abThis, bool returnNegative = false);
}