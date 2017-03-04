#pragma once
#include "../../ska/Graphic/GUI/WindowIG.h"
#include "../../ska/Graphic/Texture.h"
#include "../../ska/Graphic/GUI/Components/Label.h"
#include "../../ska/Graphic/GUI/Components/ButtonSprite.h"

#include "../../ska/Graphic/GUI/Components/HoverEventListener.h"
#include "../../ska/Graphic/GUI/Components/ClickEventListener.h"
#include "../../ska/Graphic/GUI/Components/ValueChangedEventListener.h"

#include "SlotPokemonData.h"

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
