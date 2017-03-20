#include <limits>
#include "../../ska/Graphic/SpritePath.h"
#include "WindowMouseCursor.h"

WindowMouseCursor::WindowMouseCursor(ska::TimeObservable* timeObservable, ska::MouseObservable* guiObservable, ska::KeyObservable* keyObservable, const ska::Rectangle& box, const std::string& styleName) :
TimeScrollableWindowIG<ska::ValueChangedEventListener<int>>(timeObservable, guiObservable, keyObservable, box, styleName),
m_originalBox(box) {
	m_pokemon = addWidget(std::make_unique<ska::Image>(*this, "", ska::Point<int>(), false, nullptr));
	m_item = addWidget(std::make_unique<ska::Image>(*this, "", ska::Point<int>(m_pokemon->getBox().w, 0), false, nullptr));
	m_pokemon->show(false);
	m_item->show(false);
	Widget::setWidth(box.w);
	Widget::setHeight(box.h);
	show(false);
	DrawableFixedPriority::setPriority(std::numeric_limits<int>::max());
}

void WindowMouseCursor::loadPokemon(SlotPokemonDataPtr& spd) {
	m_pokemonData = std::move(spd);
	m_pokemonData->parent = nullptr;
	m_pokemon->replaceWith(ska::SpritePath::getInstance().getPath(SPRITEBANK_CHARSET, m_pokemonData->id), 6, 8, 1, 2);
	m_pokemon->show(true);
	setWidth(m_originalBox.w * ((m_item->isVisible() ? 1 : 0) + (m_pokemon->isVisible() ? 1 : 0)));
}

void WindowMouseCursor::loadItem(unsigned int id) {
	//auto result = m_lastItemId;
	m_item->replaceWith(ska::SpritePath::getInstance().getPath(SPRITEBANK_INVENTORY, id), 2, 1, 1, 2);
	m_item->show(true);
	m_lastItemId = id;
	setWidth(m_originalBox.w * ((m_item->isVisible() ? 1 : 0) + (m_pokemon->isVisible() ? 1 : 0)));
}

SlotPokemonDataPtr WindowMouseCursor::unloadPokemon() {
	m_pokemon->show(false);
	if(!m_item->isVisible()) {
		show(false);
	}
	return std::move(m_pokemonData);
}

void WindowMouseCursor::unloadItem() {
	m_item->show(false);
	if (!m_pokemon->isVisible()) {
		show(false);
	}
}

bool WindowMouseCursor::isLoaded() const {
	return m_pokemonData != nullptr;
}

