#ifndef DEF_CHARGEMENTIMAGES
#define DEF_CHARGEMENTIMAGES

#include <iostream>
#include <string>
#include <SDL2/SDL.h>
#include <vector>

#include "Character.h"
#include "IDs.h"

class IniReader;

unsigned int DistanceSquared(const SDL_Rect* pos1, const SDL_Rect* pos2);
bool IsPositionInBox(const SDL_Rect *pos, const SDL_Rect *box);
std::string intToStr(int x);
SDL_Rect PosToCenterPicture(const SDL_Rect *imageToCenter, const SDL_Rect *imageBackground);
bool CollisionBoxABoxB(SDL_Rect rectA, SDL_Rect rectB);
int GetDirectionFromPos(SDL_Rect *posHero, SDL_Rect *mousePos);
SDL_Rect GetCurrentSpritePosOfHero(Character* hero, bool animation);
int GetRandom(std::vector<int>& probs);
IniReader* GetRandomMobSettings(World* w);
string uintToStr(unsigned int x);
unsigned int GetMax10Pow(int num);
void SwitchesAcquisition(vector<bool> &vect, std::string filename);
void VariablesAcquisition(vector<int> &vect, std::string filename);
void VariablesWriting(vector<int> &vect, std::string filename);
void SwitchesWriting(vector<bool> &vect, std::string filename);
#endif
