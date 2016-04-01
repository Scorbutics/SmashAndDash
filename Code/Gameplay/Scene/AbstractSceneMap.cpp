#include "../WGameCore.h"
#include "../Weather.h"
#include "../../ska/World/World.h"
#include "../../ska/World/LayerE.h"
#include "AbstractSceneMap.h"

AbstractSceneMap::AbstractSceneMap(ska::SceneHolder& sh, ska::InputContextManager& ril) : AbstractNoGUISceneMap(sh, ril)
{
}

AbstractSceneMap::AbstractSceneMap(ska::Scene& oldScene) : 
AbstractNoGUISceneMap(oldScene) {

}

void AbstractSceneMap::load() {

}

void AbstractSceneMap::unload() {

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

	AbstractNoGUISceneMap::eventUpdate(stuck);

	//GUI
	gui.dialogRefresh();
	gui.refresh();

}

AbstractSceneMap::~AbstractSceneMap()
{
}
