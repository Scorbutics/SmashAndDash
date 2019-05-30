#include <limits>
#include "Game/__internalConfig/LoggerConfig.h"
#include "Core/Utils/SpritePath.h"
#include "WindowMouseCursor.h"
#include "Core/Utils/SkaConstants.h"

WindowMouseCursor::WindowMouseCursor(ska::TimeObservable* timeObservable, ska::MouseObservable* guiObservable, ska::KeyObservable* keyObservable, const ska::Rectangle& box) :
	TimeScrollableWindowIG<ska::ValueChangedEventListener<int>>(timeObservable, guiObservable, keyObservable, box),
	m_originalBox(box) {
	m_pokemon = &addWidget<ska::Image>("", ska::Point<int>(), false, nullptr);
	m_item = &addWidget<ska::Image>("", ska::Point<int>(m_pokemon->getBox().w, 0), false, nullptr);
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
	auto charset = ska::Texture{ ska::SpritePath::getInstance().getPath(SPRITEBANK_CHARSET, m_pokemonData->id) };
	static constexpr auto BLOCKSIZE = 72;
	auto clip = ska::Rectangle{ 1 * BLOCKSIZE, 7 * BLOCKSIZE, BLOCKSIZE, BLOCKSIZE };
	m_pokemon->replaceWith(std::move(charset), &clip);
	m_pokemon->show(true);
	setWidth(m_originalBox.w * ((m_item->isVisible() ? 1 : 0) + 1));
}

void WindowMouseCursor::loadItem(unsigned int id) {
	//auto result = m_lastItemId;
	auto itemTexture = ska::Texture{ ska::SpritePath::getInstance().getPath(SPRITEBANK_INVENTORY, id) };
	m_item->replaceWith(std::move(itemTexture), nullptr);
	m_item->show(true);
	m_lastItemId = id;
	setWidth(m_originalBox.w * (1 + (m_pokemon->isVisible() ? 1 : 0)));
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

