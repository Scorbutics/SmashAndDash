#ifndef DEF_MOBSPAWNINGMANAGER
#define DEF_MOBSPAWNINGMANAGER

class WorldImpl;

class MobSpawningManager {

public:
	MobSpawningManager(WorldImpl& w, unsigned int delay);
	void refresh();
	void spawnOnWorld(unsigned int probPercentSpawnPerBlock);
	~MobSpawningManager();

private:
	unsigned int m_t0, m_duration, m_rmin, m_rmax, m_spawnNum;
	WorldImpl& m_world;


};

#endif