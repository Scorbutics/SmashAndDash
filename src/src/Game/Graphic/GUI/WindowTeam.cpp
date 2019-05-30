#include <algorithm>
#include "WindowTeam.h"
#include "SlotPokemon.h"
#include "SlotPokemonData.h"
#include "WindowMouseCursor.h"
#include "Rectangle.h"
#include "GUI/Components/Concrete/TileSurface.h"

WindowTeam::WindowTeam(Widget& parent, WindowMouseCursor* mouseCursor, const ska::Point<int>& absolutePos) :
MoveableWindow<ska::ValueChangedEventListener<SlotPokemonDataPtr*>>(parent, ska::Rectangle{ absolutePos.x, absolutePos.y, 11 * 32, 15 * 32 }),
m_mouseCursor(mouseCursor) {

	setTitle("Equipe Pokémon");
	auto& tileSurface = setBackground<ska::TileSurface>(ska::Rectangle{ 0, 0, 1, 1 }, ska::GUI::MENU_DEFAULT_THEME_PATH + "menu.png");
	tileSurface.setOpacity(210);

	auto count = -1;
	m_slots.push_back(&addWidget<SlotPokemon>(ska::Point<int>(14, 20 + count * 2 * 32)));
	m_slots.push_back(&addWidget<SlotPokemon>(ska::Point<int>(14, 20 + count * 2 * 32)));
	m_slots.push_back(&addWidget<SlotPokemon>(ska::Point<int>(14, 20 + count * 2 * 32)));
	m_slots.push_back(&addWidget<SlotPokemon>(ska::Point<int>(14, 20 + count * 2 * 32)));
	m_slots.push_back(&addWidget<SlotPokemon>(ska::Point<int>(14, 20 + count * 2 * 32)));
	m_slots.push_back(&addWidget<SlotPokemon>(ska::Point<int>(14, 20 + count * 2 * 32)));

	for(auto& s : m_slots) {
		s->show(false);
		s->addHandler<ska::ValueChangedEventListener<SlotPokemonDataPtr*>>([this](Widget*, ska::ValueChangedEvent<SlotPokemonDataPtr*>& vce) {
			directNotify(vce);
		});
	}

	addHandler<ska::ValueChangedEventListener<SlotPokemonDataPtr*>>([this](Widget*, ska::ValueChangedEvent<SlotPokemonDataPtr*>& vce) {
		if (m_mouseCursor->isLoaded()) {
			insertPokemon((*vce.getValue())->parent, std::move(m_mouseCursor->unloadPokemon()));
		} else if(m_visibleSlots.size() > 1) {
			auto parent = (*vce.getValue())->parent;
			m_mouseCursor->loadPokemon(*vce.getValue());
			unloadPokemon(parent);
			m_mouseCursor->show(true);
		}
	});
}

void WindowTeam::organizeSlots() {
	unsigned int count = 0;
	for (auto s : m_visibleSlots) {
		s->move(ska::Point<int>(14, 20 + (count++)* 2 * 32));
	}
}

SlotPokemon* WindowTeam::insertPokemon(SlotPokemon*, SlotPokemonDataPtr spd) {
	SlotPokemon* place = nullptr;
	for(auto s : m_slots) {
		if(!s->isLoaded()) {
			place = s;
			break;
		}
	}

	if (place != nullptr) {
		place->show(true);
		spd->parent = place;
		place->load(spd);
		m_visibleSlots.push_front(place);
		organizeSlots();
	}
	return place;
}

void WindowTeam::unloadPokemon(SlotPokemon* slot) {
	for (auto sIt = m_visibleSlots.begin(); sIt != m_visibleSlots.end(); ) {
		if(slot == *sIt) {
			sIt = m_visibleSlots.erase(sIt);
		} else {
			++sIt;
		}
	}
	slot->unload();
	organizeSlots();
}

