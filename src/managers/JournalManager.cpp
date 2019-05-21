#include "journalManager.h"

//Variables
std::vector<Event> JournalManager::_events = std::vector<Event>();

//Functions
void JournalManager::loadEvents(std::string storyFolder)
{
	//Variables
	ifstream read;
	JournalManager::_events = std::vector<Event>();
	std::string nextLine;
	Event nextEvent = Event();

	//Open File
	read.open("data//stories//" + storyFolder + "//events.wtxt");
	if (read.fail()) {
		OptionsManager::wError("Unable to open 'events.wtxt'", "JOURNALMANAGER_H", true);
	}

	//Parse Line
	while (!read.eof()) {
		getline(read, nextLine);

		if (nextLine == "{") { //NEW EVENT
			nextEvent = Event();
		} else if (nextLine == "}") { //FINISHED WITH EVENT
			JournalManager::_events.push_back(nextEvent);
		} else if (nextLine.find("name") != std::string::npos) { //NAME
			nextEvent.name = clFile::cut(nextLine);
		} else if (nextLine.find("event code:") != std::string::npos) { //EVENT CODE
			nextEvent.eventCode = stoi(clFile::cut(nextLine));
		} else if (nextLine.find("story:") != std::string::npos) { //STORY
			while (nextLine != "\t}") {
				getline(read, nextLine);
				
				if (nextLine != "\t}" && nextLine != "\t{" & nextLine != "") {
					nextEvent.story.push_back(nextLine);
				}
			}
			for (int i = 0; i < nextEvent.story.size(); i++) {
				for (int j = 0; j < nextEvent.story[i].length(); j++) {
					if (nextEvent.story[i][j] == '\t') {
						nextEvent.story[i].erase(nextEvent.story[i].begin() + j);
						j--;
					}
				}
			}
		} else if (nextLine.find("items:") != std::string::npos) { //ITEMS
			while (nextLine != "\t}") {
				getline(read, nextLine);
				
				if (nextLine != "\t}" && nextLine != "\t{" & nextLine != "") {
					nextEvent.itemInfo.push_back(nextLine);
				}
			}			
		} else if (nextLine.find("recurring:") != std::string::npos) { //RECURRING
			if (clUtil::uppercase(clFile::cut(nextLine)) == "TRUE")
				nextEvent.recurring = true;
			else
				nextEvent.recurring = false;
		} else if (nextLine.find("equip:") != std::string::npos) { //EQUIP ITEMS
			if (clUtil::uppercase(clFile::cut(nextLine)) == "TRUE")
				nextEvent.equipItems = true;
			else
				nextEvent.equipItems = false;
		} else if (nextLine.find("journal entry:") != std::string::npos) { //JOURNAL ENTRY
			if (clUtil::uppercase(clFile::cut(nextLine)) == "TRUE")
				nextEvent.journalEntry = true;
			else
				nextEvent.journalEntry = false;
		}
	}
}
void JournalManager::displayCinematically(std::vector<std::string> displayThis)
{
	clCons::cls();
	if (clCons::getConsoleHeight() > displayThis.size()) {
		clCons::centerVer(displayThis.size());
	}
	for (int i = 0; i < displayThis.size(); i++) {
		if (!OptionsManager::fastText) {
			int beforeSpaces = (clCons::getConsoleWidth() / 2) - (displayThis[i].length() / 2);
			for (int k = 0; k < beforeSpaces; k++) {
				std::cout << " ";
			}
			for (int j = 0; j < displayThis[i].length(); j++) {
				std::cout << displayThis[i][j];
				if (displayThis[i][j] == '.') {
					std::this_thread::sleep_for(std::chrono::milliseconds(1000));
				} else {
					std::this_thread::sleep_for(std::chrono::milliseconds(50));
				}
			}
			std::cout << std::endl;
		} else {
			clCons::centerHor(displayThis[i]);
		}
	}
	clCons::paus("");
}
void JournalManager::enactEvent(int code, std::vector<Player*> players)
{
	//Grab Event
	Event* event = getEvent(code);

	//Display Story
	JournalManager::displayCinematically(event->story);

	//Create Items
	int averageLevel = 0;
	for (int i = 0; i < players.size(); i++) {
		averageLevel += players[i]->getLevel();
	}
	averageLevel /= players.size();
	std::vector<Item> items = CalcManager::createItemsFromStrings(event->itemInfo, averageLevel);

	//Add Items
	for (int i = 0; i < players.size(); i++) {
		for (int j = 0; j < items.size(); j++) {
			if (items[j].getLevel() <= players[i]->getLevel() && (items[j].getType() == WEAPON || items[j].getType() == ARMOR)) {
				players[i]->equip(items[j]);
			} else {
				players[i]->addItemToInventory(items[j]);
			}
		}
	}
}
void JournalManager::openJournal(std::vector<int> eventCodes)
{
	//Variables
	const int TOTAL_WIDTH_EXTRAS = 3;
	int displayWidth = clCons::getConsoleWidth();
	int selection = 0;
	int leftTopMargin = 0;
	int rightTopMargin = 0;
	int displayHeight = 12;
	char input = 0;
	std::string scrollableString = "[Arrow Keys - Scroll Entry]";
	clCons::cls();

	while (input != 'j' && input != 'J' && input != '\033') {

		//Variables
		bool scrollable = false;
		int leftDisplayWidth = 0;
		int rightDisplayWidth = 0;
		std::vector<ColorString> leftDisplay;
		std::vector<ColorString> rightDisplay;

		//Check Console Height
		while (clCons::getConsoleHeight() < displayHeight + 4) {
			clCons::cls();
			clCons::centerVerhor(1, "Please expand your console.");
			clCons::paus("");
		}

		//Set Left Display
		for (int i = leftTopMargin; i < eventCodes.size(); i++) {
			std::string prefix = "   ", suffix = "   ";
			if (i == selection) {
				prefix = " -[";
				suffix = "]- ";
			}
			leftDisplay.push_back(ColorString(prefix + JournalManager::getEvent(eventCodes[i])->name + suffix, ColorFlag::GREEN, true, 0));
			if (leftDisplay[leftDisplay.size() - 1].length() > leftDisplayWidth) {
				leftDisplayWidth = leftDisplay[leftDisplay.size() - 1].length();
			}
		}

		//Set Right Display
		rightDisplayWidth = clCons::getConsoleWidth() - TOTAL_WIDTH_EXTRAS - leftDisplayWidth;
		Event eventInfo = *JournalManager::getEvent(eventCodes[selection]);
		if (eventInfo.story.size() > displayHeight) {
			scrollable = true;
			for (int i = rightTopMargin; i < eventInfo.story.size(); i++) {
				rightDisplay.push_back(ColorString(clCons::centerStringIn(rightDisplayWidth, eventInfo.story[i]), ColorFlag::GRAY, true, 0));				
			}
		} else {
			int lessThan = (displayHeight - eventInfo.story.size()) / 2;
			for (int i = 0; i < lessThan; i++) {
				rightDisplay.push_back(ColorString("", ColorFlag::GRAY, false));
			}
			for (int i = 0; i < eventInfo.story.size(); i++) {
				rightDisplay.push_back(ColorString(clCons::centerStringIn(rightDisplayWidth, eventInfo.story[i]), ColorFlag::GRAY, true, 0));
			}
		}

		//Display
		clCons::clsNew();
		clCons::centerVer(displayHeight + 4);
		ColorManager::centerHorColor(ColorFlag::GRAY, false, "-=[Journal]=-", ' ', 2);
		for (int i = -1; i < displayHeight + 1; i++) {
			if (i == -1 || i == displayHeight) { //DISPLAY TOP / BOTTOM
				for (int j = 0; j < clCons::getConsoleWidth(); j++) {
					if (j == 0 || j == clCons::getConsoleWidth() - 1 || j == leftDisplayWidth + 1) {
						std::cout << "+";
					}
					else if (i == displayHeight && j == (leftDisplayWidth + 2) + (rightDisplayWidth / 2 - scrollableString.length() / 2) && scrollable) {
						std::cout << scrollableString;
						j += scrollableString.length();
					} else {
						std::cout << "-";
					}
				}
			}
			else { //DIPLAY MIDDLE
				//LEFT
				std::cout << "|";
				if (leftDisplay.size() > i) {
					ColorManager::csout(leftDisplay[i]);
					for (int j = 0; j < leftDisplayWidth - leftDisplay[i].length(); j++) {
						std::cout << " ";
					}
				} else {
					for (int j = 0; j < leftDisplayWidth; j++) {
						std::cout << " ";
					}
				}

				//RIGHT
				std::cout << "|";
				if (rightDisplay.size() > i) {
					ColorManager::csout(rightDisplay[i]);
					for (int j = 0; j < rightDisplayWidth - rightDisplay[i].length(); j++) {
						std::cout << " ";
					}
				} else {
					for (int j = 0; j < rightDisplayWidth; j++) {
						std::cout << " ";
					}
				}
				std::cout << "|";
			}
		}

		//Gather Input
		input = _getch();

		//Parse Input
		switch (input) {
		case 's': //SCROLL LEFT DOWN
		case 'S':
			selection++;
			if (selection >= eventCodes.size())
				selection = eventCodes.size() - 1;
			if ((leftTopMargin + displayHeight <= leftDisplay.size()) && (selection >= leftTopMargin + displayHeight)) {
				leftTopMargin++;
			}
			rightTopMargin = 0;
			break;
		case 'w': //SCROLL LEFT UP
		case 'W':
			selection--;
			if (selection < 0)
				selection = 0;
			if (selection < leftTopMargin)
				leftTopMargin--;
			rightTopMargin = 0;
			break;
		case 80: //SCROLL RIGHT DOWN
			if ((rightTopMargin + displayHeight <= eventInfo.story.size()))
				rightTopMargin++;
			break;
		case 72: //SCROLL RIGHT UP
			if (rightTopMargin > 0)
				rightTopMargin--;
			break;
		};		
	}
}
Event * JournalManager::getEvent(int code)
{
	for (int i = 0; i < JournalManager::_events.size(); i++) {
		if (JournalManager::_events[i].eventCode == code) {
			return &JournalManager::_events[i];
		}
	}
}