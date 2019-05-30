#pragma once
#include "World/TileWorld.h"
#include "../Data/SettingsChangeEvent.h"
#include "../PokemonGameEventDispatcher.h"
#include "../Weather.h"
#include "Base/Patterns/SubObserver.h"

namespace ska {
	class IniReader;
	class PrefabEntityManager;
}

class WorldImpl :
	public ska::TileWorld,
	public ska::SubObserver<SettingsChangeEvent> {

public:
	WorldImpl(PokemonGameEventDispatcher& ged, ska::Tileset& tileset, const ska::TileWorldLoader& loader);
	~WorldImpl() = default;

	WorldImpl& operator=(const WorldImpl&) = delete;
	void load(const ska::TilesetCorrespondanceMapper& mapper, const std::string& fileName, ska::Tileset* tileset);

	void loadWeatherFromData(const std::string& stringDataFile);
	void loadFogFromData(const std::string& stringDataFile);

	bool onSettingsChange(SettingsChangeEvent& sce);

	std::vector<ska::IniReader>& getMobSettings();

	//TODO Weather has nothing to do here...
	Weather& getFog();
	Weather& getWeather();

private:
	std::vector<ska::IniReader> m_mobSettings;
	Weather m_fog;
	Weather m_weather;
};
