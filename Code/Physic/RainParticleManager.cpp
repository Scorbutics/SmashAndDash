#include <math.h>
#include "../Gameplay/WGameCore.h"
#include "RainParticleManager.h"
#include "../Gameplay/World/Layer.h"
#include "RainParticle.h"

RainParticleManager::RainParticleManager()
{
	m_wind = 0;
	m_countWind = 1;
	m_Twind = m_countWind * M_PI / 96; //période du vent : simule une perturbation dans la pluie
}

int RainParticleManager::getWind() const {
	const double windX = cos(m_Twind);
	return (int)((windX > 0 ? 8 * TAILLEBLOC*windX : 0));
}


void RainParticleManager::displayRainFog()
{
	WGameCore& wScreen = WGameCore::getInstance();

	//Si la pluie est jouée, on met un effet "temps couvert"
	if (m_active) {
		unsigned int mosaicNumberX, mosaicNumberY;
		mosaicNumberX = wScreen.getWidth() / m_rainWeather.getWidth() + 1;
		mosaicNumberY = wScreen.getHeight() / m_rainWeather.getHeight() + 1;

		SDL_Rect buf;
		buf.x = buf.y = 0;
		for (unsigned int j = 0; j < mosaicNumberX; j++) {
			for (unsigned int k = 0; k < mosaicNumberY; k++) {
				m_rainWeather.render(buf.x, buf.y);
				buf.y += m_rainWeather.getHeight();
			}
			buf.x += m_rainWeather.getWidth();
			buf.y = 0;
		}

		if (m_countWind < 2000 && m_countWind != 0) {
			m_countWind += m_sens;
			m_Twind = m_countWind * M_PI / 140;
		} else {
			m_sens *= -1;
			m_countWind += m_sens;
			m_Twind = m_countWind * M_PI / 140;
		}

	}
}

void RainParticleManager::display() {
	ParticleManager::display();	
	displayRainFog();
}

void RainParticleManager::refresh() {
	ParticleManager::refresh();

	WGameCore& wScreen = WGameCore::getInstance();
	World& w = wScreen.getWorld();
	const double windX = cos(m_Twind);
	if (windX > 0)
	{
		//ajoute un effet de vent en fonction de la pluie
		const int windForce = (int)(3 * windX + 0.5);
		w.setWind(windForce);
	}
}

void RainParticleManager::playRain(int idSprite, float acceleration, float density, unsigned int duration)
{
	WGameCore& wScreen = WGameCore::getInstance();

	m_duration = duration;
	m_active = true;
	//this->removeAll(PARTICLE_MANAGER_RAIN);

	SDL_Rect posBuf;
	for (int i = 0; i < wScreen.getWidth()* wScreen.getHeight()*density / (TAILLEBLOC*TAILLEBLOC); i++)
	{
		posBuf.y = wScreen.getHeight() / 2;
		posBuf.x = wScreen.getWidth() / 2;
		this->addParticle(ParticlePtr(new RainParticle(*this, i, idSprite, posBuf, rand() % ((int)(90 - acceleration + 1)) + acceleration, acceleration, acceleration, true, true)));
		//ajoute une particule gravitationnelle d'id 0
	}

	m_rainWeather.load("."FILE_SEPARATOR"Sprites"FILE_SEPARATOR"Meteo"FILE_SEPARATOR"gloomy.png", -1, -1, -1, 128);
	m_t0 = SDL_GetTicks();

}

RainParticleManager::~RainParticleManager()
{
}
