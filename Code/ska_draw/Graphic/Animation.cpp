#include <iostream>
#include <string>

#include "Animation.h"

ska::Animation::Animation() {
	m_offset = Rectangle{0, 0, 0, 0};
	m_offsetBase = m_offset;
	m_frameWidth = m_offset.w;
	m_frameHeight = m_offset.h;
	m_frameNumber = 1;
	m_count = SDL_GetTicks();
	m_countMAX = static_cast<unsigned int>(-1);
	m_position = 0;
	m_isVertical = false;
	m_sensAni = 1;
	m_active = true;
	m_cycles = 0;
}

ska::Animation::Animation(unsigned int delay, int frameNumber, bool isVertical, Rectangle offsetAndFrameSize) {
    m_offset = offsetAndFrameSize;
    m_offsetBase = m_offset;
    m_frameWidth = offsetAndFrameSize.w;
    m_frameHeight = offsetAndFrameSize.h;
    m_frameNumber = frameNumber;
	if(m_frameNumber == 0)
		m_frameNumber = 1;
    m_count = SDL_GetTicks();
    m_countMAX = delay;
    m_position = 0;
    m_isVertical = isVertical;
    m_sensAni = 1;
    m_active = true;
	m_cycles = 0;
}

ska::Animation::Animation(unsigned int delay, int frameNumber, bool isVertical, int x, int y, unsigned int width, unsigned int height) {
	Rectangle r;
	r.x = x;
	r.y = y;
	r.w = width;
	r.h = height;

	m_offset = r;
	m_offsetBase = m_offset;
	m_frameWidth = r.w;
	m_frameHeight = r.h;
    m_frameNumber = frameNumber;
	if(m_frameNumber == 0)
		m_frameNumber = 1;
    m_count = SDL_GetTicks();
    m_countMAX = delay;
    m_position = 0;
    m_isVertical = isVertical;
    m_sensAni = 1;
    m_active = true;
	m_cycles = 0;
}

void ska::Animation::setVertical(const bool b) {
	m_isVertical = b;
}

unsigned int ska::Animation::getNumberFrames() {
	return m_frameNumber;
}

unsigned int ska::Animation::getDelay() {
	return m_countMAX;
}

void ska::Animation::stop(bool x) {
    m_active = !x;
}

int ska::Animation::getPosition() {
	return m_position;
}

int ska::Animation::getMaxPosition() {
	return m_frameNumber - 1;
}

unsigned int ska::Animation::getCount() {
    return m_count;
}

void ska::Animation::setCount(unsigned int x) {
    m_count = SDL_GetTicks() + x;
}

void ska::Animation::setDelay(unsigned int x) {
    m_countMAX = x;
}

void ska::Animation::resetCount() {
    m_count = SDL_GetTicks();
}

unsigned int ska::Animation::getCycles() {
	return m_cycles;
}

void ska::Animation::setOffsetAndFrameSize(Rectangle s) {
    m_offsetBase = s;
    m_frameWidth = s.w;
    m_frameHeight = s.h;
    m_offset = s;

	if (m_isVertical) {
		m_offset.y = m_offsetBase.y + m_position*m_frameHeight;
	} else {
		m_offset.x = m_offsetBase.x + m_position*m_frameWidth;
	}
}

const ska::Rectangle ska::Animation::getOffsetBase() const {
	return m_offsetBase;
}

const ska::Rectangle ska::Animation::getOffsetAndFrameSize() const {
    return m_offset;
}

void ska::Animation::setFrames(const unsigned int framesNumber) {
	m_frameNumber = framesNumber;
}

void ska::Animation::setCurrentFrame(unsigned int number) {
	m_position = number;

    if(m_position >= m_frameNumber) {
        m_sensAni = -1;
        m_position = m_frameNumber-2;
    }
	if(m_position <= 0) {
        m_sensAni = 1;
        m_position = 0;
    }

	if (m_isVertical) {
		m_offset.y = m_offsetBase.y + m_position*m_frameHeight;
	} else {
		m_offset.x = m_offsetBase.x + m_position*m_frameWidth;
	}

}

void ska::Animation::nextFrame() {
    m_position += m_sensAni;
	setCurrentFrame(m_position);
}

void ska::Animation::resetCycles() {
	m_cycles = 0;
}

const ska::Rectangle ska::Animation::getRectOfCurrentFrame() {

	if (m_active == false) {
		return m_offset;
	}

    if(SDL_GetTicks() - m_count > m_countMAX) {
        m_count = SDL_GetTicks();
		nextFrame();
    }


    return m_offset;
}

