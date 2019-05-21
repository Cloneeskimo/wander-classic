#include "Clfram.h"

//Utility Functions
namespace clUtil {
	void error(string message, string tag, int code, string location, string fileName, string flags) {

		//handles an error based on received parameters
		#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
		SYSTEMTIME time;
		GetLocalTime(&time);
		#endif
		if (flags.find("DO_LOG") != std::string::npos) { //Will Log Error
			ifstream test;
			ofstream write;
			#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
			write.open(location + fileName + " " + to_string(time.wMonth) + "-" + to_string(time.wDay) + "-" + to_string(time.wYear) + ".txt", ios::app);
			#else
			int logNum = 0;
			while (!test.fail()) {
				test.close();
				logNum++;
				test.open("errorLog" + to_string(logNum + 1) + ".txt");
			}
			if (logNum == 0) {
				write.open("errorLog.txt");
			} else {
				write.open("errorLog" + to_string(logNum + 1) + ".txt");
			}
			#endif

			#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
			std::string precursor = "[";
			if (time.wMonth < 10) {
				precursor += "0";
			}
			precursor += to_string(time.wMonth) + "/";
			if (time.wDay < 10) {
				precursor += "0";
			}
			precursor += to_string(time.wDay) + "/" + to_string(time.wYear) + " ";
			if (time.wHour < 10) {
				precursor += "0";
			}
			precursor += to_string(time.wHour) + ":";
			if (time.wMinute < 10) {
				precursor += "0";
			}
			precursor += to_string(time.wMinute) + ":";
			if (time.wSecond < 10) {
				precursor += "0";
			}
			precursor += to_string(time.wSecond) + "]";

			write << precursor;
			#endif
			write << "[" << tag << "]: " << message << " (Code: " << code << ")" << endl;
			write.close();
		}
		if (flags.find("DO_DISPLAY") != std::string::npos) {
			cout << endl;
			clCons::centerHor(message);
			clCons::paus("");
		}
		if (flags.find("FATAL_ERROR") != std::string::npos) {
			quit();
		}
		return;
	}
	string implode(vector<string> toImplode, bool addNewLine) {
		string imploded = "";
		for (int i = 0; i < toImplode.size(); i++) {
			imploded += toImplode[i];
			if (addNewLine) {
				imploded += "\n";
			}
		}
		return imploded;
	}
	vector<string> explode(string toExplode, char atThis, bool removeSpaces) {
		vector<string> toReturn;
		string next;
		bool justAdded = false;
		for (int i = 0; i < toExplode.size(); i++) {
			if (justAdded && toExplode[i] == ' ' && removeSpaces) {
				justAdded = false;
			} else if (toExplode[i] == atThis) {
				toReturn.push_back(next);
				next = "";
				justAdded = true;
			} else {
				next.push_back(toExplode[i]);
			}
		}
		toReturn.push_back(next);
		return toReturn;
	}
	string uppercase(string toConvert, int lengthToUpper) {
		string newString = "";
		for (int i = 0; i < toConvert.length(); i++) {
			if (i < lengthToUpper) {
				newString.push_back(toupper(toConvert[i]));
			} else {
				newString.push_back(toConvert[i]);
			}
		}
		return newString;
	}
	string lowercase(string toConvert, int lengthToLower) {
		string newString = "";
		for (int i = 0; i < toConvert.length(); i++) {
			if (i < lengthToLower) {
				newString.push_back(tolower(toConvert[i]));
			} else {
				newString.push_back(toConvert[i]);
			}
		}
		return newString;
	}
	string boolToWord(bool toConvert, string isTrue, string isFalse) {
		if (toConvert) {
			return isTrue;
		} else {
			return isFalse;
		}
	}
	void quit() {
		exit(1);
	}
	bool containsOnlyLetters(string doesThis) {

		//test each letter
		for (int i = 0; i < doesThis.length(); i++) {
			if (!((doesThis[i] >= 65 && doesThis[i] <= 90) || (doesThis[i] >= 97 && doesThis[i] <= 122))) {
				return false;
			}
		}
		return true;
	}
	int wordCount(string howMany) {
		int wordCount = 0;
		bool hasBegun = false;
		bool justSpaced = false;
		for (int i = 0; i < howMany.length(); i++) {
			if (howMany[i] == ' ' && hasBegun && !justSpaced) {
				wordCount++;
				justSpaced = true;
			}
			if (howMany[i] != ' ') {
				hasBegun = true;
				justSpaced = false;
			}
		}
		if (howMany[howMany.length() - 1] != ' ') {
			wordCount++;
		}
		return wordCount;
	}
}

