#include "WindowBag.h"
//#include "Inventory_Area.h"
#include "../../ska/Graphic/GUI/Components/ButtonQuit.h"
#include "../../Utils/IDs.h"
#include "WindowTeam.h"

WindowBag::WindowBag(const ska::InputContextManager& icm, std::string fichierMenu, ska::Rectangle posFond, int taillePolice) : MovableWindow(icm, fichierMenu, posFond, taillePolice)
{
    m_boolUseObject = 0;
    m_boolGiveObject = 0;
    m_boolTrashObject = 0;
    m_curObjectPos.x = m_curObjectPos.y = 0;
}

ska::Point<int> WindowBag::getCurObjectPos()
{
    return m_curObjectPos;
}

void WindowBag::reset()
{
    clear();
	ska::Rectangle buf;
    buf.y = TAILLEBLOCFENETRE*3/4;
    buf.x = 9*TAILLEBLOCFENETRE;
	//addWidget(std::unique_ptr<ska::ButtonQuit>(new ska::ButtonQuit(*this, buf, "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"close_button.png")));
    buf.x = 4*TAILLEBLOCFENETRE;
    //addTextArea("PokéSac", 20, buf);
    buf.x = TAILLEBLOCFENETRE;
    buf.y = TAILLEBLOCFENETRE*2;
    //this->addInventory(wScreen.getInventory(), buf);
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
	//GUI& gui = wScreen.getGUI();

    //lors d'un clic sur "Utiliser" du menu contextuel apparaissant apres un clic droit sur un objet
    if(m_boolUseObject != 0)
    {
        //Inventory_Area* invArea = this->getInventoryArea(0);
		//ska::Point<int> invAreaAbsolutePos = invArea->getAbsolutePos();
        //gui.getClickMenu()->hide(true);
        m_boolUseObject = 0;
		/*m_curObjectPos = gui.getClickMenu()->getRect();
        m_curObjectPos.x -= invAreaAbsolutePos.x;
        m_curObjectPos.y -= invAreaAbsolutePos.y;
        if(invArea->getObjectAtPos(m_curObjectPos) != NULL)
        {

            gui.getClickMenu()->setPos(ska::Point<int>());
            if(invArea->getObjectAtPos(m_curObjectPos)->getEffect() == "onPkmn")
            {
                gui.getWindowTeam()->reset("use_object");
                gui.getWindowTeam()->hide(false);
                this->hide(true);
            }
            else
                invArea->useObjectAtPos(m_curObjectPos);
        }*/
    }



}

WindowBag::~WindowBag()
{
}

