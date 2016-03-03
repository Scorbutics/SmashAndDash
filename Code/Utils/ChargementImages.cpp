#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "ChargementImages.h"
#include "../ska/World/World.h"
#include "../Gameplay/Character.h"
#include "../ska/Utils/NumberUtils.h"

using namespace std;



int GetRandom(vector<int>& probs)
{
	int r = rand()%(100);
	size_t newSize;
	vector<int> newProbs;

	const size_t size = probs.size();
	newSize = size;
	for(unsigned int i = 0; i < size && 1 < newSize; i++)
		if(r > probs[i])
		{
			probs[i] = -1;
			newSize--;
		}

	newProbs.reserve(newSize);
	for(unsigned int i = 0; i < size; i++)
		if(probs[i] != -1)
			newProbs.push_back(i);

	
	return newProbs[rand()%newSize];
}

ska::IniReader* GetRandomMobSettings(ska::World* w)
{
	ska::IniReader* reader = NULL;
	vector<int> probs;
	const size_t totMobs = w->getMobSettings().size();

	probs.resize(totMobs);
	for(size_t i = 0; i < totMobs; i++)
		probs[i] = w->getMobSettings()[i].getInt("Spawn probability_percent");


	if(totMobs != 0)
		reader = &w->getMobSettings()[GetRandom(probs)];

	return reader;
}

ska::Rectangle GetCurrentSpritePosOfHero(Character* hero, bool animation)
{
    int direction = hero->getDirection();
	ska::Rectangle spritePos = hero->getAnimation()->getOffsetBase();


    switch(direction)
    {
        case 0:
        spritePos.y = hero->getAnimation()->getOffsetAndFrameSize().h*2;
        break;
        case 1:
        spritePos.y = hero->getAnimation()->getOffsetAndFrameSize().h;
        break;
        case 2:
        spritePos.y = 0;
        break;
        case 3:
        spritePos.y = hero->getAnimation()->getOffsetAndFrameSize().h*3;
        break;
        case 4:
        spritePos.y = hero->getAnimation()->getOffsetAndFrameSize().h*7;
        break;
        case 5:
        spritePos.y = hero->getAnimation()->getOffsetAndFrameSize().h*5;
        break;
        case 6:
        spritePos.y = hero->getAnimation()->getOffsetAndFrameSize().h*4;
        break;
        case 7:
        spritePos.y = hero->getAnimation()->getOffsetAndFrameSize().h*6;
        break;
        default:
        break;
    }

	hero->getAnimation()->setOffsetAndFrameSize(spritePos);

	if(animation)
		spritePos = hero->getAnimation()->getRectOfCurrentFrame();
	else
		spritePos.x += spritePos.w;

    return spritePos;
}

void VariablesAcquisition(vector<int> &vect, std::string filename)
{
	ska::IniReader reader(filename);
	int var_number = reader.getInt("Game var_number");
	stringstream ss;

	if(var_number > 0)
		for(int i = 1; i <= var_number; i++)
		{
			ss << "%";

			for(int j = 4 - ska::NumberUtils::getMax10Pow(i); j >= 0; j--)
				ss << "0";

			ss << i;

			vect.push_back(reader.getInt("Variable " + ss.str()));
			ss.str("");
		}

}

void SwitchesAcquisition(vector<bool> &vect, std::string filename)
{
	ska::IniReader reader(filename);
	int switch_number = reader.getInt("Game switch_number");
	stringstream ss;

	if(switch_number > 0)
		for(int i = 1; i <= switch_number; i++)
		{
			ss << "%";

			for (int j = 4 - ska::NumberUtils::getMax10Pow(i); j >= 0; j--)
				ss << "0";

			ss << i;

			vect.push_back(reader.getInt("Switch " + ss.str()) != 0);
			ss.str("");
		}

}

void VariablesWriting(vector<int> &vect, std::string filename)
{
	ska::IniReader reader(filename);
	reader.set("Game var_number", vect.size());
	stringstream ss;

	for(int i = 1; i <= vect.size(); i++)
	{
		ss << "%";

		for (int j = 4 - ska::NumberUtils::getMax10Pow(i); j >= 0; j--)
			ss << "0";

		ss << i;

		reader.set("Variable " + ss.str(), vect[i-1]);
		ss.str("");
	}
	reader.save(filename);
}

void SwitchesWriting(vector<bool> &vect, std::string filename)
{
	ska::IniReader reader(filename);
	reader.set("Game switch_number", vect.size());
	stringstream ss;

	for(int i = 1; i <= vect.size(); i++)
	{
		ss << "%";

		for (int j = 4 - ska::NumberUtils::getMax10Pow(i); j >= 0; j--)
			ss << "0";

		ss << i;

		reader.set("Switch " + ss.str(), vect[i-1]);
		ss.str("");
	}
	reader.save(filename);
}
