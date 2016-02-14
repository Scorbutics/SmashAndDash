#ifndef DEF_MOBSPAWNINGMANAGER
#define DEF_MOBSPAWNINGMANAGER

class MobSpawningManager
{
	public:
	MobSpawningManager(unsigned int delay);
	void refresh();
	~MobSpawningManager();
	void spawnOnWorld(unsigned int probPercentSpawnPerBlock);

	private:
	unsigned int m_t0, m_duration, m_rmin, m_rmax, m_spawnNum;


};

#endif