#ifndef DEF_INVENTORY_AREA
#define DEF_INVENTORY_AREA

#include <iostream>
#include <string>
#include "DynamicWindowArea.h"
#include "../../Gameplay/Inventory/Inventory.h"
#include "../Rectangle.h"

using namespace std;

class Inventory_Area : public DynamicWindowArea
{
    public:
		Inventory_Area(DialogMenu* parent, Inventory* inv, ska::Rectangle relativePos);
    virtual void refresh();
    virtual void display();
	virtual std::string getKey(){return "";};
    virtual void forceValue(unsigned int){};
    Inventory* getInventory();
	unsigned int getAmountAtPos(ska::Rectangle relativePos);
	Object* getObjectAtPos(ska::Rectangle relativePos);
	void useObjectAtPos(ska::Rectangle objectPos);
	void useObjectAtPos(ska::Rectangle objectPos, int i); //i = indice du pokémon sur lequel appliquer l'objet
    virtual ~Inventory_Area();

    protected:
		unsigned int getIndexFromPos(ska::Rectangle relativePos);

    Inventory *m_inv;
	ska::Rectangle m_curObjectPos, m_lastObjectPos;

};


#endif
