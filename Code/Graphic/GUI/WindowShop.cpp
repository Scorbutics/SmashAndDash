
#include "Utils\IDs.h"
#include "WindowShop.h"
#include "GUI.h"
#include "Gameplay\WGameCore.h"
#include "Utils\Singleton_template.h"
#include "Utils\ChargementImages.h"

using namespace std;

WindowShop::WindowShop(std::string squareSpriteName, std::string squareSpriteNameHighlight, std::string fichierMenu, SDL_Rect posFond): m_dialog(fichierMenu, posFond, 22), m_userBar(fichierMenu, posFond, 22), m_shopBar(fichierMenu, posFond, 22)
{
	m_shopInv.load(squareSpriteName, squareSpriteNameHighlight);
    m_pos = posFond;
    m_dialog.setPos(posFond.x, posFond.y);
    m_dialog.resize(4*TAILLEBLOCFENETRE, posFond.h);
    m_shopBar.setPos(posFond.x+4*TAILLEBLOCFENETRE, posFond.y);
    m_shopBar.resize(posFond.w-4*TAILLEBLOCFENETRE, posFond.h-4*TAILLEBLOCFENETRE);
    m_userBar.setPos(posFond.x+4*TAILLEBLOCFENETRE, posFond.y + posFond.h - 4*TAILLEBLOCFENETRE);


    m_boolQuit = 0;
}

bool WindowShop::isVisible()
{
    return (m_userBar.isVisible() && m_shopBar.isVisible() && m_dialog.isVisible());
}

DialogMenu* WindowShop::getShopBar()
{
    return (DialogMenu*)&(m_shopBar);
}

void WindowShop::hide(bool x)
{
    m_userBar.hide(x);
    m_shopBar.hide(x);
    m_dialog.hide(x);
}

void WindowShop::reset()
{
	WGameCore& wScreen = WGameCore::getInstance();
	GUI& gui = wScreen.getGUI();

    m_shopBar.deleteAll();
    m_userBar.deleteAll();



    m_dialog.setPos(m_pos.x, m_pos.y);
    m_dialog.resize(4*TAILLEBLOCFENETRE, m_pos.h);
    m_shopBar.setPos(m_pos.x+4*TAILLEBLOCFENETRE, m_pos.y);
    m_shopBar.resize(m_pos.w-4*TAILLEBLOCFENETRE, m_pos.h-4*TAILLEBLOCFENETRE);
    m_userBar.setPos(m_pos.x+4*TAILLEBLOCFENETRE, m_pos.y + m_pos.h - 4*TAILLEBLOCFENETRE);
    m_userBar.resize(m_pos.w - 4*TAILLEBLOCFENETRE, 4*TAILLEBLOCFENETRE);

    gui.hide(true);
	gui.getWindowBag()->hide(true);
	gui.getWindowTeam()->hide(true);
	gui.getWindowSettings()->hide(true);
	gui.getToolbar()->hide(true);
    this->hide(false);

    vector<int> vBool;
    vBool.push_back(0);
    vBool.push_back(1);
    vector<string> vSell, vBuy, vQuit;
    vSell.push_back("Vendre");
    vSell.push_back("Vendu !");
    vBuy.push_back("Acheter");
    vBuy.push_back("Achet� !");
    vQuit.push_back("Quitter");
    vQuit.push_back("Quitter");

    SDL_Rect buf;
    buf.y = (TAILLEBLOCFENETRE*3)/4;
    buf.x = m_shopBar.getPos()->w/2 - TAILLEBLOCFENETRE;
    m_shopBar.addTextArea("Shop", 20, buf);
    buf.x = m_userBar.getPos()->w/2 - 2*TAILLEBLOCFENETRE;
    m_userBar.addTextArea("Utilisateur", 20, buf);
    buf.y = m_userBar.getPos()->h - TAILLEBLOCFENETRE;
    buf.x = m_userBar.getPos()->w - 2*TAILLEBLOCFENETRE;
    m_userBar.addButton(buf, "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"button.png", "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"buttonpressed.png", &m_boolQuit, vBool, vQuit, 22, "window_shop_quit");

	buf.x = TAILLEBLOCFENETRE/2;
	buf.y =	2*TAILLEBLOCFENETRE;
	buf.w = m_shopBar.getPos()->w;
	buf.h = m_shopBar.getPos()->h;
    m_shopBar.addInventory(m_shopInv, buf);

	buf.w = m_userBar.getPos()->w;
	buf.h = m_userBar.getPos()->h;

    m_shopInvArea = m_shopBar.getInventoryArea(0);
    m_userBar.addInventory(wScreen.getInventory(), buf);
    m_userInvArea = m_userBar.getInventoryArea(0);

    m_shopInv.clear();
    m_shopInv.add(0,1);
    m_shopInv.add(1, 1);
	m_shopInv.add(2, 1);

}

