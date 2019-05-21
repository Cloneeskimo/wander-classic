
#ifndef JournalManager_h
#define JournalManager_h

//Includes
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <Clfram.h>
#include <Item.h>
#include <managers/OptionsManager.h>
#include <managers/CalcManager.h>
#include <entity/Player.h>

//Event Struct
struct Event {

	//Cosntructors
	Event() {}; //default
	Event(std::string name, std::vector<std::string> story, std::vector<std::string> itemInfo, int eventCode, bool equipItems = false, bool recurring = false, bool journalEntry = true) { //full
		this->name = name;
		this->story = story;
		this->itemInfo = itemInfo;
		this->equipItems = equipItems;
		this->eventCode = eventCode;
		this->recurring = recurring;
		this->journalEntry = journalEntry;
	}

	//Data
	std::string name = "unnamed event";
	std::vector<std::string> story;
	std::vector<std::string> itemInfo;
	bool equipItems = false;
	bool recurring = false;
	bool journalEntry = true;
	int eventCode = -1;
};

//JournalManager Class
static class JournalManager {
public:

	//Functions
	static void loadEvents(std::string storyFolder);
	static void displayCinematically(std::vector<std::string> displayThis);
	static void enactEvent(int code, std::vector<Player*> player);
	static void openJournal(std::vector<int> eventCodes);
	static Event* getEvent(int code);

private:

	//Data
	static std::vector<Event> _events;
};

#endif