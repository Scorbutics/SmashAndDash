#include <string>
#include "Weather.h"
#include "World/TileWorld.h"
#include "Utils/NumberUtils.h"
#include "Draw/Renderer.h"
#include "Draw/DrawableContainer.h"

Weather::Weather(ska::TileWorld& w, const std::string& wSprite, int number, int distance, int intensityX, int intensityY, int alpha) :
    m_number(0),
    m_world(w) {
	m_active = false;
	m_mosaic = false;
	load(wSprite, number, distance, intensityX, intensityY, alpha);
}

Weather::Weather(ska::TileWorld& w) :
	m_intensityX(0),
	m_intensityY(0),
	m_number(0),
    m_distance(0),
	m_world(w) {

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
		m_weather = std::make_unique<ska::Texture>(wSprite, ska::Texture::DEFAULT_T_RED, ska::Texture::DEFAULT_T_GREEN, ska::Texture::DEFAULT_T_BLUE, alpha);
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
    for(auto i = 0; i < m_number; i++) {
		m_pos.push_back({ 0, 0 });
    }
}

void Weather::setMosaicEffect(bool x) {
    m_mosaic = x;
}

void Weather::resetPos() {
    for(auto i = 0; i < m_number; i++) {
		m_pos[i] = { 0, 0 };
    }

}

void Weather::graphicUpdate(const ska::Rectangle & cameraPos, ska::DrawableContainer& drawables) {
	if (isVisible()) {
		m_cameraPos = cameraPos;
		drawables.add(*this);
	}
}

void Weather::resetRandomPos() {
    for(auto i = 0; i < m_number; i++) {
	    const auto radius = static_cast<float>(ska::NumberUtils::random(m_distance, m_distance + m_weather->getWidth() * 2));
		m_pos[i] = ska::Point<float>::cartesian(radius, ska::NumberUtils::random(0.0, 2*M_PI));
    }
}

void Weather::update() {
	if (!m_active) {
		return;
	}

	const auto worldWidth = static_cast<float>(m_world.getPixelWidth());
	const auto worldHeight = static_cast<float>(m_world.getPixelHeight());
	
	for (auto i = 0; i < m_number; i++) {

		m_pos[i].x += static_cast<float>(m_intensityX / 5.);
		m_pos[i].y += static_cast<float>(m_intensityY / 5.);

		if ((m_pos[i].x + m_weather->getWidth()) < 0 && m_intensityX < 0) {
			m_pos[i].x = worldWidth;
		} else if (m_pos[i].x > worldWidth && m_intensityX > 0) {
			m_pos[i].x = -worldWidth;
		}

		if ((m_pos[i].y + m_weather->getHeight())< 0 && m_intensityY < 0) {
			m_pos[i].y = worldHeight;
		} else if (m_pos[i].y > worldHeight && m_intensityY > 0) {
			m_pos[i].y = -worldHeight;
		}
	}
}

void Weather::render(const ska::Renderer& render) const {
	const auto& worldView = m_cameraPos;

	if (!m_active || m_weather == nullptr) {
		return;
	}
	
	const auto cameraWidth = static_cast<float>(worldView.w);
	const auto cameraHeight = static_cast<float>(worldView.h);

    for(auto i = 0; i < m_number; i++) {
        if(m_mosaic) {
			const auto weatherWidth = m_weather->getWidth();
			const auto weatherHeight = m_weather->getHeight();
			if(weatherWidth == 0|| weatherHeight == 0) {
				return;
			}
	        const auto nbrMosaicX = static_cast<int>(cameraWidth / weatherWidth + 2);
	        const auto nbrMosaicY = static_cast<int>(cameraHeight / weatherHeight + 2);
			const ska::Point<int> oRel = { static_cast<int>(- worldView.x + (worldView.x / weatherWidth) * weatherWidth), static_cast<int>(-worldView.y + (worldView.y / weatherHeight) * weatherHeight)};

			ska::Point<int> buf;
            for(auto i1 = 0; i1 < nbrMosaicX; i1++) {
				buf.x = i1 * weatherWidth + oRel.x;
                for(auto j = 0; j < nbrMosaicY; j++) {
					buf.y = j * weatherHeight + oRel.y;	
					render.render(*m_weather, buf.x, buf.y, nullptr);
                }
            }

		} else {
			const ska::Rectangle oRel = { -(worldView.x), -(worldView.y), 0, 0 };
			const ska::Point<int> buf { static_cast<int>(m_pos[i].x) + oRel.x, static_cast<int>(m_pos[i].y) + oRel.y };
			render.render(*m_weather, buf.x, buf.y, nullptr);
		}
    }
}

void Weather::hide(bool active) {
    m_active = !active;
}
