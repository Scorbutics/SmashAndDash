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

class SlotPokemon : public ska::WindowIG<ska::ValueChangedEventListener<SlotPokemonData>, ska::HoverEventListener, ska::ClickEventListener> {
public:
	SlotPokemon(ska::Widget& parent, ska::Point<int> relativePos, unsigned int slot, unsigned int pokemonId);
    //void display() const override;
	bool onStatisticsChangeEvent(StatisticsChangeEvent& sce);
	void load(SlotPokemonData& spd);
    virtual ~SlotPokemon() = default;
	
private:
    //int m_key, m_index;
    //int* m_boolUseObjectSelectPkmn;
	//ska::Texture m_image, m_imagePressed;
	//std::string m_action, m_style, m_stylePressed;;
	const unsigned int m_fontSize;
	SlotPokemonData m_data;
	ska::ButtonSprite* m_sprite;
	//std::unique_ptr<Button> m_buttonArea;

};
