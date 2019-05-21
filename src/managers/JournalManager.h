#pragma once
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include "clfram.h"
#include "item.h"
#include "optionsManager.h"
#include "calcManager.h"
#include "player.h"

struct Event
{
	//Cosntructor
	Event() {};
	Event(std::string name, std::vector<std::string> story, std::vector<std::string> itemInfo, int eventCode, bool equipItems = false, bool recurring = false, bool journalEntry = true)
	{
		this->name = name;
		this->story = story;
		this->itemInfo = itemInfo;
		this->equipItems = equipItems;
		this->eventCode = eventCode;
		this->recurring = recurring;
		this->journalEntry = journalEntry;
	}

	//Variables
	std::string name = "unnamed event";
	std::vector<std::string> story;
	std::vector<std::string> itemInfo;
	bool equipItems = false;
	bool recurring = false;
	bool journalEntry = true;
	int eventCode = -1;
};

static class JournalManager
{
public:
	
	//Functions
	static void loadEvents(std::string storyFolder);
	static void displayCinematically(std::vector<std::string> displayThis);
	static void enactEvent(int code, std::vector<Player*> players);
	static void openJournal(std::vector<int> eventCodes);
	static Event* getEvent(int code);

private:

	//Variables
	static std::vector<Event> _events;
};