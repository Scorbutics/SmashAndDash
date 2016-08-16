#include "SpriteButton.h"
#include "../../Utils\ChargementImages.h"
#include "../../ska/Utils/RectangleUtils.h"
#include "../../ska/Graphic/SpritePath.h"
#include "DialogMenu.h"
#include "../../ska/Inputs/InputContextManager.h"

SpriteButton::SpriteButton(DialogMenu& parent, const ska::InputContextManager& playerICM, ska::Point<int> relativePos, const std::string& placeHolderStyleName, unsigned int spriteId, std::function<void(void)> const& callback) :
DynamicWindowArea(parent),
m_skillPlaceHolder(placeHolderStyleName + ".png"),
m_skillPlaceHolderHover(placeHolderStyleName + "_hover.png"),
m_skillPlaceHolderPressed(placeHolderStyleName + "_pressed.png"),
m_skill(ska::SpritePath::getInstance().getPath(SPRITEBANK_ICONS, spriteId)),
m_playerICM(playerICM),
m_state(ButtonState::NONE),
m_callback(callback) {
	m_placeHolderRect = ska::Rectangle{ relativePos.x, relativePos.y, m_skillPlaceHolder.getWidth(), m_skillPlaceHolder.getHeight() };

	m_skillRect = ska::Rectangle{ 0, 0, m_skill.getWidth(), m_skill.getHeight() };
	m_skillRect = ska::RectangleUtils::posToCenterPicture(m_skillRect, m_placeHolderRect);
}

void SpriteButton::display() const {
	if (!m_parent.isVisible()) {
		return;
	}

	const ska::Texture* selectorPlaceHolder;
	switch (m_state) {
	case ButtonState::HOVER:
		selectorPlaceHolder = &m_skillPlaceHolderHover;
		break;
	case ButtonState::PRESSED:
		selectorPlaceHolder = &m_skillPlaceHolderPressed;
		break;
	default:
		selectorPlaceHolder = &m_skillPlaceHolder;
		break;
	}

	selectorPlaceHolder->render(m_placeHolderRect.x, m_placeHolderRect.y);
	m_skill.render(m_skillRect.x, m_skillRect.y);

}

void SpriteButton::refresh() {
	if (!m_parent.isVisible()) {
		return;
	}

	const ska::InputRange& mousePos = m_playerICM.getRanges()[ska::InputRangeType::MousePos];
	const ska::InputActionContainer& in = m_playerICM.getActions();
	
	ska::Rectangle buf;
	buf.x = m_relativePos.x + m_parent.getRect().x;
	buf.y = m_relativePos.y + m_parent.getRect().y;

	bool click = in[ska::InputAction::LClic];
	if (ska::RectangleUtils::isPositionInBox(mousePos, buf)) {
		if (click) {
			m_state = ButtonState::PRESSED;
			m_callback();
		} else {
			m_state = ButtonState::HOVER;
		}
	} else {
		m_state = ButtonState::NONE;
	}
}


SpriteButton::~SpriteButton() {
}
