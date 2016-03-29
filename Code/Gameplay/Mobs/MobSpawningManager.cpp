#include "MobSpawningManager.h"
#include "../../Utils\IDs.h"
#include "..\World\WorldImpl.h"
#include "../../Utils\ChargementImages.h"

using namespace std;

MobSpawningManager::MobSpawningManager(WorldImpl& w, unsigned int delay) : m_world(w)
{
	m_t0 = SDL_GetTicks();
	m_duration = delay;
	m_rmin = 8*TAILLEBLOC;
	m_rmax = m_rmin + 5*TAILLEBLOC;
	m_spawnNum = 1;
}

void MobSpawningManager::spawnOnWorld(unsigned int spawnNumber)
{
	const unsigned int blockSize = m_world.getBlockSize();
	const unsigned int range = (m_world.getNbrBlocX() > m_world.getNbrBlocY() ? m_world.getNbrBlocX()*blockSize : m_world.getNbrBlocY()*blockSize);

	for(unsigned int i = 0; i < spawnNumber; i++) {
		ska::IniReader* reader = GetRandomMobSettings(&m_world);

		if(reader != NULL) {
			ska::Rectangle pos;
			pos.x = m_world.getNbrBlocX()*blockSize / 2;
			pos.y = m_world.getNbrBlocY()*blockSize / 2;

			m_world.spawnMob(pos, 0, range, m_spawnNum, reader);
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