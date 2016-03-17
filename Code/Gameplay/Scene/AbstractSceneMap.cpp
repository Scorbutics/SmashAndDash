#include "../WGameCore.h"
#include "../Weather.h"
#include "../../ska/World/World.h"
#include "../../Graphic/Scrolling.h"
#include "../../ska/Script/ScriptDispatcher.h"
#include "../../ska/World/LayerE.h"
#include "AbstractSceneMap.h"

AbstractSceneMap::AbstractSceneMap(ska::EntityManager& em, ska::RawInputListener& ril) : AbstractNoGUISceneMap(em, ril)
{
}

void AbstractSceneMap::graphicUpdate(ska::DrawableContainer& drawables) {
	WGameCore& core = WGameCore::getInstance();
	ska::World& world = core.getWorld();
	GUI& gui = core.getGUI();
	

	AbstractNoGUISceneMap::graphicUpdate(drawables);

	//Affiche la GUI
	drawables.addHead(gui);

	//Affiche le Pokémon ou l'objet sur le curseur de la souris
	//mouseCursor.displaySelectedPokemon();
	//mouseCursor.displaySelectedObject();

}

void AbstractSceneMap::eventUpdate(bool stuck) {
	WGameCore& core = WGameCore::getInstance();
	GUI& gui = core.getGUI();
	//ska::InputListener& kdListener = core.getInputListener();
	
	AbstractNoGUISceneMap::eventUpdate(stuck);

	//Gère les évènements extérieurs de la GUI (input)
	//GUI A PASSER EN TANT QU'OBSERVATEUR
	/*if (!kdListener.refresh(stuck)) {
		gui.getDialog()->hide(true);
		gui.getImgDialog()->hide(true);
		gui.hide(false);
	}*/

	//GUI
	gui.dialogRefresh();
	gui.refresh();

}

AbstractSceneMap::~AbstractSceneMap()
{
}
