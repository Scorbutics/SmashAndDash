#include <memory>

#include "GUIMap.h"

#include "WindowTeam.h"


GUIMap::GUIMap(ska::Window& w, ska::InputContextManager& playerICM) : ska::GUI(w, playerICM) {

	auto& windowTeam = std::unique_ptr<ska::WindowIG>(new WindowTeam(*this, ska::Point<int>(2 * TAILLEBLOCFENETRE, 2 * TAILLEBLOCFENETRE)));
	addWindow(std::move(windowTeam));
}
