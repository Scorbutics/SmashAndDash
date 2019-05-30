#include <sstream>
#include <string>

#include "ChargementImages.h"
#include "Utils/NumberUtils.h"
#include "../Gameplay/Mobs/MobSpawner.h"
#include "Inputs/Readers/IniReader.h"

int GetRandom(std::vector<int>& probs)
{
	const auto r = rand()%(100);
	std::vector<int> newProbs;

	const auto size = probs.size();
	auto newSize = size;
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

ska::IniReader* GetRandomMobSettings(MobSpawner& w) {
	ska::IniReader* reader = nullptr;
	std::vector<int> probs;
	const auto totMobs = w.getMobSettings().size();

	probs.resize(totMobs);
	for(size_t i = 0; i < totMobs; i++)
		probs[i] = w.getMobSettings()[i].get<int>("Spawn probability_percent");


	if(totMobs != 0)
		reader = &w.getMobSettings()[GetRandom(probs)];

	return reader;
}

void VariablesAcquisition(std::unordered_map<std::string, int> &vect, const std::string& filename) {
	ska::IniReader reader(filename);
	const auto varMaxNumber = reader.get<int>("Game var_number");
	std::stringstream ss;

	if (varMaxNumber > 0) {
		for (auto i = 1; i <= varMaxNumber; i++) {
			ss << "%";

			//TODO padding de str avec des '0'
			for (int j = 4 - ska::NumberUtils::getMax10Pow(i); j >= 0; j--)
				ss << "0";

			ss << i;

			const auto& variableNumberStr = ss.str();
			vect[variableNumberStr] = (reader.get<int>("Variable " + variableNumberStr));
			ss.str("");
		}
	}

}

void SwitchesAcquisition(std::unordered_map<std::string, bool> &vect, const std::string& filename)
{
	ska::IniReader reader(filename);
	const auto switchMaxNumber = reader.get<int>("Game switch_number");
	std::stringstream ss;

	if (switchMaxNumber > 0) {
		for (auto i = 1; i <= switchMaxNumber; i++) {
			ss << "%";

			//TODO padding de str avec des '0'
			for (int j = 4 - ska::NumberUtils::getMax10Pow(i); j >= 0; j--)
				ss << "0";

			ss << i;

			const auto& switchNumberStr = ss.str();
			vect[switchNumberStr] = (reader.get<int>("Switch " + switchNumberStr) != 0);
			ss.str("");
		}
	}

}

void VariablesWriting(const std::unordered_map<std::string, int> &vect, const std::string& filename) {
	ska::IniReader reader(filename);
	reader.set("Game var_number", vect.size());
	std::stringstream ss;

	for(unsigned int i = 1; i <= vect.size(); i++) {
		ss << "%";

		//TODO padding de str avec des '0'
		for (int j = 4 - ska::NumberUtils::getMax10Pow(i); j >= 0; j--)
			ss << "0";

		ss << i;

		const auto& variableNumberStr = ss.str();
		reader.set("Variable " + ss.str(), vect.at(variableNumberStr));
		ss.str("");
	}
	reader.save(filename);
}

void SwitchesWriting(const std::unordered_map<std::string, bool> &vect, const std::string& filename)
{
	ska::IniReader reader(filename);
	reader.set("Game switch_number", vect.size());
	std::stringstream ss;

	for(unsigned int i = 1; i <= vect.size(); i++)
	{
		ss << "%";

		//TODO padding de str avec des '0'
		for (int j = 4 - ska::NumberUtils::getMax10Pow(i); j >= 0; j--)
			ss << "0";

		ss << i;

		const auto& switchNumberStr = ss.str();
		reader.set("Switch " + ss.str(), vect.at(switchNumberStr));
		ss.str("");
	}
	reader.save(filename);
}
