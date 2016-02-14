#include "WindowBag.h"
#include "../../Gameplay\WGameCore.h"
#include "Inventory_Area.h"


using namespace std;

WindowBag::WindowBag(string fichierMenu, SDL_Rect posFond, int taillePolice) : MovableWindow(fichierMenu, posFond, taillePolice)
{
    m_boolUseObject = 0;
    m_boolGiveObject = 0;
    m_boolTrashObject = 0;
    m_curObjectPos.x = m_curObjectPos.y = 0;
}

SDL_Rect WindowBag::getCurObjectPos()
{
    return m_curObjectPos;
}

void WindowBag::reset()
{
    this->deleteAll();
    SDL_Rect buf;
	WGameCore& wScreen = WGameCore::getInstance();
    buf.y = TAILLEBLOCFENETRE*3/4;
    buf.x = 9*TAILLEBLOCFENETRE;
    this->addButtonClose("."FILE_SEPARATOR"Menu"FILE_SEPARATOR"close_button.png", "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"close_button_active.png", buf);
    buf.x = 4*TAILLEBLOCFENETRE;
    this->addTextArea("PokéSac", 20, buf);
    buf.x = TAILLEBLOCFENETRE;
    buf.y = TAILLEBLOCFENETRE*2;
    this->addInventory(wScreen.getInventory(), buf);
}

int* WindowBag::getBoolUseObject()
{
    return &m_boolUseObject;
}

int* WindowBag::getBoolTrashObject()
{
    return &m_boolTrashObject;
}

int* WindowBag::getBoolGiveObject()
{
    return &m_boolGiveObject;
}

void WindowBag::refresh()
{
    MovableWindow::refresh();
	WGameCore& wScreen = WGameCore::getInstance();
	GUI& gui = wScreen.getGUI();

    //lors d'un clic sur "Utiliser" du menu contextuel apparaissant apres un clic droit sur un objet
    if(m_boolUseObject != 0)
    {
        Inventory_Area* invArea = this->getInventoryArea(0);
        SDL_Rect invAreaAbsolutePos = invArea->getAbsolutePos();
        gui.getClickMenu()->hide(true);
        m_boolUseObject = 0;
        m_curObjectPos = *gui.getClickMenu()->getPos();
        m_curObjectPos.x -= invAreaAbsolutePos.x;
        m_curObjectPos.y -= invAreaAbsolutePos.y;
        if(invArea->getObjectAtPos(m_curObjectPos) != NULL)
        {
			SDL_Rect nullPos;
			nullPos.x = 0;
			nullPos.y = 0;
			nullPos.w = 0;
			nullPos.h = 0;

            gui.getClickMenu()->setPos(nullPos);
            if(invArea->getObjectAtPos(m_curObjectPos)->getEffect() == "onPkmn")
            {
                gui.getWindowTeam()->reset("use_object");
                gui.getWindowTeam()->hide(false);
                this->hide(true);
            }
            else
                invArea->useObjectAtPos(m_curObjectPos);
        }
    }



}

WindowBag::~WindowBag()
{
}

