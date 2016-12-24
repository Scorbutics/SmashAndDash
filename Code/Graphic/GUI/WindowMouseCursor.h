#pragma once

#include "../../ska/Graphic/GUI/DynamicWindowIG.h"
#include "../../ska/Graphic/GUI/Components/ValueChangedEventListener.h"
#include "../../ska/Graphic/GUI/Components/Image.h"

class WindowMouseCursor : 
	public ska::DynamicWindowIG<ska::ValueChangedEventListener<int>> {
public:
	WindowMouseCursor(ska::MouseObservable* guiObservable, ska::KeyObservable* keyObservable, const ska::Rectangle& box, const std::string& styleName);

	unsigned int loadPokemon(unsigned int id);
	unsigned int loadItem(unsigned int id);
	void unloadPokemon();
	void unloadItem();

	virtual ~WindowMouseCursor() = default;

private:
	unsigned int m_lastPokemonId;
	unsigned int m_lastItemId;
	ska::Image* m_pokemon;
	ska::Image* m_item;
	const ska::Rectangle m_originalBox;
};