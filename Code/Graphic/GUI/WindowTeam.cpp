#include "WindowTeam.h"
#include "SlotPokemon.h"

WindowTeam::WindowTeam(ska::MouseObservable& guiObservable, const ska::Point<int>& absolutePos) :
	ska::WindowIG(guiObservable, ska::Rectangle{ absolutePos.x, absolutePos.y, 5*TAILLEBLOCFENETRE, 5*TAILLEBLOCFENETRE }, true) {


}

void WindowTeam::addPokemon(unsigned int pokemonId) {
	auto& slotPokemon = std::unique_ptr<SlotPokemon>(new SlotPokemon(*this, ska::Point<int>(10, 10 + m_widgets.size() * 2 * TAILLEBLOCFENETRE), pokemonId));
	addWidget(std::move(slotPokemon));
}

