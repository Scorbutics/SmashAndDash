#ifndef DEF_GESTIONEVENTS
#define DEF_GESTIONEVENTS

#include "Layer.h"
#include "Character.h"
#include "WGameCore.h"

#include <vector>

int GestionEvents();
string ExtractTo(int depart, std::string chaine, char jusquaCeCaractere, int *entier = NULL);
void TeleportHeroToMap(std::string param);
string intToStr(int x);
#endif
