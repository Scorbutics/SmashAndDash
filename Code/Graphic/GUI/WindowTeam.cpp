#include "WindowTeam.h"
#include "SlotPokemon.h"
#include "SlotPokemonData.h"

WindowTeam::WindowTeam(ska::Widget& parent, const ska::Point<int>& absolutePos) :
ska::MoveableWindow<ska::ValueChangedEventListener<SlotPokemonData>>(parent, ska::Rectangle{ absolutePos.x, absolutePos.y, 11 * TAILLEBLOCFENETRE, 13 * TAILLEBLOCFENETRE }, ska::Button::MENU_DEFAULT_THEME_PATH + "menu"),
	m_pokemonCount(0) {
	
	int count = -1;
	m_slots.push_back(addWidget(std::unique_ptr<SlotPokemon>(new SlotPokemon(*this, ska::Point<int>(14, 20 + (count)* 2 * TAILLEBLOCFENETRE), count++, -1))));
	m_slots.push_back(addWidget(std::unique_ptr<SlotPokemon>(new SlotPokemon(*this, ska::Point<int>(14, 20 + (count)* 2 * TAILLEBLOCFENETRE), count++, -1))));
	m_slots.push_back(addWidget(std::unique_ptr<SlotPokemon>(new SlotPokemon(*this, ska::Point<int>(14, 20 + (count)* 2 * TAILLEBLOCFENETRE), count++, -1))));
	m_slots.push_back(addWidget(std::unique_ptr<SlotPokemon>(new SlotPokemon(*this, ska::Point<int>(14, 20 + (count)* 2 * TAILLEBLOCFENETRE), count++, -1))));
	m_slots.push_back(addWidget(std::unique_ptr<SlotPokemon>(new SlotPokemon(*this, ska::Point<int>(14, 20 + (count)* 2 * TAILLEBLOCFENETRE), count++, -1))));
	m_slots.push_back(addWidget(std::unique_ptr<SlotPokemon>(new SlotPokemon(*this, ska::Point<int>(14, 20 + (count)* 2 * TAILLEBLOCFENETRE), count++, -1))));

	for(auto& s : m_slots) {
		s->show(false);
		s->addHandler<ska::ValueChangedEventListener<SlotPokemonData>>([this](ska::Widget* tthis, ska::ValueChangedEvent<SlotPokemonData>& vce) {
			/* If we have more than 1 pokémon, we can swap each others */
			if (m_pokemonCount > 1) {
				directNotify(vce);
			}
		});
	}
}

void WindowTeam::loadPokemon(unsigned int slot, unsigned int pokemonId) {
	if(slot >= m_slots.size()) {
		return;
	}
	auto slotPokemon = m_slots[slot];
	SlotPokemonData spd;
	spd.slotNumber = slot;
	spd.id = pokemonId;
	slotPokemon->load(spd);
	slotPokemon->show(true);
	slotPokemon->showWidgets(true);
	m_pokemonCount++;
}

void WindowTeam::unloadPokemon(unsigned int slot) {
	m_slots[slot]->showWidgets(false);
	//m_pokemonCount--;
}

