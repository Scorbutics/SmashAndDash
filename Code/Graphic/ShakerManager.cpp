#include "ShakerManager.h"
#include "Gameplay\WGameCore.h"
#include "Utils\Singleton_template.h"


ShakerManager::ShakerManager()
{
	m_active = false;
	m_duration = 0;
	m_sens = -1;
	m_intensity = 0;
}


void ShakerManager::refresh()
{
	WGameCore& wScreen = WGameCore::getInstance();

	if(!m_active)
		return;

	if(SDL_GetTicks() - m_t0 < m_duration)
	{
		SDL_Rect buf = m_oRelBackup;

		if (m_sens < 0)
			buf.x = rand()%(m_intensity+1) + m_oRelBackup.x;
		else
			buf.x = m_oRelBackup.x;

		wScreen.setORel(buf);

		m_sens *= -1;
	}
	else
	{
		m_active = false;
		wScreen.setORel(m_oRelBackup);
	}

}


void ShakerManager::shake(unsigned int intensity, unsigned int duration)
{
	WGameCore& wScreen = WGameCore::getInstance();
	//si jamais on n'est PAS déjà en train de secouer l'écran
	//(évite d'appeler plusieurs fois la fonction pendant sa propre exécution)
	//(effet de récursivité sinon)
	if(!m_active)
	{
		//alors on commence les secousses
		m_active = true;

		m_t0 = SDL_GetTicks();
		m_duration = duration;
		m_intensity = intensity;

		m_oRelBackup = wScreen.getORel();
	}

}


ShakerManager::~ShakerManager(){}
