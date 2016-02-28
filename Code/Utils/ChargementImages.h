#ifndef DEF_CHARGEMENTIMAGES
#define DEF_CHARGEMENTIMAGES

#include <iostream>
#include <string>
#include <SDL2/SDL.h>
#include <vector>
#include "../Gameplay/Character.h"
#include "../Graphic/Rectangle.h"

class IniReader;

ska::Rectangle GetCurrentSpritePosOfHero(Character* hero, bool animation);
int GetRandom(std::vector<int>& probs);
IniReader* GetRandomMobSettings(World* w);

void SwitchesAcquisition(vector<bool> &vect, std::string filename);
void VariablesAcquisition(vector<int> &vect, std::string filename);
void VariablesWriting(vector<int> &vect, std::string filename);
void SwitchesWriting(vector<bool> &vect, std::string filename);
#endif
