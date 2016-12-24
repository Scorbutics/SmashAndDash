#pragma once

#include "../../ska/Graphic/GUI/DynamicWindowIG.h"
#include "../../ska/Graphic/GUI/Components/ValueChangedEventListener.h"
#include "../../ska/Graphic/GUI/Components/Image.h"
#include "SlotPokemonData.h"

class WindowMouseCursor : 
	public ska::DynamicWindowIG<ska::ValueChangedEventListener<int>> {
public:
	WindowMouseCursor(ska::MouseObservable* guiObservable, ska::KeyObservable* keyObservable, const ska::Rectangle& box, const std::string& styleName);

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