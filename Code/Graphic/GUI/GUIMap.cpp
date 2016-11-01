#include <memory>

#include "GUIMap.h"

#include "WindowTeam.h"


GUIMap::GUIMap(ska::Window& w, ska::InputContextManager& playerICM) : ska::GUI(w, playerICM) {

	auto rawWindowTeam = new WindowTeam(*this, ska::Point<int>(4 * TAILLEBLOCFENETRE, 4 * TAILLEBLOCFENETRE));
	rawWindowTeam->show(false);
	rawWindowTeam->addPokemon(25);

	auto& windowTeam = std::unique_ptr<ska::WindowIG>(rawWindowTeam);
	addWindow(std::move(windowTeam));
}
