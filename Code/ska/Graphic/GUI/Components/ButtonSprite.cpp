#include "ButtonSprite.h"
#include "../../../Graphic/SpritePath.h"
#include "../../../Utils/RectangleUtils.h"

ska::ButtonSprite::ButtonSprite(GUI& guiObservable, Widget& parent, ska::Point<int> relativePos, const std::string& placeHolderStyleName, unsigned int id, ClickEventHandler const& callback) :
Button(guiObservable, parent, relativePos, placeHolderStyleName, callback),
m_img(ska::SpritePath::getInstance().getPath(SPRITEBANK_ICONS, id)) {
}

void ska::ButtonSprite::display() const {
	ska::Button::display();
	m_img.render(m_rect.x, m_rect.y);
}

void ska::ButtonSprite::move(const ska::Point<int>& pos) {
	const auto& box = getBox();
	const auto& absPos = getAbsolutePosition();
	
	ska::Rectangle placeHolderRect = ska::Rectangle{ pos.x + absPos.x - box.x, pos.y + absPos.y - box.y, box.w, box.h };

	m_rect = ska::Rectangle{ 0, 0, m_img.getWidth(), m_img.getHeight() };
	m_rect = ska::RectangleUtils::posToCenterPicture(m_rect, placeHolderRect);
	ska::Widget::move(pos);
}

