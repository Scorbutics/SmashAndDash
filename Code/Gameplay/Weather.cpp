#include <iostream>
#include <string>
#include "Weather.h"
#include "../ska/World/World.h"
#include "../ska/Utils/NumberUtils.h"

Weather::Weather(ska::World& w, const std::string& wSprite, int number, int distance, int intensityX, int intensityY, int alpha) : m_world(w) {
	m_active = false;
	m_mosaic = false;
	load(wSprite, number, distance, intensityX, intensityY, alpha);
}

Weather::Weather(ska::World& w) : m_world(w) {
	m_active = false;
	m_mosaic = false;
}

void Weather::load(const std::string& wSprite, int number, int distance, int intensityX, int intensityY, int alpha) {
	m_distance = distance;
	m_intensityX = intensityX;
	m_intensityY = intensityY;
	setNumber(number);
	m_active = true;
	
	if (!wSprite.empty()) {
		m_weather = ska::TexturePtr(new ska::Texture(wSprite, DEFAULT_T_RED, DEFAULT_T_GREEN, DEFAULT_T_BLUE, alpha));
	}

	resetRandomPos();
}

bool Weather::isVisible() const {
	return m_active;
}

void Weather::setNumber(int number) {
	m_pos.clear();
	
    m_number = number;

	m_pos.reserve(m_number);
    for(int i = 0; i < m_number; i++) {
		m_pos.push_back({ 0, 0 });
    }
}

void Weather::setMosaicEffect(bool x) {
    m_mosaic = x;
}

void Weather::resetPos() {
    for(int i = 0; i < m_number; i++) {
		m_pos[i] = { 0, 0 };
    }

}

void Weather::resetRandomPos() {
    for(int i = 0; i < m_number; i++) {
		float radius = (float) ska::NumberUtils::random(m_distance, m_distance + m_weather->getWidth() * 2);
		m_pos[i] = ska::NumberUtils::cartesian(radius, ska::NumberUtils::random(0.0, 2*M_PI));
    }
}

void Weather::update() {
	const ska::Rectangle* worldView = m_world.getView();

	if (!m_active || worldView == nullptr) {
		return;
	}

	const float worldWidth = (float)m_world.getPixelWidth();
	const float worldHeight = (float)m_world.getPixelHeight();
	
	const ska::Rectangle oRel = { -worldView->x, -worldView->y };

	for (int i = 0; i < m_number; i++) {

		m_pos[i].x += (float)(m_intensityX / 5.);
		m_pos[i].y += (float)(m_intensityY / 5.);

		if ((m_pos[i].x + m_weather->getWidth()) < 0 && m_intensityX < 0) {
			m_pos[i].x = worldWidth;
		}
		else if (m_pos[i].x > worldWidth && m_intensityX > 0) {
			m_pos[i].x = -worldWidth;
		}

		if ((m_pos[i].y + m_weather->getHeight())< 0 && m_intensityY < 0) {
			m_pos[i].y = worldHeight;
		}
		else if (m_pos[i].y > worldHeight && m_intensityY > 0) {
			m_pos[i].y = -worldHeight;
		}
	}
}

void Weather::display() const {
	const ska::Rectangle* worldView = m_world.getView();

	if (!m_active || worldView == nullptr) {
		return;
	}

	const ska::Rectangle oRel = { -worldView->x, -worldView->y };

	const float worldWidth = (float) m_world.getPixelWidth();
	const float worldHeight = (float) m_world.getPixelHeight();

    for(int i = 0; i < m_number; i++) {

		ska::Rectangle buf;
		buf.x = (int)m_pos[i].x + oRel.x;
		buf.y = (int)m_pos[i].y + oRel.y;

        if(m_mosaic) {
            int nbrMosaicX, nbrMosaicY;
			nbrMosaicX = worldWidth / m_weather->getWidth() + 1;
			nbrMosaicY = worldHeight / m_weather->getHeight() + 1;

            for(int i = 0; i < nbrMosaicX; i++) {
				buf.x = i * m_weather->getWidth() + oRel.x;
                for(int j = 0; j < nbrMosaicY; j++) {
					buf.y = j*m_weather->getHeight() + oRel.y;
					if (buf.x + m_weather->getWidth() >= 0 && buf.x <= worldWidth && buf.y + m_weather->getHeight() >= 0 && buf.y <= worldHeight) {
						m_weather->render(buf.x, buf.y);
					}
                }
            }

		} else if (buf.x + m_weather->getWidth() >= 0 && buf.x <= worldWidth && buf.y + m_weather->getHeight() >= 0 && buf.y <= worldHeight) {
			m_weather->render(buf.x, buf.y);
		}
    }
}

void Weather::hide(bool active)
{
    m_active = !active;
}

Weather::~Weather()
{
}
