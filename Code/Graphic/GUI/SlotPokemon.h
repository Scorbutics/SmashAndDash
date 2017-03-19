#pragma once


#include "SlotPokemonData.h"
#include "../../ska/Graphic/GUI/Components/Concrete/ButtonSprite.h"
#include "../../ska/Graphic/GUI/Components/Concrete/Label.h"
#include "../../ska/Graphic/GUI/Windows/WindowIG.h"

class StatisticsChangeEvent;

class SlotPokemon : public ska::WindowIG<ska::ValueChangedEventListener<SlotPokemonDataPtr*>, ska::HoverEventListener, ska::ClickEventListener> {
public:
	SlotPokemon(Widget& parent, ska::Point<int> relativePos);
	bool onStatisticsChangeEvent(StatisticsChangeEvent& sce);
	void load(SlotPokemonDataPtr& spd);
	void unload();
	bool isLoaded() const;

    virtual ~SlotPokemon() = default;
	
private:
	const unsigned int m_fontSize;
	SlotPokemonDataPtr m_data;
	
	ska::ButtonSprite* m_sprite;
	ska::Label* m_name;
	ska::Label* m_level;
	ska::Label* m_hp;
	ska::Label* m_type1;
	ska::Label* m_type2;

};
