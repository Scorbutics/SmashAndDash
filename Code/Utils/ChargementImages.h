#ifndef DEF_CHARGEMENTIMAGES
#define DEF_CHARGEMENTIMAGES

#include <iostream>
#include <string>
#include <SDL2/SDL.h>
#include <vector>
#include "../Gameplay/Character.h"
#include "../Graphic/Rectangle.h"

class IniReader;

unsigned int DistanceSquared(const ska::Rectangle* pos1, const ska::Rectangle* pos2);
bool IsPositionInBox(const ska::Rectangle *pos, const ska::Rectangle *box);
ska::Rectangle PosToCenterPicture(const ska::Rectangle *imageToCenter, const ska::Rectangle *imageBackground);
bool CollisionBoxABoxB(ska::Rectangle rectA, ska::Rectangle rectB);
int GetDirectionFromPos(ska::Rectangle *posHero, ska::Rectangle *mousePos);
ska::Rectangle GetCurrentSpritePosOfHero(Character* hero, bool animation);
int GetRandom(std::vector<int>& probs);
IniReader* GetRandomMobSettings(World* w);
unsigned int GetMax10Pow(int num);
void SwitchesAcquisition(vector<bool> &vect, std::string filename);
void VariablesAcquisition(vector<int> &vect, std::string filename);
void VariablesWriting(vector<int> &vect, std::string filename);
void SwitchesWriting(vector<bool> &vect, std::string filename);
#endif
