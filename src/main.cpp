// WANDER
// Jacob Oaks

#include "gameEngine.h"
#include "clfram.h"
#include <fstream>

int main()
{

	ifstream read;
	read.open("data\\saves\\CloneeskimoTwo.wsave");
	dataSegment loadedSaveData = clFile::readDataSegment(&read);

	clCons::paus("");

	GameEngine Wander;
	Wander.init();
}