//Display Functions
namespace clCons {
	void paus(string pauseMessage) {
		if (pauseMessage == "LEAVE AS DEFAULT PARAMETER FOR DEFAULT MSG") {
			clCons::advDisp("Press anything to continue");
		} else {
			clCons::advDisp(pauseMessage);
		}
		char temp = _getch();
		return;
	}
	void cls() {
		#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
		system("cls");
		return;
		#endif
		for (int i = 0; i < 1000; i++) {
			cout << " ";
		}
	}
	void clsNew() {
		HANDLE hOut;
		COORD Position;

		hOut = GetStdHandle(STD_OUTPUT_HANDLE);

		Position.X = 0;
		Position.Y = 0;
		SetConsoleCursorPosition(hOut, Position);
	}
	int getConsoleWidth() {
		#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		if (!GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
			clUtil::error("Could not determine width of console screen", "GDC++LIB", 0, "", "GDC++LIB ERRORLOG");
			return 0;
		} else {
			return csbi.srWindow.Right - csbi.srWindow.Left + 1;
		}
		#endif
		return 0;
	}
	int getConsoleHeight() {
		#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		if (!GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
			clUtil::error("Could not determine height of console screen", "CLFRAM_H", 0, "", "CLFRAM_ERROR", "DO_DISPLAY DO_LOG FATAL_ERROR");
			return 0;
		} else {
			return csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
		}
		#endif
		return 0;
	}
	int menu(string topMsg, string bottomMsg, vector<string> menuItems, char compliment, char returnInput, int returnOption) {
		
		//creates a menu UI with the given parameters and returns the option selected by the user
		int selection = 1;
		char input;
		while (1) {
			if (clOption::USE_CLS_NEW) {
				clsNew();
			} else {
				cls();
			}
			clCons::centerVer(menuItems.size() + 4);
			clCons::centerHor(topMsg, compliment, 1);
			cout << endl;
			for (int i = 0; i < menuItems.size(); i++) {
				if (selection == i + 1) {
					clCons::centerHor(">- " + menuItems[i] + " -<");
				} else {
					clCons::centerHor(menuItems[i]);
				}
			}
			cout << endl << endl;
			clCons::centerHor(bottomMsg, compliment, 0);

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
	int numberChooser(string prompt, int bottomMargin, int topMargin) {

		//variables
		int input = 1;
		int selection = bottomMargin;
		int screenWidth = clCons::getConsoleWidth();
		int displayWidth = (clCons::getConsoleWidth() * 2) / 3;
		std::string markerString = "";

		//marker calculation
		int effTopMarg = topMargin - bottomMargin;
		int effBotMarg = 0;
		int effSelec = selection - bottomMargin;
		int markerLocation = (effSelec / effTopMarg) * displayWidth;

		clCons::cls();
		while (!(input == 13 || input == 32)) {

			//update measurements
			int oldWidth = screenWidth;
			screenWidth = clCons::getConsoleWidth();
			if (oldWidth != screenWidth) {
				clCons::cls();
			} else {
				clCons::clsNew();
			}
			displayWidth = (clCons::getConsoleWidth() * 2) / 3;

			//marker calculation
			effTopMarg = topMargin - bottomMargin;
			effBotMarg = 0;
			effSelec = selection - bottomMargin;
			markerLocation = (int)(((double)effSelec / (double)effTopMarg) * (double)displayWidth);

			//hard-set marker locations
			if (selection == bottomMargin) {
				markerLocation = 0;
			} else if (selection == topMargin) {
				markerLocation = displayWidth - 1;
			}

			//marker pre-display
			markerString = "<";
			for (int i = 0; i < displayWidth; i++) {
				if (i == markerLocation) {
					markerString += "|";
				} else {
					markerString += "-";
				}
			}
			markerString += ">";

			//display
			clCons::centerVer(4);
			clCons::centerHor(prompt + " (" + to_string(selection) + "/" + to_string(topMargin) + ")", ' ', 2);
			clCons::centerHor(markerString);

			//grab input
			input = _getch();

			//act on input
			switch (input) {
				case 'D':
				case 'd':
					selection++;
					if (selection > topMargin)
						selection = topMargin;
					break;
				case 'A':
				case 'a':
					selection--;
					if (selection < bottomMargin)
						selection = bottomMargin;
					break;
			}
		}

		return selection;
	}
	void advDisp(string toDisplay, int newLines, int afterSpaces, int inBetweenSpaces) {
		for (int i = 0; i < toDisplay.size(); i++) {
			cout << toDisplay[i];
			if (i != toDisplay.size() - 1) {
				for (int j = 0; j < inBetweenSpaces; j++) {
					cout << " ";
				}
			}
		}
		for (int k = 0; k < afterSpaces; k++) {
			cout << " ";
		}
		for (int l = 0; l < newLines; l++) {
			cout << endl;
		}
		return;
	}
	void centerHor(string toDisplay, char spaces, int afterlines) {
		int width = clCons::getConsoleWidth();
		if (toDisplay.size() > width) {
			cout << toDisplay;
			return;
		}
		int beforeSpaces = (int)((width - (double)toDisplay.size()) / 2);
		for (int i = 0; i < beforeSpaces; i++) {
			cout << spaces;
		}
		advDisp(toDisplay);
		for (int j = 0; j < width - beforeSpaces - toDisplay.size(); j++) {
			cout << spaces;
		}
		for (int k = 0; k < afterlines; k++) {
			cout << endl;
		}
	}
	void centerVer(int linesToBeCentered) {
		int height = clCons::getConsoleHeight();
		int width = clCons::getConsoleWidth();
		if (linesToBeCentered > height) {
			return;
		}
		int linesToDisplay = (int)((height - linesToBeCentered) / 2);
		for (int i = 0; i < linesToDisplay; i++) {
			cout << endl;
		}
	}
	void centerVerhor(int linesToBeCentered, string toDisplay, char spaces, int afterlines, bool cls) {
		if (cls) {
			if (clOption::USE_CLS_NEW) {
				clsNew();
			} else {
				clCons::cls();
			}
		}
		centerVer(linesToBeCentered);
		centerHor(toDisplay, spaces, afterlines);
		return;
	}
	string centerStringIn(int width, string centerThis, int rightOffSet) {
		std::string toReturn = "";
		int beforeSpaces = ((width - rightOffSet) / 2) - (centerThis.size() / 2);
		for (int i = 0; i < beforeSpaces; i++) {
			toReturn += " ";
		}
		toReturn += centerThis;
		return toReturn;
	}
	string advInput(string prompt) {

		//variables
		std::string input;
		char nextChar = ' ';

		//input loop
		while (nextChar != 13) {
			if (input == " ") {
				input = "";
			}
			if (nextChar == 8) {
				if (input.length() > 0) {
					input = input.substr(0, input.length() - 1);
				}
			} else if (nextChar != '\033') {
				input += nextChar;
			}
			if (prompt != "LEAVE DEFAULT FOR NONE") {
				centerVerhor(2, prompt, ' ', 0);
				centerHor(input);
			} else {
				centerVerhor(1, input, ' ', 0);
			}
			nextChar = _getch();
		}
		return input;
	}
}

//File IO Functions
namespace clFile {
	string read(string location, int y, int x, bool readLineRatherThanSpace) {
		ifstream read;
		string result;
		read.open(location);
		if (read.fail()) {
			clUtil::error("FATAL ERROR: Was unable to open " + location, "CF++LIB", 0, "", "CF++LIB ERRORLOG", "DO_DISPLAY DO_LOG FATAL_ERROR");
			exit(1);
		}
		for (int i = 0; i < y; i++) {
			read.ignore(256, '\n');
		}
		read.ignore(x);
		if (readLineRatherThanSpace) {
			getline(read, result);
		} else if (!readLineRatherThanSpace) {
			read >> result;
		}
		read.close();
		return result;
	}
	string cut(ifstream* read) {

		//variables
		std::string toReturn;
		bool inLoop = true;

		//get information
		while (toReturn == "" || toReturn == "\n" || toReturn == " ") {
			getline(*read, toReturn);
		}

		//cut to colon
		for (int i = 0; inLoop; i++) {
			if (toReturn[i] == ':') {
				toReturn = toReturn.substr(i + 1, toReturn.size() - (i + 1));
				inLoop = false;
			}
		}

		//cut to value
		inLoop = true;
		for (int j = 0; inLoop; j++) {
			if (toReturn[j] != ' ') {
				toReturn = toReturn.substr(j, toReturn.size() - j);
				inLoop = false;
			}
		}

		//return value
		return toReturn;
	}
	string cut(string cutThis) {

		//variables
		std::string toReturn = cutThis;
		bool inLoop = true;

		//cut to colon
		for (int i = 0; inLoop; i++) {
			if (toReturn[i] == ':') {
				toReturn = toReturn.substr(i + 1, toReturn.size() - (i + 1));
				inLoop = false;
			}
		}

		//cut to value
		inLoop = true;
		for (int j = 0; inLoop; j++) {
			if (toReturn[j] != ' ') {
				toReturn = toReturn.substr(j, toReturn.size() - j);
				inLoop = false;
			}
		}

		//return value
		return toReturn;
	}
	vector<string> readBlock(string location, int y, bool untilChar, char until, int lines, bool entireFile) {
		ifstream read;
		vector<string> result;
		string nextLine;
		bool stillAdding = true;
		read.open(location);
		if (read.fail()) {
			clUtil::error("FATAL ERROR: Was unable to open " + location, "CF++LIB", 0, "", "CF++LIB ERRORLOG", "DO_DISPLAY DO_LOG FATAL_ERROR");
			exit(1);
		}
		if (entireFile) {
			while (!read.eof()) {
				getline(read, nextLine);
				result.push_back(nextLine);
			}
			read.close();
			return result;
		}
		for (int i = 0; i < y; i++) {
			read.ignore(256, '\n');
		}
		if (untilChar) {
			while (stillAdding) {
				getline(read, nextLine);
				for (int i = 0; i < nextLine.size(); i++) {
					if (nextLine[i] == until) {
						stillAdding = false;
					}
				}
				if (stillAdding) {
					result.push_back(nextLine);
				}
			}
		} else {
			for (int j = 0; j < lines; j++) {
				getline(read, nextLine);
				result.push_back(nextLine);
			}
		}
		return result;
	}
}

//Mathematical Functions
namespace clMath {
	int abVal(int abThis, bool returnNegative) {
		int abVal = 0;
		while (abThis != 0) {
			if (abThis < 0) {
				abThis++;
			} else {
				abThis--;
			}
			if (returnNegative) {
				abVal--;
			} else {
				abVal++;
			}
		}
		return abVal;
	}
}