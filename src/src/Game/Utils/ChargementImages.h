#pragma once

#include <string>
#include <vector>
#include <unordered_map>

namespace ska {
	class IniReader;
}

class MobSpawner;

int GetRandom(std::vector<int>& probs);
ska::IniReader* GetRandomMobSettings(MobSpawner& w);

void SwitchesAcquisition(std::unordered_map<std::string, bool> &vect, const std::string& filename);
void VariablesAcquisition(std::unordered_map<std::string, int> &vect, const std::string& filename);
void VariablesWriting(const std::unordered_map<std::string, int> &vect, const std::string& filename);
void SwitchesWriting(const std::unordered_map<std::string, bool> &vect, const std::string& filename);
