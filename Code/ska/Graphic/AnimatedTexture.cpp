#include "AnimatedTexture.h"

using namespace std;

ska::AnimatedTexture::AnimatedTexture() {

}

void ska::AnimatedTexture::loadFromText(unsigned int fontSize, std::string text, SDL_Color c, const unsigned int horizontalFrames, const unsigned int verticalFrames, const unsigned int animatedFrames, bool isVertical) {
	m_sprite.loadFromText(fontSize, text, c);
	recalculateFrames(horizontalFrames, verticalFrames, animatedFrames, isVertical);
}

void ska::AnimatedTexture::load(std::string id, const unsigned int horizontalFrames, const unsigned int verticalFrames, const unsigned int animatedFrames, bool isVertical, int r, int g, int b, int a) {
	m_sprite.load(id, r, g, b, a);
	recalculateFrames(horizontalFrames, verticalFrames, animatedFrames, isVertical);
}

void ska::AnimatedTexture::recalculateFrames(const unsigned int horizontalFrames, const unsigned int verticalFrames, const unsigned int animatedFrames, const bool isVertical) {
	m_anim.setFrames(animatedFrames);
	m_anim.setVertical(isVertical);
	const unsigned int width = m_sprite.getWidth() / horizontalFrames;
	const unsigned int height = m_sprite.getHeight() / verticalFrames;
	Rectangle frame = { 0, 0, width, height };
	m_anim.setOffsetAndFrameSize(frame);
}

void ska::AnimatedTexture::setOffset(const Point<int>& offset) {
	Rectangle tmp = m_anim.getOffsetAndFrameSize();
	tmp.x = offset.x;
	tmp.y = offset.y;
	m_anim.setOffsetAndFrameSize(tmp);
}

void ska::AnimatedTexture::operator=(const AnimatedTexture& text) {
	m_anim = text.m_anim;
	m_sprite = text.m_sprite;
}

void ska::AnimatedTexture::setAlpha(int alpha) {
	m_sprite.setAlpha(alpha);
}

void ska::AnimatedTexture::setColor(int red, int green, int blue) {
	m_sprite.setColor(red, green, blue);
}

void ska::AnimatedTexture::setDelay(const unsigned int delay) {
	m_anim.setDelay(delay);
}

int ska::AnimatedTexture::render(int x, int y) {
	Rectangle tmp = m_anim.getRectOfCurrentFrame();
	return m_sprite.render(x, y, &tmp);
}

const unsigned int ska::AnimatedTexture::getWidth() const {
	return m_sprite.getWidth();
}

const unsigned int ska::AnimatedTexture::getHeight() const {
	return m_sprite.getHeight();
}

void ska::AnimatedTexture::stop(const bool x) {
	m_anim.stop(x);
}

void ska::AnimatedTexture::nextFrame() {
	m_anim.nextFrame();
}