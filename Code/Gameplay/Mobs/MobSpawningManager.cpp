#include "MobSpawningManager.h"
#include "../../Utils\IDs.h"
#include "..\World\WorldImpl.h"
#include "../../Utils\ChargementImages.h"
#include "MobSpawner.h"

using namespace std;

MobSpawningManager::MobSpawningManager(MobSpawner& w, unsigned int delay) : m_mobSpawner(w)
{
	m_t0 = SDL_GetTicks();
	m_duration = delay;
	m_rmin = 8*TAILLEBLOC;
	m_rmax = m_rmin + 5*TAILLEBLOC;
	m_spawnNum = 1;
}

void MobSpawningManager::spawnOnWorld(unsigned int spawnNumber)
{
	const unsigned int blockSize = m_mobSpawner.getBlockSize();
	const unsigned int range = (m_mobSpawner.getNbrBlocX() > m_mobSpawner.getNbrBlocY() ? m_mobSpawner.getNbrBlocX()*blockSize : m_mobSpawner.getNbrBlocY()*blockSize);

	for(unsigned int i = 0; i < spawnNumber; i++) {
		ska::IniReader* reader = GetRandomMobSettings(&m_mobSpawner);

		if(reader != NULL) {
			ska::Rectangle pos;
			pos.x = m_mobSpawner.getNbrBlocX()*blockSize / 2;
			pos.y = m_mobSpawner.getNbrBlocY()*blockSize / 2;

			m_mobSpawner.spawnMob(pos, 0, range, m_spawnNum, reader);
		}
			
	}

}

void MobSpawningManager::refresh()
{

	/*if (ska::TimeUtils::getTicks() - m_t0 >= m_duration) {
		ska::IniReader* reader = GetRandomMobSettings(&m_world);
		if (reader != NULL) {
			m_world.spawnMob(pos, m_rmin, m_rmax, m_spawnNum, reader);
		}
		m_t0 = ska::TimeUtils::getTicks();
	}*/
}

MobSpawningManager::~MobSpawningManager()
{}