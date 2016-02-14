#ifndef DEF_INVENTORY_AREA
#define DEF_INVENTORY_AREA

#include <iostream>
#include <string>
#include <SDL2/SDL.h>
#include "DynamicWindowArea.h"
#include "Gameplay/Inventory/Inventory.h"

using namespace std;

class Inventory_Area : public DynamicWindowArea
{
    public:
    Inventory_Area(DialogMenu* parent, Inventory* inv, SDL_Rect relativePos);
    virtual void refresh();
    virtual void display();
	virtual std::string getKey(){return "";};
    virtual void forceValue(unsigned int){};
    Inventory* getInventory();
    unsigned int getAmountAtPos(SDL_Rect relativePos);
    Object* getObjectAtPos(SDL_Rect relativePos);
    void useObjectAtPos(SDL_Rect objectPos);
    void useObjectAtPos(SDL_Rect objectPos, int i); //i = indice du pokémon sur lequel appliquer l'objet
    virtual ~Inventory_Area();

    protected:
    unsigned int getIndexFromPos(SDL_Rect relativePos);

    Inventory *m_inv;
    SDL_Rect m_curObjectPos, m_lastObjectPos;

};


#endif
