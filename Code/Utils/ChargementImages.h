#ifndef DEF_CHARGEMENTIMAGES
#define DEF_CHARGEMENTIMAGES

#include <iostream>
#include <string>
#include <vector>
#include "../Gameplay/Character.h"
#include "../ska/Graphic/Rectangle.h"

class IniReader;
class MobSpawner;

ska::Rectangle GetCurrentSpritePosOfHero(Character* hero, bool animation);
int GetRandom(std::vector<int>& probs);
ska::IniReader* GetRandomMobSettings(MobSpawner& w);

void SwitchesAcquisition(vector<bool> &vect, std::string filename);
void VariablesAcquisition(vector<int> &vect, std::string filename);
void VariablesWriting(vector<int> &vect, std::string filename);
void SwitchesWriting(vector<bool> &vect, std::string filename);
#endif
