#pragma once
#include "../../ska/Graphic/GUI/WindowIG.h"
#include "../../ska/Graphic/Texture.h"
#include "../../ska/Graphic/GUI/Components/Label.h"
#include "../../ska/Graphic/GUI/Components/ButtonSprite.h"

#include "../../ska/Graphic/GUI/Components/HoverEventListener.h"

class SlotPokemon : public ska::WindowIG<ska::HoverEventListener> {
public:
	SlotPokemon(ska::Widget& parent, ska::Point<int> relativePos, unsigned int pokemonId);
    //void display() const override;
    virtual ~SlotPokemon() = default;
	
private:
    //int m_key, m_index;
    //int* m_boolUseObjectSelectPkmn;
	//ska::Texture m_image, m_imagePressed;
	//std::string m_action, m_style, m_stylePressed;;
	const unsigned int m_fontSize;
	std::unique_ptr<ska::ButtonSprite> m_spriteArea;
	std::unique_ptr<ska::Label> m_pvArea;
	std::unique_ptr<ska::Label> m_levelArea;
	std::unique_ptr<ska::Label> m_type1Area;
	std::unique_ptr<ska::Label> m_type2Area;
	std::unique_ptr<ska::Label> m_nameArea;
    //std::unique_ptr<Button> m_buttonArea;

};
