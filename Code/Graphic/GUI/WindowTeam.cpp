#include "WindowTeam.h"
#include "SlotPokemon.h"

WindowTeam::WindowTeam(ska::MouseObservable& guiObservable, ska::KeyObservable& keyboardObs, const ska::Point<int>& absolutePos) :
ska::MoveableWindow<>(guiObservable, keyboardObs, ska::Rectangle{ absolutePos.x, absolutePos.y, 7 * TAILLEBLOCFENETRE, 7 * TAILLEBLOCFENETRE }, true),
	m_pokemonCount(0) {


}

void WindowTeam::addPokemon(unsigned int pokemonId) {
	auto slotPokemon = std::unique_ptr<SlotPokemon>(new SlotPokemon(*this, ska::Point<int>(14, 20 + (m_pokemonCount) * 2 * TAILLEBLOCFENETRE), pokemonId));
	addWidget(slotPokemon);
	m_pokemonCount++;
}

