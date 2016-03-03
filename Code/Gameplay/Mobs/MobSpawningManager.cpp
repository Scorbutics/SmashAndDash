#include "MobSpawningManager.h"
#include "../../Utils\IDs.h"
#include "..\WGameCore.h"
#include "../../Utils\ChargementImages.h"

using namespace std;

MobSpawningManager::MobSpawningManager(unsigned int delay)
{
	m_t0 = SDL_GetTicks();
	m_duration = delay;
	m_rmin = 8*TAILLEBLOC;
	m_rmax = m_rmin + 5*TAILLEBLOC;
	m_spawnNum = 1;
}

void MobSpawningManager::spawnOnWorld(unsigned int spawnNumber)
{
	WGameCore& wScreen = WGameCore::getInstance();
	ska::World& w = wScreen.getWorld();
	const unsigned int range = (w.getNbrBlocX() > w.getNbrBlocY() ? w.getNbrBlocX()*TAILLEBLOC: w.getNbrBlocY()*TAILLEBLOC);

	for(unsigned int i = 0; i < spawnNumber; i++)
	{
			ska::IniReader* reader = GetRandomMobSettings(&w);

			if(reader != NULL)
			{
				ska::Rectangle pos;
				pos.x = w.getNbrBlocX()*TAILLEBLOC/2;
				pos.y = w.getNbrBlocY()*TAILLEBLOC/2;

				w.spawnMob(pos, 0, range, m_spawnNum, reader);
				
			}
			
	}

}

void MobSpawningManager::refresh()
{
	WGameCore& wScreen = WGameCore::getInstance();

	if(SDL_GetTicks() - m_t0 >= m_duration)
	{
		ska::World& w = wScreen.getWorld();

		ska::IniReader* reader = GetRandomMobSettings(&w);

		if(reader != NULL)
		{
			ska::Rectangle pos = wScreen.getORel();
			pos.x *= -1;
			pos.y *= -1;
			pos.x += wScreen.getWidth()/2;
			pos.y += wScreen.getHeight()/2;


			w.spawnMob(pos, m_rmin, m_rmax, m_spawnNum, reader);
		}

		m_t0 = SDL_GetTicks();
	}
}

MobSpawningManager::~MobSpawningManager()
{}