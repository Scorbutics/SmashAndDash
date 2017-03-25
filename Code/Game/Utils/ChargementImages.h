#pragma once

#include <iostream>
#include <string>
#include <vector>

namespace ska {
	class IniReader;
}

class MobSpawner;

int GetRandom(std::vector<int>& probs);
ska::IniReader* GetRandomMobSettings(MobSpawner& w);

void SwitchesAcquisition(std::vector<bool> &vect, const std::string& filename);
void VariablesAcquisition(std::vector<int> &vect, const std::string& filename);
void VariablesWriting(const std::vector<int> &vect, const std::string& filename);
void SwitchesWriting(const std::vector<bool> &vect, const std::string& filename);
