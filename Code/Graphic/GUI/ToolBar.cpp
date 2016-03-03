#include "ToolBar.h"
#include "../../Gameplay\WGameCore.h"
#include "../../Gameplay\Inventory\Inventory.h"
#include "../../Graphic\GUI\Inventory_Area.h"
#include "../../Utils/IDs.h"

using namespace std;

ToolBar::ToolBar(string squareSpriteName, string squareSpriteNameHighlight, string fichierMenu, ska::Rectangle posFond) : MovableWindow(fichierMenu, posFond, 22)
{
	ska::Rectangle pos;
	pos.x = TAILLEBLOCFENETRE/2;
	pos.y = TAILLEBLOCFENETRE/2;
	pos.w = posFond.w;
	pos.h = posFond.h;

	m_inv.load(squareSpriteName, squareSpriteNameHighlight);
    this->addInventory(m_inv, pos);
    m_invArea = this->getInventoryArea(0);
}


void ToolBar::refresh()
{
	WGameCore& wScreen = WGameCore::getInstance();
	ska::KeyInput* in = wScreen.getInputListener().getKeyInput();

    MovableWindow::refresh();
    if(m_invArea != NULL)
        m_invArea->refresh();
	else
		return;

	Inventory* inv = m_invArea->getInventory();
	Object* obj;

    if(in->getKeyState(SDL_SCANCODE_Z))
    {
		obj = inv->getObjectFromIndex(0);
        in->setKeyState(SDL_SCANCODE_Z,0);
		if(obj != NULL)
		{
			if(obj->getEffect() == "onPkmn")
				inv->use(obj->getID(), 1, 0);
			else 
				inv->use(obj->getID(), 1);
		}
    }
    else if(in->getKeyState(SDL_SCANCODE_X))
    {
		obj = inv->getObjectFromIndex(1);
        in->setKeyState(SDL_SCANCODE_X,0);
		if(obj != NULL)
		{
			if(obj->getEffect() == "onPkmn")
				inv->use(obj->getID(), 1, 0);
			else 
				inv->use(obj->getID(), 1);
		}
    }
    else if(in->getKeyState(SDL_SCANCODE_C))
    {
		obj = inv->getObjectFromIndex(2);
        in->setKeyState(SDL_SCANCODE_C,0);
		if(obj != NULL)
		{
			if(obj->getEffect() == "onPkmn")
				inv->use(obj->getID(), 1, 0);
			else 
				inv->use(obj->getID(), 1);
		}
    }
    else if(in->getKeyState(SDL_SCANCODE_V))
    {
		obj = inv->getObjectFromIndex(3);
        in->setKeyState(SDL_SCANCODE_V,0);
		if(obj != NULL)
		{
			if(obj->getEffect() == "onPkmn")
				inv->use(obj->getID(), 1, 0);
			else 
				inv->use(obj->getID(), 1);
		}
    }
    else if(in->getKeyState(SDL_SCANCODE_B))
    {
		obj = inv->getObjectFromIndex(4);
        in->setKeyState(SDL_SCANCODE_B,0);
		if(obj != NULL)
		{
			if(obj->getEffect() == "onPkmn")
				inv->use(obj->getID(), 1, 0);
			else 
				inv->use(obj->getID(), 1);
		}
    }
    else if(in->getKeyState(SDL_SCANCODE_N))
    {
		obj = inv->getObjectFromIndex(5);
        in->setKeyState(SDL_SCANCODE_N,0);
		if(obj != NULL)
		{
			if(obj->getEffect() == "onPkmn")
				inv->use(obj->getID(), 1, 0);
			else 
				inv->use(obj->getID(), 1);
		}
    }

}

ToolBar::~ToolBar()
{
}
