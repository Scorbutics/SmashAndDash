#pragma once
#include "World/World.h"
#include "Utils/Observer.h"
#include "../Data/SettingsChangeEvent.h"
#include "../PokemonGameEventDispatcher.h"
#include "../Weather.h"

namespace ska {
	class IniReader;
	class PrefabEntityManager;
}

class WorldImpl :
	public ska::World,
	public ska::Observer<SettingsChangeEvent> {

public:
	WorldImpl(PokemonGameEventDispatcher& ged, unsigned int tailleBloc, const unsigned int wWidth, const unsigned int wHeight);
	WorldImpl(const WorldImpl&) = delete;
	~WorldImpl();

	WorldImpl& operator=(const WorldImpl&) = delete;
	void load(const std::string& fileName, const std::string& chipsetName) override;

	void loadWeatherFromData(const std::string& stringDataFile);
	void loadFogFromData(const std::string& stringDataFile);

	bool onSettingsChange(SettingsChangeEvent& sce);

	virtual void graphicUpdate(unsigned int ellapsedTime, ska::DrawableContainer& drawables) override;

private:
	Weather m_fog;
	Weather m_weather;
	PokemonGameEventDispatcher& m_ged;

};
