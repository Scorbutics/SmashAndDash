#pragma once

#include "SlotPokemonData.h"
#include "GUI/Components/Concrete/Image.h"
#include "GUI/Events/ValueChangedEvent.h"
#include "GUI/Utils/TimeObservable.h"
#include "GUI/Windows/TimeScrollableWindowIG.h"

class WindowMouseCursor :
	public ska::TimeScrollableWindowIG<ska::ValueChangedEventListener<int>> {
public:
	WindowMouseCursor(ska::TimeObservable* timeObservable, ska::MouseObservable* guiObservable, ska::KeyObservable* keyObservable, const ska::Rectangle& box);
	WindowMouseCursor(const WindowMouseCursor&) = delete;
	WindowMouseCursor& operator=(const WindowMouseCursor&) = delete;
	void loadPokemon(SlotPokemonDataPtr& spd);
	void loadItem(unsigned int id);
	SlotPokemonDataPtr unloadPokemon();
	void unloadItem();
	bool isLoaded() const;

	virtual ~WindowMouseCursor() = default;

private:
	SlotPokemonDataPtr m_pokemonData;
	unsigned int m_lastItemId;
	ska::Image* m_pokemon;
	ska::Image* m_item;
	const ska::Rectangle m_originalBox;
};
