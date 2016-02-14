#ifndef DEF_TOOLBAR
#define DEF_TOOLBAR

#include <iostream>
#include "../../Gameplay\Inventory\Inventory.h"
#include "MovableWindow.h"


class Inventory_Area;
class WGameCore;

class ToolBar : public MovableWindow
{
    public:
    ToolBar(std::string squareSpriteName, std::string squareSpriteNameHighlight, std::string fichierMenu, SDL_Rect posFond);
    void refresh();
    ~ToolBar();

    private:
    Inventory m_inv;
    Inventory_Area* m_invArea;
};

#endif
