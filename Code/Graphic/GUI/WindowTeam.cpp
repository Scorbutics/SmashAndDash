#include "WindowTeam.h"
#include "SlotPokemon.h"

WindowTeam::WindowTeam(ska::MouseObservable& guiObservable, const ska::Point<int>& absolutePos) :
	ska::MoveableWindow(guiObservable, ska::Rectangle{ absolutePos.x, absolutePos.y, 7 * TAILLEBLOCFENETRE, 7 * TAILLEBLOCFENETRE }, true) {


}

void WindowTeam::addPokemon(unsigned int pokemonId) {
	auto& slotPokemon = std::unique_ptr<SlotPokemon>(new SlotPokemon(*this, ska::Point<int>(10, 12 + m_widgets.size() * 2 * TAILLEBLOCFENETRE), pokemonId));
	addWidget(std::move(slotPokemon));
}

