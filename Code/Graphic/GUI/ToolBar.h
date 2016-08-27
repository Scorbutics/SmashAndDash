#pragma once
#include <iostream>
#include "../../Gameplay\Inventory\Inventory.h"
#include "MovableWindow.h"


//class Inventory_Area;

class ToolBar : public MovableWindow
{
public:
	ToolBar(const ska::InputContextManager& icm, std::string squareSpriteName, std::string squareSpriteNameHighlight, std::string fichierMenu, ska::Rectangle posFond);
    void refresh();
    ~ToolBar();

    private:
    Inventory m_inv;
    //Inventory_Area* m_invArea;
};

using ToolBarPtr = std::unique_ptr<ToolBar>;

