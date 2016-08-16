#pragma once

#include <sstream>
#include <functional>
#include <vector>
#include "../../ska/Graphic/Texture.h"
#include "DynamicWindowArea.h"
#include "ButtonState.h"

class WGameCore;

namespace ska {
	class InputContextManager;
}

class SpriteButton : public DynamicWindowArea {
public:
	SpriteButton(DialogMenu& parent, const ska::InputContextManager& playerICM, ska::Point<int> relativePos, const std::string& placeHolderStyleName, unsigned int spriteId, std::function<void(void)> const& callback);
    virtual void refresh() override;
	virtual void display() const override;
    
	virtual ~SpriteButton();

private:
	ButtonState::Enum m_state;
	const ska::InputContextManager& m_playerICM;
	ska::Texture m_skill;
	ska::Texture m_skillPlaceHolder;
	ska::Texture m_skillPlaceHolderHover;
	ska::Texture m_skillPlaceHolderPressed;
	
	std::function<void(void)> m_callback;

	ska::Rectangle m_placeHolderRect;
	ska::Rectangle m_skillRect;

};

