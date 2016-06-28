#pragma once
#include "../../ska/World/World.h"
#include "../Weather.h"

namespace ska {
	class IniReader;
	class PrefabEntityManager;
}

class WorldImpl : public ska::World {
	
public:
	WorldImpl(unsigned int tailleBloc, const unsigned int wWidth, const unsigned int wHeight);
	~WorldImpl();
	void load(const std::string& fileName, const std::string& chipsetName) override;
	
	void loadWeatherFromData(const std::string& stringDataFile);
	void loadFogFromData(const std::string& stringDataFile);

	virtual void graphicUpdate(ska::DrawableContainer& drawables) override;

private:
	Weather m_fog;
	Weather m_weather;
	
};