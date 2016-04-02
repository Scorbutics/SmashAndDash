#include <string>
#include <fstream>
#include <sstream>
#include <math.h>

#include "../../Utils\IDs.h"
#include "GUI.h"
#include "../../Gameplay\WGameCore.h"
#include "Window_Area.h"
#include "Inventory_Area.h"
#include "MouseCursor.h"
#include "../../Utils/ChargementImages.h"
#include "../../ska/Utils/StringUtils.h"
#include "../../Gameplay/Data/Statistics.h"
#include "../../Gameplay/Fight/Skill.h"
#include "../../ska/Utils/RectangleUtils.h"
#include "MouseCursor.h"
#include "DialogMenu.h"
#include "WindowSettings.h"
#include "WindowBag.h"
#include "WindowTeam.h"
#include "WindowShop.h"
#include "ToolBar.h"


#define SCROLL_BUTTON_SPEED 3

GUI::GUI()
{
	WGameCore& wScreen = WGameCore::getInstance();

    m_refreshCount = REFRESH_PNJWINDOW_COUNT;
    m_lastMouseState = 0;
    m_side = 0;
    m_pnj = NULL;
    m_isMovingWindow = m_hide = false;


	ska::Rectangle menuPos;
    menuPos.x = 0;
    menuPos.y = wScreen.getHeight() - TAILLEBLOCFENETRE*4;
    menuPos.w = wScreen.getWidth()/2;
    menuPos.h = TAILLEBLOCFENETRE*4;
    m_dial = unique_ptr<DialogMenu>(new DialogMenu("Dialogue", "", "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"menu.png", menuPos, 22));

    menuPos.x += TAILLEBLOCFENETRE;
    menuPos.y -= TAILLEBLOCFENETRE*5;
    menuPos.w = 4*TAILLEBLOCFENETRE;
    menuPos.h = 5*TAILLEBLOCFENETRE;
    m_imgDial = unique_ptr<DialogMenu>(new DialogMenu("ImgName", "."FILE_SEPARATOR"Sprites"FILE_SEPARATOR"Facesets"FILE_SEPARATOR"1.png", "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"menu.png", menuPos, 22));

    menuPos.x = 0;
    menuPos.y = 0;
    menuPos.w = 8*TAILLEBLOCFENETRE;
    menuPos.h = 5*TAILLEBLOCFENETRE;
    m_pokeInfoWindow = unique_ptr<DialogMenu>(new DialogMenu("", "", "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"menu.png", menuPos, 22, false));

    menuPos.h = 4*TAILLEBLOCFENETRE;
    menuPos.y = 5*TAILLEBLOCFENETRE;
    menuPos.w = 5*TAILLEBLOCFENETRE;
    m_facesetPkmn = unique_ptr<DialogMenu>(new DialogMenu("", "", "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"menu.png", menuPos, 22, false));

    menuPos.x = 0;
    menuPos.y = wScreen.getHeight() - 2*TAILLEBLOCFENETRE;
    menuPos.w = 9*TAILLEBLOCFENETRE;
    menuPos.h = 2*TAILLEBLOCFENETRE;
    m_attackPokemon = unique_ptr<DialogMenu>(new DialogMenu("", "", "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"menu.png", menuPos, 22, false));

    menuPos.x = wScreen.getWidth() - 2*TAILLEBLOCFENETRE;
    m_attackOpponent = unique_ptr<DialogMenu>(new DialogMenu("", "", "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"menu.png", menuPos, 22, false));


    menuPos.x = 0;
    menuPos.y = 0*TAILLEBLOCFENETRE;
    menuPos.w = 6*TAILLEBLOCFENETRE;
    menuPos.h = 3*TAILLEBLOCFENETRE;
    m_pnjInfoWindow = unique_ptr<DialogMenu>(new DialogMenu("", "", "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"menu.png", menuPos, 22, false));

    menuPos.y = 5*TAILLEBLOCFENETRE;
    menuPos.x += 3*TAILLEBLOCFENETRE;
    menuPos.h = 4*TAILLEBLOCFENETRE;
    menuPos.w = 5*TAILLEBLOCFENETRE;
    m_facesetOpponent = unique_ptr<DialogMenu>(new DialogMenu("", "", "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"menu.png", menuPos, 22, false));

    menuPos.x = menuPos.y = 0;
    menuPos.w = 4*TAILLEBLOCFENETRE;
    menuPos.h = 2*TAILLEBLOCFENETRE;
    m_clickMenu = unique_ptr<DialogMenu>(new DialogMenu("", "", "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"menu.png", menuPos, 22, false));


    menuPos.w = 10*TAILLEBLOCFENETRE;
    menuPos.h = 5*TAILLEBLOCFENETRE;
    m_wSettings = unique_ptr<WindowSettings>(new WindowSettings("."FILE_SEPARATOR"Menu"FILE_SEPARATOR"menu.png", menuPos, 22));
    m_wTeam = unique_ptr<WindowTeam>(new WindowTeam("."FILE_SEPARATOR"Menu"FILE_SEPARATOR"menu.png", menuPos, 22));
    m_wBag = unique_ptr<WindowBag>(new WindowBag("."FILE_SEPARATOR"Menu"FILE_SEPARATOR"menu.png", menuPos, 22));

	menuPos.x = wScreen.getWidth() - 7*TAILLEBLOCFENETRE;
	menuPos.y = 0;
	menuPos.w = 7*TAILLEBLOCFENETRE;
	menuPos.h = 2*TAILLEBLOCFENETRE;
    m_toolBar = unique_ptr<ToolBar>(new ToolBar("."FILE_SEPARATOR"Menu"FILE_SEPARATOR"inventory_square.png", "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"inventory_square_highlight.png", "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"menu.png", menuPos));
	
	menuPos.x = 5*TAILLEBLOCFENETRE;
	menuPos.y = 2*TAILLEBLOCFENETRE;
	menuPos.w = 10*TAILLEBLOCFENETRE;
	menuPos.h = 10*TAILLEBLOCFENETRE;
    m_wShop = unique_ptr<WindowShop>(new WindowShop("."FILE_SEPARATOR"Menu"FILE_SEPARATOR"inventory_square.png", "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"inventory_square_highlight.png", "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"menu.png", menuPos));
    m_toolBar->hide(false);

    initButtons();

}

WindowBagPtr& GUI::getWindowBag() {
    return m_wBag;
}

void GUI::initButtons()
{
	WGameCore& wScreen = WGameCore::getInstance();
	ska::Rectangle buf;
    buf.w = (TAILLEBLOCFENETRE)*2;
    buf.h = (TAILLEBLOCFENETRE)*2;
    buf.x = wScreen.getWidth() - 13*TAILLEBLOCFENETRE;
    buf.y = 0;

	for (unsigned int i = 0; i < m_buttonList.size(); i++) {
		m_buttonList[i]->deleteAll();
	}

    m_buttonList.clear();
    m_buttonList.push_back(DialogMenuPtr (new DialogMenu("", "."FILE_SEPARATOR"Sprites"FILE_SEPARATOR"Icones"FILE_SEPARATOR"pokeball.png", "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"toolsmenu.png", buf, 22, false)));
    m_buttonList[0]->setActionClic("team");

    buf.x += 5*TAILLEBLOCFENETRE/2;
	m_buttonList.push_back(DialogMenuPtr(new DialogMenu("", "."FILE_SEPARATOR"Sprites"FILE_SEPARATOR"Icones"FILE_SEPARATOR"pokedex.png", "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"toolsmenu.png", buf, 22, false)));
    m_buttonList[1]->setActionClic("pokedex");

    buf.x += 5*TAILLEBLOCFENETRE/2;
	m_buttonList.push_back(DialogMenuPtr(new DialogMenu("", "."FILE_SEPARATOR"Sprites"FILE_SEPARATOR"Icones"FILE_SEPARATOR"bag.png", "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"toolsmenu.png", buf, 22, false)));
    m_buttonList[2]->setActionClic("pokebag");

    buf.x += 5*TAILLEBLOCFENETRE/2;
	m_buttonList.push_back(DialogMenuPtr(new DialogMenu("", "."FILE_SEPARATOR"Sprites"FILE_SEPARATOR"Icones"FILE_SEPARATOR"card.png", "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"toolsmenu.png", buf, 22, false)));
    m_buttonList[3]->setActionClic("trainer_card");

    buf.x += 5*TAILLEBLOCFENETRE/2;
	m_buttonList.push_back(DialogMenuPtr(new DialogMenu("", "."FILE_SEPARATOR"Sprites"FILE_SEPARATOR"Icones"FILE_SEPARATOR"tape.png", "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"toolsmenu.png", buf, 22, false)));
    m_buttonList[4]->setActionClic("options");

    m_buttonScroll.resize(m_buttonList.size());
	for (unsigned int i = 0; i < m_buttonList.size(); i++) {
		m_buttonScroll[i] = 0;
	}


}

DialogMenuPtr& GUI::getFacesetPkmn() {
    return m_facesetPkmn;
}

WindowSettingsPtr& GUI::getWindowSettings() {
    return m_wSettings;
}

DialogMenuPtr& GUI::getClickMenu() {
    return m_clickMenu;
}

WindowTeamPtr& GUI::getWindowTeam() {
    return m_wTeam;
}

DialogMenuPtr& GUI::getFacesetOpponent() {
    return m_facesetOpponent;
}

WindowShopPtr& GUI::getWindowShop() {
    return m_wShop;
}

void GUI::display() {

	dialogDisplay();

	if (m_hide) {
		return;
	}

	for (unsigned int i = 0; i < m_buttonList.size(); i++) {
		if (m_buttonList[i] != NULL) {
			m_buttonList[i]->display();
		}
	}

	MouseCursor& mouseCursor = WGameCore::getInstance().getMouseCursor();
	mouseCursor.displaySelectedPokemon();
	mouseCursor.displaySelectedObject();
		
}

void GUI::refresh()
{
	WGameCore& wScreen = WGameCore::getInstance();
	
	const ska::InputActionContainer& in = wScreen.getActions();

	const ska::InputRange& mousePos = wScreen.getRanges()[ska::InputRangeType::MousePos];
	const ska::InputRange& lastMousePos = wScreen.getRanges()[ska::InputRangeType::LastMousePos];
	ska::Rectangle bufButtonPos;

    bufButtonPos.w = (TAILLEBLOCFENETRE)*2;
    bufButtonPos.h = (TAILLEBLOCFENETRE)*2;
    bufButtonPos.x = wScreen.getWidth() - 13*TAILLEBLOCFENETRE;
    bufButtonPos.y = 0;

	if (m_hide) {
		return;
	}

    for(unsigned int i = 0; i < m_buttonList.size(); i++)
    {
        if(m_buttonList[i] != NULL)
        {
            m_buttonList[i]->refresh();
			ska::Rectangle buttonPos = m_buttonList[i]->getRect();
			if (ska::RectangleUtils::isPositionInBox(mousePos, buttonPos))
            {


				if (m_buttonScroll[i] < buttonPos.h / 2)
                    m_buttonScroll[i] += SCROLL_BUTTON_SPEED;


                if(in[ska::InputAction::LClic] && !m_wShop->isVisible())
                {

                    if(m_buttonList[i]->getActionClic() == "options")
                    {
						m_wSettings->setPos(m_wTeam->getWidth() + m_wBag->getWidth(), 0);
                        m_wSettings->reset();
                        m_wSettings->hide(false);

                    }
                    else if(m_buttonList[i]->getActionClic() == "pokebag")
                    {
						m_wBag->setPos(m_wTeam->getWidth(), 0);
                        m_wBag->reset();
                        m_wBag->hide(false);
                    }
                    else if(m_buttonList[i]->getActionClic() == "team")
                    {
						m_wTeam->setPos(0, 0);
                        m_wTeam->reset("show");
                        m_wTeam->hide(false);
                    }

                }

				if (!ska::RectangleUtils::isPositionInBox(lastMousePos, buttonPos)) //si on change de bouton de la gui, on actualise le curseur
				{
					wScreen.getMouseCursor().modifyHint(m_buttonList[i]->getActionClic());
					wScreen.getMouseCursor().hideHint(false);
				}

                
            }
            else
                m_buttonScroll[i] = 0;

            if(m_side == 0) // GUI disposée en bas à droite de l'écran
            {
                m_buttonList[i]->setPos(bufButtonPos.x, (wScreen.getHeight() - m_buttonList[i]->getHeight()/2) - m_buttonScroll[i]);
				m_buttonList[i]->setPosImg(ska::RectangleUtils::posToCenterPicture(m_buttonList[i]->getPosImg(), bufButtonPos).x, ska::RectangleUtils::posToCenterPicture(m_buttonList[i]->getPosImg(), m_buttonList[i]->getRect()).y - TAILLEBLOCFENETRE / 4);
                bufButtonPos.x += 5*TAILLEBLOCFENETRE/2;
            }


        }
    }

    if(wScreen.getMouseCursor().isActiveHint(this) )
        wScreen.getMouseCursor().displayHint();

	if (in[ska::InputAction::RClic])
        this->setClickMenu();



}

ToolBarPtr& GUI::getToolbar() {
    return m_toolBar;
}


void GUI::setClickMenu()
{
	WGameCore& wScreen = WGameCore::getInstance();
	ska::Rectangle invAreaAbsolutePos, buttonPos;

	const ska::InputActionContainer& in = wScreen.getActions();

	const ska::InputRange& mousePos = wScreen.getRanges()[ska::InputRangeType::MousePos];
	const ska::InputRange& lastMousePos = wScreen.getRanges()[ska::InputRangeType::LastMousePos];

    vector<int> vBool;
    vBool.push_back(0);
    vBool.push_back(1);

    Inventory_Area* invArea = m_wBag->getInventoryArea(0);
    if(invArea != NULL && m_wBag->isVisible())
    {
        invAreaAbsolutePos = invArea->getAbsolutePos();
		if (ska::RectangleUtils::isPositionInBox(mousePos, invAreaAbsolutePos) && !ska::RectangleUtils::isPositionInBox(mousePos, m_clickMenu->getRect()))
        {
			ska::Point<int> posClickMenu = mousePos;
            posClickMenu.x -= 5;
            posClickMenu.y -= 5;
            m_clickMenu->setPos(posClickMenu);
            m_curObjectPos = m_clickMenu->getPos();
            m_curObjectPos.x -= invAreaAbsolutePos.x;
            m_curObjectPos.y -= invAreaAbsolutePos.y;
            if(invArea->getObjectAtPos(m_curObjectPos) != NULL)
            {
                vector<string> vUse, vGive, vTrash;
                vUse.push_back("Utiliser");
                vUse.push_back("Ok !");
                vGive.push_back("Donner");
                vGive.push_back("Ok !");
                vTrash.push_back("Jeter");
                vTrash.push_back("Ok !");

                m_clickMenu->deleteAll();
                buttonPos.x = 32;
                buttonPos.y = 16;
                m_clickMenu->addButton(buttonPos, "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"buttonclickmenu.png", "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"buttonclickmenupressed.png", m_wBag->getBoolUseObject(), vBool, vUse, 18, "inventory_menu_use");
                buttonPos.y += 15;
                m_clickMenu->addButton(buttonPos, "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"buttonclickmenu.png", "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"buttonclickmenupressed.png", m_wBag->getBoolGiveObject(), vBool, vGive, 18, "inventory_menu_give");
                buttonPos.y += 15;
                m_clickMenu->addButton(buttonPos, "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"buttonclickmenu.png", "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"buttonclickmenupressed.png", m_wBag->getBoolTrashObject(), vBool, vTrash, 18, "inventory_menu_trash");
                m_clickMenu->hide(false);
                invArea = m_wBag->getInventoryArea(0);
            }
            else
                m_clickMenu->hide(true);



        }
		else if (!ska::RectangleUtils::isPositionInBox(mousePos, m_clickMenu->getRect()))
        {
            m_clickMenu->hide(true);
            *m_wBag->getBoolUseObject() = 0;
            *m_wBag->getBoolTrashObject() = 0;
            *m_wBag->getBoolGiveObject() = 0;
        }





    }





}


//Reset des informations de notre pokémon (à chaque refresh)
void GUI::resetAttackPokemonWindow(Character* pokemon)
{
    m_attackPokemon->deleteAll();
	ska::Rectangle buf, bufNULL;
    buf.x = TAILLEBLOCFENETRE/4;
    buf.y = 0;
    bufNULL.x = 0;
    bufNULL.y = 0;
    vector<Skill_ptr>* v = pokemon->getSkills();
    if(v != NULL && v->size() > 0)
    {
        bufNULL.w = (*v)[0]->getIcon()->getWidth();
        bufNULL.h = (*v)[0]->getIcon()->getHeight();
    
		for(unsigned int i = 0; i < (*v).size(); i++)
		{
			buf.x += (*v)[i]->getIcon()->getWidth();
			m_attackPokemon->addImageArea((*v)[i]->getIcon(), false, buf, &bufNULL);
		}
	}

}

//Reset des informations pokémon adverse (à chaque refresh)
void GUI::resetAttackOpponentWindow(Character* op)
{
    m_attackOpponent->deleteAll();
	ska::Rectangle buf, bufNULL;
    buf.x = TAILLEBLOCFENETRE/4;
    buf.y = 0;
    bufNULL.x = 0;
    bufNULL.y = 0;
    vector<Skill_ptr>* v = op->getSkills();
    if(v != NULL && v->size() > 0)
    {
        bufNULL.w = (*v)[0]->getIcon()->getWidth();
        bufNULL.h = (*v)[0]->getIcon()->getHeight();
    
		for(unsigned int i = 0; i < (*v).size(); i++)
		{
			buf.y += (*v)[i]->getIcon()->getHeight();
			m_attackOpponent->addImageArea((*v)[i]->getIcon(), false, buf, &bufNULL);

		}
	}
}

DialogMenuPtr& GUI::getAttackPokemonWindow() {
    return m_attackPokemon;
}

DialogMenuPtr& GUI::getAttackOpponentWindow() {
    return m_attackOpponent;
}

size_t GUI::getButtonListSize() {
    return m_buttonList.size();
}

void GUI::resetInfoPokemonWindow(Character* pokemon)
{
    int id;
    stringstream ss;
	WGameCore& wScreen = WGameCore::getInstance();
	ska::Rectangle buf, posHero, oRel = { 0 };

    m_pokeInfoWindow->deleteAll();
    m_pnj = pokemon;

    if(m_pnj != NULL) {
        posHero.x = (pokemon->getCenterPos().x + abs(oRel.x))/TAILLEBLOCFENETRE;
        posHero.y = (pokemon->getCenterPos().y + abs(oRel.y))/TAILLEBLOCFENETRE;

        buf.x = 6*TAILLEBLOCFENETRE+TAILLEBLOCFENETRE/2;
        buf.y = TAILLEBLOCFENETRE*3/4;
        m_pokeInfoWindow->addButtonClose("."FILE_SEPARATOR"Menu"FILE_SEPARATOR"close_button.png", "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"close_button_active.png", buf);
        buf.x = TAILLEBLOCFENETRE/2;
        buf.y = TAILLEBLOCFENETRE/2;
        m_pokeInfoWindow->addTextArea("Type 1 : " + pokemon->getDescriptor()->getType(1), 20, buf);
        buf.x += 3*TAILLEBLOCFENETRE-10;
        m_pokeInfoWindow->addTextArea("Type 2 : " + pokemon->getDescriptor()->getType(1), 20, buf);
        buf.x -= 3*TAILLEBLOCFENETRE-10;
        buf.y = TAILLEBLOCFENETRE/2 + 20;
        m_pokeInfoWindow->addTextArea("Nom : " + pokemon->getDescriptor()->getName(), 20, buf);
        buf.y = TAILLEBLOCFENETRE/2 + 60;
        m_pokeInfoWindow->addTextArea("Level : " + ska::StringUtils::intToStr(pokemon->getStatistics()->getLevel()), 20, buf);
        buf.y = TAILLEBLOCFENETRE/2 + 80;
		m_pokeInfoWindow->addTextArea("HP : " + ska::StringUtils::intToStr(pokemon->getHp()), 20, buf);
        buf.y = TAILLEBLOCFENETRE/2 + 100;
		m_pokeInfoWindow->addTextArea("Position : " + ska::StringUtils::intToStr(posHero.x) + ":" + ska::StringUtils::intToStr(posHero.y), 20, buf);

        m_facesetPkmn->deleteAll();
        id = pokemon->getID();
		if (id >= 0) {
			ss << "."FILE_SEPARATOR"Facesets"FILE_SEPARATOR"" << id << ".png";
		} else {
			ss << "."FILE_SEPARATOR"Facesets"FILE_SEPARATOR"" << -id << ".png";
		}

        buf.x = TAILLEBLOCFENETRE/2;
        buf.y = TAILLEBLOCFENETRE/2;
        m_facesetPkmn->addImageArea(ss.str(), true, buf, NULL);
    }
}

void GUI::resetInfoPNJWindow(Character* pnj)
{
	WGameCore& wScreen = WGameCore::getInstance();
	ska::Rectangle buf, posHero, oRel = { 0 };
    stringstream ss;
    int id;
    m_pnjInfoWindow->deleteAll();
    m_pnj = pnj;

    if(m_pnj != NULL) {
        posHero.x = (pnj->getCenterPos().x + abs(oRel.x))/TAILLEBLOCFENETRE;
        posHero.y = (pnj->getCenterPos().y + abs(oRel.y))/TAILLEBLOCFENETRE;

        buf.x = 4*TAILLEBLOCFENETRE+TAILLEBLOCFENETRE/2;
        buf.y = TAILLEBLOCFENETRE/2;
        m_pnjInfoWindow->addButtonClose("."FILE_SEPARATOR"Menu"FILE_SEPARATOR"close_button.png", "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"close_button_active.png", buf);
		ska::Rectangle rectSrcBuf;
        rectSrcBuf.x = 0;
        rectSrcBuf.y = pnj->getSprite()->getHeight()*2 /8;
        rectSrcBuf.w = pnj->getSprite()->getWidth()/3;
        rectSrcBuf.h = pnj->getSprite()->getHeight()/8;
        m_pnjInfoWindow->addImageArea(pnj->getSprite(), false, buf, &rectSrcBuf);
        buf.x = TAILLEBLOCFENETRE/2;
        /*
        m_pokeInfoWindow->addTextArea("Type 1 : " + pnj->getType(1), 20, buf);
        buf.x += 3*TAILLEBLOCFENETRE;
        m_pokeInfoWindow->addTextArea("Type 2 : " + pnj->getType(1), 20, buf);
        buf.x -= 3*TAILLEBLOCFENETRE;
        buf.y = TAILLEBLOCFENETRE/2 + 20;*/
        m_pnjInfoWindow->addTextArea(pnj->getDescriptor()->getName(), 20, buf);
        buf.y = TAILLEBLOCFENETRE/2 + 20;
		m_pnjInfoWindow->addTextArea("Level : " + ska::StringUtils::intToStr(pnj->getStatistics()->getLevel()), 20, buf);
        buf.y = TAILLEBLOCFENETRE/2 + 40;
		m_pnjInfoWindow->addTextArea("HP : " + ska::StringUtils::intToStr(pnj->getHp()) + "/" + ska::StringUtils::intToStr(pnj->getStatistics()->getHpMax()), 20, buf);
        buf.y = TAILLEBLOCFENETRE/2 + 60;
		m_pnjInfoWindow->addTextArea("Position : " + ska::StringUtils::intToStr(posHero.x) + ":" + ska::StringUtils::intToStr(posHero.y), 20, buf);

        m_facesetOpponent->deleteAll();
        id = pnj->getID();
		if (id >= 0) {
			ss << "."FILE_SEPARATOR"Facesets"FILE_SEPARATOR"" << id << ".png";
		} else {
			ss << "."FILE_SEPARATOR"Facesets"FILE_SEPARATOR"" << -id << ".png";
		}

        buf.x = TAILLEBLOCFENETRE/2;
        buf.y = TAILLEBLOCFENETRE/2;
        m_facesetOpponent->addImageArea(ss.str(), false, buf, NULL);
    }
}

void GUI::update(ska::Observable<const int>* obs, const ska::EventArg& e, const int& i) {
}

void GUI::dialogDisplay()
{
	WGameCore& wScreen = WGameCore::getInstance();
	m_hide = false;

    if(m_dial->isVisible()) {
        m_hide = true;
        m_dial->display();
    }
	

    if(m_imgDial->isVisible()) {
        m_hide = true;
        m_imgDial->display();
    }


	if (m_pnjInfoWindow->isVisible()) {
		m_pnjInfoWindow->display();
	}


    if(m_attackPokemon->isVisible()) {
		vector<Skill_ptr>* v = /*wScreen.getFight().getPokemon()->getSkills()*/ NULL;
		
		ska::Rectangle cooldownPos;
        cooldownPos.x = m_attackPokemon->getPos().x + TAILLEBLOCFENETRE/4;
        cooldownPos.y = m_attackPokemon->getPos().y;

        //On blit les sprites d'attaque
        m_attackPokemon->display();

        //On blit les cooldowns par dessus
        for(unsigned int i = 0; i < v->size(); i++) {
            if(!(*v)[i]->cooldownOK()) {
				ska::Texture* cooldownText;
                cooldownText = (*v)[i]->getSpriteRemainingCD();
                cooldownPos.x = m_attackPokemon->getPos().x + TAILLEBLOC/2 + (i+1) * (*v)[i]->getIcon()->getWidth();
				cooldownText->render(cooldownPos.x, cooldownPos.y);
            }
        }

    }

	if (m_wSettings->isVisible()) {
		m_wSettings->display();
	}

	if (m_wBag->isVisible()) {
		m_wBag->display();
	}

	if (m_wTeam->isVisible()) {
		m_wTeam->display();
	}

    if(m_clickMenu->isVisible()) {
		const ska::InputRange& mousePos = wScreen.getRanges()[ska::InputRangeType::MousePos];
		if (ska::RectangleUtils::isPositionInBox(mousePos, m_clickMenu->getRect())) {
			m_clickMenu->display();
		} else {
            m_clickMenu->setPos(ska::Point<int>());
            m_clickMenu->hide(true);
        }

    }

	if (m_toolBar->isVisible()) {
		m_toolBar->display();
	}

	if (m_wShop->isVisible()) {
		m_wShop->display();
	}

}


void GUI::dialogRefresh()
{
	WGameCore& wScreen = WGameCore::getInstance();

	if(m_dial->isVisible()) {
		m_hide = true;
		m_dial->refresh();
	}


	if(m_imgDial->isVisible()) {
		m_hide = true;
		m_imgDial->refresh();
	}


	if (m_pnjInfoWindow->isVisible()) {
		m_pnjInfoWindow->refresh();
	}


	if(m_attackPokemon->isVisible()) {
		vector<Skill_ptr>* v = /*wScreen.getFight().getPokemon()->getSkills()*/ NULL;

		ska::Rectangle cooldownPos;
		cooldownPos.x = m_attackPokemon->getPos().x + TAILLEBLOCFENETRE/4;
		cooldownPos.y = m_attackPokemon->getPos().y;

		//On blit les sprites d'attaque
		m_attackPokemon->refresh();

		//On blit les cooldowns par dessus
		for(unsigned int i = 0; i < v->size(); i++) {
			if(!(*v)[i]->cooldownOK()) {
				ska::Texture* cooldownText;
				cooldownText = (*v)[i]->getSpriteRemainingCD();
				cooldownPos.x = m_attackPokemon->getPos().x + TAILLEBLOC/2 + (i+1) * (*v)[i]->getIcon()->getWidth();
				cooldownText->render(cooldownPos.x, cooldownPos.y);

			}
		}

	}



	if (m_wSettings->isVisible()) {
		m_wSettings->refresh();
	}

	if (m_wBag->isVisible()) {
		m_wBag->refresh();
	}

	if (m_wTeam->isVisible()) {
		m_wTeam->refresh();
	}

	if(m_clickMenu->isVisible()) {
		const ska::InputRange& mousePos = wScreen.getRanges()[ska::InputRangeType::MousePos];
		
		if (ska::RectangleUtils::isPositionInBox(mousePos, m_clickMenu->getRect())) {
			m_clickMenu->refresh();
		} else {
			m_clickMenu->setPos(ska::Point<int>());
			m_clickMenu->hide(true);
		}

	}


	if (m_toolBar->isVisible()) {
		m_toolBar->refresh();
	}

	if (m_wShop->isVisible()) {
		m_wShop->refresh();
	}

}


DialogMenuPtr& GUI::getDialog() {
    return m_dial;
}

DialogMenuPtr& GUI::getImgDialog() {
    return m_imgDial;
}

DialogMenuPtr& GUI::getInfoPNJWindow() {
    return m_pnjInfoWindow;
}

DialogMenuPtr& GUI::getInfoPokemonWindow() {
    return m_pokeInfoWindow;
}

//GUI::isPositionOnButton renvoie l'indice du bouton où se situe "pos" dans m_buttonList
//Renvoie -1 si pos n'est dans aucun bouton
int GUI::isPositionOnButton(const ska::Point<float>& pos)
{
	for (unsigned int i = 0; i < m_buttonList.size(); i++) {
		if (ska::RectangleUtils::isPositionInBox(pos, m_buttonList[i]->getRect())) {
			return i;
		}
	}
    return -1;
}

void GUI::hide(bool x)
{
    m_hide = x;
}

bool GUI::isVisible() const {
    return !m_hide;
}

DialogMenuPtr& GUI::getButton(unsigned int id) {
    return m_buttonList[id];
}

bool GUI::isMovingAWindow()
{
    return ((m_wSettings->isVisible() && m_wSettings->isMoving()) || (m_wBag->isVisible() && m_wBag->isMoving()) || (m_wTeam->isVisible() && m_wTeam->isMoving()) || ( m_toolBar->isVisible() && m_toolBar->isMoving()) );
}

bool GUI::isMouseOnAWindow()
{
	WGameCore& wScreen = WGameCore::getInstance();
	const ska::InputRange& mousePos = wScreen.getRanges()[ska::InputRangeType::MousePos];

	return ((m_wBag->isVisible() && ska::RectangleUtils::isPositionInBox(mousePos, m_wBag->getRect())) 
		|| (m_wSettings->isVisible() && ska::RectangleUtils::isPositionInBox(mousePos, m_wSettings->getRect())) 
		|| (m_wTeam->isVisible() && ska::RectangleUtils::isPositionInBox(mousePos, m_wTeam->getRect()))
		|| (m_toolBar->isVisible() && ska::RectangleUtils::isPositionInBox(mousePos, m_toolBar->getRect())));
}

int GUI::getRefreshPNJWindowCount()
{
    return m_refreshCount;
}

void GUI::setRefreshPNJWindowCount(int x)
{
    m_refreshCount = x;
}
