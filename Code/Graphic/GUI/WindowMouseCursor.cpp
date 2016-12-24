#include <limits>
#include "../../ska/Graphic/SpritePath.h"
#include "WindowMouseCursor.h"

WindowMouseCursor::WindowMouseCursor(ska::MouseObservable* guiObservable, ska::KeyObservable* keyObservable, const ska::Rectangle& box, const std::string& styleName) :
ska::DynamicWindowIG<ska::ValueChangedEventListener<int>>(guiObservable, keyObservable, box, styleName),
m_originalBox(box),
m_lastPokemonId(std::numeric_limits<unsigned int>::min()) {
	m_pokemon = addWidget(std::make_unique<ska::Image>(*this, "", ska::Point<int>(), false, nullptr));
	m_item = addWidget(std::make_unique<ska::Image>(*this, "", ska::Point<int>(m_pokemon->getBox().w, 0), false, nullptr));
	m_pokemon->show(false);
	m_item->show(false);
	setWidth(box.w);
	setHeight(box.h);
	show(false);
	setPriority(std::numeric_limits<int>::max());
}

unsigned int WindowMouseCursor::loadPokemon(unsigned int id) {
	auto result = m_lastPokemonId;
	m_pokemon->replaceWith(ska::SpritePath::getInstance().getPath(SPRITEBANK_CHARSET, id), 6, 8, 1, 2);
	m_pokemon->show(true);
	m_lastPokemonId = id;
	setWidth(m_originalBox.w * ((m_item->isVisible() ? 1 : 0) + (m_pokemon->isVisible() ? 1 : 0)));
	return result;
}

unsigned int WindowMouseCursor::loadItem(unsigned int id) {
	auto result = m_lastItemId;
	m_item->replaceWith(ska::SpritePath::getInstance().getPath(SPRITEBANK_INVENTORY, id), 2, 1, 1, 2);
	m_item->show(true);
	m_lastItemId = id;
	setWidth(m_originalBox.w * ((m_item->isVisible() ? 1 : 0) + (m_pokemon->isVisible() ? 1 : 0)));
	return result;
}

void WindowMouseCursor::unloadPokemon() {
	m_pokemon->show(false);
	if(!m_item->isVisible()) {
		show(false);
	}
}

void WindowMouseCursor::unloadItem() {
	m_item->show(false);
	if (!m_pokemon->isVisible()) {
		show(false);
	}
}
