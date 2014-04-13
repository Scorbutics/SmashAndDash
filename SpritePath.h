#ifndef DEF_SPRITEPATH
#define DEF_SPRITEPATH

#include <string>
#include <vector>
#include <SDL2/SDL.h>
#include "Singleton_template.h"


class SpritePath : public Singleton<SpritePath>
{
	friend class Singleton<SpritePath>;

    private:
		std::vector<std::string> m_paths;
		SpritePath();
		~SpritePath();

	public:
		std::string getPath(unsigned int spritebankType, int id);
		

};

#endif
