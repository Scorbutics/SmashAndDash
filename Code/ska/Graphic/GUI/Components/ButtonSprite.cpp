#include <limits>
#include "ButtonSprite.h"
#include "../../../Graphic/SpritePath.h"
#include "../../../Utils/RectangleUtils.h"

ska::ButtonSprite::ButtonSprite(Widget& parent, ska::Point<int> relativePos, const std::string& placeHolderStyleName, unsigned int id, ClickEventHandler const& callback) :
Button(parent, relativePos, placeHolderStyleName, nullptr, callback),
m_img(ska::SpritePath::getInstance().getPath(SPRITEBANK_ICONS, id)) {
	memset(&m_clip, 0, sizeof m_clip);
	move(getRelativePosition() + relativePos);
	m_clip.w = std::numeric_limits<int>().max();
	m_clip.h = std::numeric_limits<int>().max();
}

ska::ButtonSprite::ButtonSprite(Widget& parent, ska::Point<int> relativePos, const std::string& placeHolderStyleName, const std::string& imagePath, const ska::Rectangle& clip, ClickEventHandler const& callback) :
Button(parent, relativePos, placeHolderStyleName, nullptr, callback),
m_img(imagePath),
m_clip(clip) {
	move(getRelativePosition() + relativePos);
}

void ska::ButtonSprite::replaceWith(const std::string& path, const ska::Rectangle& clip) {
	m_img.load(path);
	m_clip = clip;
}

void ska::ButtonSprite::clear() {
	m_img.free();
}

void ska::ButtonSprite::display() const {
	if(!isVisible()) {
		return;
	}
	ska::Button::display();

	const auto& pos = getAbsolutePosition();
	m_img.render(pos.x + m_rect.x, pos.y + m_rect.y, std::numeric_limits<int>().max() == m_clip.w ? nullptr : &m_clip);
}

void ska::ButtonSprite::move(const ska::Point<int>& pos) {
	const auto& box = getBox();

	ska::Rectangle placeHolderRect = ska::Rectangle{ 0, 0, box.w, box.h };
	m_rect = m_clip.w == 0 ? ska::Rectangle{ 0, 0, m_img.getWidth(), m_img.getHeight() } : m_clip;
	m_rect = ska::RectangleUtils::posToCenterPicture(m_rect, placeHolderRect);
	ska::Widget::move(pos);
}

