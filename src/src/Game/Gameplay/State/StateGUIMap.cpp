#include "Game/__internalConfig/LoggerConfig.h"
#include "StateGUIMap.h"
#include "Core/Draw/DrawableContainer.h"

StateGUIMap::StateGUIMap(CustomEntityManager& em, PokemonGameEventDispatcher& ged, WorldState& worldState, Settings& settings) :
	m_dialogs(em, ged, worldState),
	m_menu(em, ged, worldState, settings),
	m_debug(ged, worldState) {
}

void StateGUIMap::onGraphicUpdate(unsigned int, ska::DrawableContainer& drawables) {
	m_dialogs.graphicUpdate(drawables);
	m_menu.graphicUpdate(drawables);
	m_debug.graphicUpdate(drawables);
}

void StateGUIMap::onEventUpdate(unsigned int ellapsedTime) {
	m_dialogs.refresh(ellapsedTime);
	m_menu.refresh(ellapsedTime);
	m_debug.refresh(ellapsedTime);
}
