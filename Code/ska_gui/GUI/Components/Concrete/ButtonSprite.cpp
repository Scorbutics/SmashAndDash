#include <limits>
#include "ButtonSprite.h"

#include "Utils/SpritePath.h"

ska::ButtonSprite::ButtonSprite(Widget& parent, Point<int> relativePos, const std::string& placeHolderStyleName, unsigned int id, ClickEventHandler const& callback) :
Button(parent, relativePos, placeHolderStyleName, nullptr, callback),
m_img(SpritePath::getInstance().getPath(SPRITEBANK_ICONS, id)) {
	m_clip = Rectangle{0, 0, 0, 0};
	ButtonSprite::move(getRelativePosition() + relativePos);
	m_clip.w = std::numeric_limits<int>().max();
	m_clip.h = std::numeric_limits<int>().max();
}

ska::ButtonSprite::ButtonSprite(Widget& parent, Point<int> relativePos, const std::string& placeHolderStyleName, const std::string& imagePath, const Rectangle& clip, ClickEventHandler const& callback) :
Button(parent, relativePos, placeHolderStyleName, nullptr, callback),
m_img(imagePath),
m_clip(clip) {
	ButtonSprite::move(getRelativePosition() + relativePos);
}

void ska::ButtonSprite::replaceWith(const std::string& path, const Rectangle& clip) {
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
	Button::display();

	const auto& pos = getAbsolutePosition();
	m_img.render(pos.x + m_rect.x, pos.y + m_rect.y, std::numeric_limits<int>().max() == m_clip.w ? nullptr : &m_clip);
}

void ska::ButtonSprite::move(const Point<int>& pos) {
	const auto& box = getBox();

	Rectangle placeHolderRect = Rectangle{ 0, 0, box.w, box.h };
	m_rect = m_clip.w == 0 ? Rectangle{ 0, 0, static_cast<int>(m_img.getWidth()), static_cast<int>(m_img.getHeight()) } : m_clip;
	m_rect = RectangleUtils::posToCenterPicture(m_rect, placeHolderRect);
	Widget::move(pos);
}

