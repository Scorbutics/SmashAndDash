#ifndef DEF_SHAKERMANAGER
#define DEF_SHAKERMANAGER

#include "Rectangle.h"
class WGameCore;
class World;

class ShakerManager
{
	public:
	ShakerManager();
	~ShakerManager();
	void refresh();
	void shake(unsigned int intensity, unsigned int duration);

	private:
	bool m_active;
	ska::Rectangle m_oRelBackup;
	unsigned int m_duration, m_t0;
	int m_intensity, m_sens;

};

#endif
