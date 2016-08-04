#ifndef DEF_INVENTORY_AREA
#define DEF_INVENTORY_AREA

#include <iostream>
#include <string>
#include "DynamicWindowArea.h"
#include "../../Gameplay/Inventory/Inventory.h"
#include "../../ska/Graphic/Rectangle.h"

class Inventory_Area : public DynamicWindowArea
{
    public:
		Inventory_Area(DialogMenu* parent, Inventory* inv, ska::Point<int> relativePos);
    virtual void refresh();
    virtual void display();
	virtual std::string getKey(){return "";};
    virtual void forceValue(unsigned int){};
    Inventory* getInventory();
	ska::Rectangle getRect() const;
	unsigned int getAmountAtPos(ska::Point<int> relativePos);
	Object* getObjectAtPos(ska::Point<int> relativePos);
	void useObjectAtPos(ska::Point<int> objectPos);
	void useObjectAtPos(ska::Point<int> objectPos, int i); //i = indice du pokémon sur lequel appliquer l'objet
    virtual ~Inventory_Area();

    protected:
		unsigned int getIndexFromPos(ska::Point<int> relativePos);

    Inventory *m_inv;
	ska::Point<int> m_curObjectPos, m_lastObjectPos;

};


#endif