void WindowShop::refresh()
{
	WGameCore& wScreen = WGameCore::getInstance();
    SDL_Rect mousePos = wScreen.getInputListener().getMouseInput()->getMousePos();
	MouseCursor& mouseCur = wScreen.getMouseCursor();
	MouseInput* in = wScreen.getInputListener().getMouseInput();

    m_pos.x = m_dialog.getPos()->x;
    m_pos.y = m_dialog.getPos()->y;
    m_shopBar.setPos(m_pos.x+4*TAILLEBLOCFENETRE, m_pos.y);
    m_userBar.setPos(m_pos.x+4*TAILLEBLOCFENETRE, m_pos.y + m_pos.h - 4*TAILLEBLOCFENETRE);

    //Lors d'un clic sur "Quitter", on quitte...
    if(m_boolQuit != 0)
    {
        m_boolQuit = 0;
        //m_userInv.clear(); //avant tout, on efface l'inventaire qu'utilisait l'utilisateur pour acheter des objets
        this->hide(true);
        wScreen.getGUI().hide(false);
        wScreen.getGUI().getToolbar()->hide(false);
    }

    //Lors d'un d�pot d'objet dans notre inventaire, on ach�te
    if(mouseCur.getObject() != NULL && in->mouseClick(SDL_BUTTON_LEFT) && IsPositionInBox(&mousePos, m_userBar.getPos()) && m_lastClickInv == "shop")
    {
        wScreen.getInventory().add(mouseCur.getObject()->getID(), mouseCur.getObjectAmount());
        m_shopInv.remove(mouseCur.getObject()->getID(), mouseCur.getObjectAmount());
        m_shopInv.add(mouseCur.getObject()->getID(), mouseCur.getObjectAmount());
        mouseCur.setObjectAmount(0);
        mouseCur.removeObject();
        in->setMouseLastState(SDL_BUTTON_LEFT, 0);
        in->setMouseState(SDL_BUTTON_LEFT, 0);

        //TODO: perdre les sous
    }
    else if(mouseCur.getObject() == NULL && in->mouseClick(SDL_BUTTON_LEFT) && IsPositionInBox(&mousePos, m_userBar.getPos()))
    {
        m_lastClickInv = "user";
    }

    //Lors d'un d�pot d'objet dans le shop, on vend
    if(mouseCur.getObject() != NULL && in->mouseClick(SDL_BUTTON_LEFT) && IsPositionInBox(&mousePos, m_shopBar.getPos()) &&  m_lastClickInv == "user")
    {
        mouseCur.setObjectAmount(0);
        mouseCur.removeObject();
        //TODO: gagner les sous
        in->setMouseLastState(SDL_BUTTON_LEFT, 0);
        in->setMouseState(SDL_BUTTON_LEFT, 0);
    }
    else if(mouseCur.getObject() == NULL && in->mouseClick(SDL_BUTTON_LEFT) && IsPositionInBox(&mousePos, m_shopBar.getPos()))
    {
        m_lastClickInv = "shop";
    }

    m_userBar.refresh();
    m_shopBar.refresh();
    m_dialog.refresh();
}

void WindowShop::display()
{
	m_userBar.display();
	m_shopBar.display();
	m_dialog.display();
}

WindowShop::~WindowShop()
{
}