#include "WindowTeam.h"
#include "../../Gameplay\WGameCore.h"
#include "../../Gameplay\PokemonManager.h"
#include "Inventory_Area.h"
#include "../../Utils\ChargementImages.h"
#include "../../Utils/IDs.h"
#include "../../ska/Utils/RectangleUtils.h"

using namespace std;

WindowTeam::WindowTeam(string fichierMenu, ska::Rectangle posFond, int taillePolice) :MovableWindow(fichierMenu, posFond, taillePolice)
{
    m_boolUseObjectSelectPkmn.push_back(0);
    m_boolUseObjectSelectPkmn.push_back(0);
    m_boolUseObjectSelectPkmn.push_back(0);
    m_boolUseObjectSelectPkmn.push_back(0);
    m_boolUseObjectSelectPkmn.push_back(0);
    m_boolUseObjectSelectPkmn.push_back(0);
    m_indexCursor = 0;
}

void WindowTeam::reset(string action)
{
    this->deleteAll();
	ska::Rectangle buf;
	WGameCore& wScreen = WGameCore::getInstance();
	PokemonManager& pkmnMng = wScreen.getPokemonManager();

    vector<int> vBool;
    vBool.push_back(0);
    vBool.push_back(1);
    buf.x = 3*TAILLEBLOCFENETRE;
    buf.y = TAILLEBLOCFENETRE*3/4;
    buf.h = TAILLEBLOCFENETRE*3;
    this->addTextArea("Equipe Pokémon", 20, buf);
    this->resize(this->getWidth(), (int)pkmnMng.getPokemonTeamSize()*3*TAILLEBLOCFENETRE + 2*TAILLEBLOCFENETRE);
    buf.x = 9*TAILLEBLOCFENETRE;
    this->addButtonClose("."FILE_SEPARATOR"Menu"FILE_SEPARATOR"close_button.png", "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"close_button_active.png", buf);
    buf.x = TAILLEBLOCFENETRE/3;
    buf.y = TAILLEBLOCFENETRE*5/2;
    m_slotPkmn.clear();
    for(unsigned int i = 0; i < pkmnMng.getPokemonTeamSize(); i++)
    {
        if(pkmnMng.getPokemon(i) != NULL )
        {
			ska::Rectangle /*rectSrcBuf,*/ posSlot;
            /*rectSrcBuf.x = 0;
            rectSrcBuf.y = pkmnMng->getPokemon(i)->getHeight();
            rectSrcBuf.w = pkmnMng->getPokemon(i)->getWidth();
            rectSrcBuf.h = pkmnMng->getPokemon(i)->getHeight();*/
			posSlot.x = 6;
            posSlot.y = buf.y - TAILLEBLOCFENETRE;
			posSlot.w = 0;
			posSlot.h = 0;

            m_slotPkmn.push_back(unique_ptr<SlotPokemon_Area>(new SlotPokemon_Area(this, posSlot, "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"button_slot_pokemon.png", "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"button_slot_pokemon.png", action, &m_boolUseObjectSelectPkmn[i], i)));
            (m_slotPkmn[i])->setPokemon(i);
            buf.y += buf.h;

        }
    }
}



void WindowTeam::refresh()
{
    MovableWindow::refresh();
	WGameCore& wScreen = WGameCore::getInstance();
	const ska::InputActionContainer& in = wScreen.getActions();
	const ska::InputRange& mouseClickPos = wScreen.getRanges()[ska::InputRangeType::MousePos];
	PokemonManager& pkmnMng = wScreen.getPokemonManager();
	MouseCursor& mouseCur = wScreen.getMouseCursor();

	for(unsigned int i = 0; i < m_slotPkmn.size(); i++)
	{
		m_slotPkmn[i]->display();
		m_slotPkmn[i]->refresh();
			
	}
	

    for(unsigned int i = 0; i < m_slotPkmn.size(); i++)
    {
 
		ska::Rectangle rectSlot = m_slotPkmn[i]->getRectSize();
		if (pkmnMng.getPokemon(i) != wScreen.getFight().getPokemon() && pkmnMng.getPokemonTeamSize() > 1 && mouseCur.getPokemon() == NULL && in[ska::InputAction::LClic] && ska::RectangleUtils::isPositionInBox(mouseClickPos, rectSlot)) //Si on clique dans la fenetre d'un slot
        {
            mouseCur.setPokemon(pkmnMng.getPokemon(i));
			pkmnMng.remove(i);
            this->reset("show");
            m_indexCursor = i;
            break;
        }
		else if (in[ska::InputAction::LClic] && mouseCur.getPokemon() != NULL && ska::RectangleUtils::isPositionInBox(mouseClickPos, rectSlot))
        {
            pkmnMng.add(mouseCur.getPokemon());
			mouseCur.removePokemon();
            pkmnMng.swapPokemonOrders(i, m_indexCursor);
            this->reset("show");
            break;
        }

    }

    for(unsigned int i = 0; i < m_boolUseObjectSelectPkmn.size(); i++)
        if(m_boolUseObjectSelectPkmn[i] != 0)
        {
            this->hide(true);
            wScreen.getGUI().getWindowBag()->hide(false);
            Inventory_Area* invArea = wScreen.getGUI().getWindowBag()->getInventoryArea(0);
            invArea->useObjectAtPos(wScreen.getGUI().getWindowBag()->getCurObjectPos(), i);
            m_boolUseObjectSelectPkmn[i] = 0;
        }

}

WindowTeam::~WindowTeam()
{
}

