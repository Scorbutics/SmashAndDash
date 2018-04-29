#include "Utils/SkaConstants.h"

#define ENTITEMAX 151 //Nombre d'entités TOTALES dans le jeu ENTIER

#define TAILLEBLOC 48 //Taille d'un bloc dans le chipset du monde courant

#define BATTLE_START_TICKS 1000
#define BATTLE_START_CHANCE 5
#define BATTLE_START_DISTANCE_SQUARED 20736 // 3 * 48 = 3 blocs

//Types de boutons pouvant composer une fenêtre
#define BUTTON_CLOSE 0
#define BUTTON_TEXT_AREA 1
#define BUTTON_IMAGE_AREA 2
#define BUTTON_SCROLL_TEXT 3
#define BUTTON_INVENTORY_AREA 4
#define BUTTON_CHOICE 5
#define REFRESH_PNJWINDOW_COUNT 20

#define SKILL_PROJECTILE 0
#define SKILL_MELEE 1


#define CHARACTER_ANIMATION_MOVE 0
#define CHARACTER_ANIMATION_ATTACK 1

#define STATS_TYPE_LINEAR 0
#define STATS_TYPE_SQUARE 1

#define STATS_HP 0
#define STATS_ATTACK 1
#define STATS_DEFENSE 2
#define STATS_SPE_ATTACK 3
#define STATS_SPE_DEFENSE 4
#define STATS_SPEED 5

#define AI_BALANCED 0
#define AI_OFFENSIVE 1
#define AI_DEFENSIVE 2



//nombre de sprites différents dans la fiche d'un personnage sur une même ligne
#define HORIZONTAL_SPRITE_NUMBER_PER_CHAR_SHEET 6
//idem mais verticalement, sur une même colonne
#define VERTICAL_SPRITE_NUMBER_PER_CHAR_SHEET 8

#define SPAWN_LIMIT_ALLOWED 50

#define ID_CURRENT_POKEMON INT16_MAX-1
#define ID_CURRENT_OPPONENT INT16_MAX


#define EVENT_MOUSE 0
#define EVENT_KEYBOARD 1

#define POKEMON_TEAM_MAX_SIZE 6

#define GUI_DEFAULT_DISPLAY_PRIORITY 32000
#define SKILLS_DEFAULT_DISPLAY_PRIORITY GUI_DEFAULT_DISPLAY_PRIORITY - 1

#define DEFAULT_BGM "./Music/Greencity.mp3"
