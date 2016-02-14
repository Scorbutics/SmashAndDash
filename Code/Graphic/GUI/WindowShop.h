#ifndef DEF_WINDOW_SHOP
#define DEF_WINDOW_SHOP

#include "MovableWindow.h"
#include "Gameplay\Inventory\Inventory.h"
class WGameCore;

class WindowShop
{
    public:
    WindowShop(std::string squareSpriteName, std::string squareSpriteNameHighlight, std::string fichierMenu, SDL_Rect posFond);
    bool isVisible();
    void hide(bool x);
    DialogMenu* getShopBar();
    void reset();
    void refresh();
    ~WindowShop();
	void display();

    private:
    int m_boolQuit;
    MovableWindow m_dialog, m_userBar, m_shopBar;
    SDL_Rect m_pos;
    Inventory m_shopInv;
    Inventory_Area *m_userInvArea, *m_shopInvArea;
    std::string m_lastClickInv;
};

#endif