#include <memory>

#include "GUIMap.h"

#include "WindowTeam.h"
#include "WindowSettings.h"

GUIMap::GUIMap(ska::Window& w, ska::InputContextManager& playerICM) : ska::GUI(w, playerICM) {

	auto rawWindowTeam = new WindowTeam(m_wMaster, ska::Point<int>(4 * TAILLEBLOCFENETRE, 4 * TAILLEBLOCFENETRE));
	rawWindowTeam->show(false);
	rawWindowTeam->addPokemon(25);

	auto windowTeam = std::unique_ptr<WindowTeam>(rawWindowTeam);
	addWindow<WindowTeam> (windowTeam, "team");


	auto rawWindowSettings = new WindowSettings(*this, m_wMaster, ska::Point<int>(4 * TAILLEBLOCFENETRE, 4 * TAILLEBLOCFENETRE));
	rawWindowSettings->show(false);
	//rawWindowSettings->addPokemon(25);

	auto windowSettings = std::unique_ptr<WindowSettings>(rawWindowSettings);
	addWindow<WindowSettings, ska::KeyEventListener>(windowSettings, "settings");
}
