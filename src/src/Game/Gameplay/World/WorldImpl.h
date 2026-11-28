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
	WorldImpl(PokemonGameEventDispatcher& ged, unsigned int blockSize);
	~WorldImpl() = default;

	WorldImpl& operator=(const WorldImpl&) = delete;
	// The map the player is on ("<world>/w_5_4") : its wild Pokemon, its weather, its fog
	void setMap(const std::string& mapDirectory);

	void loadWeatherFromData(const std::string& stringDataFile);
	void loadFogFromData(const std::string& stringDataFile);

	bool onSettingsChange(SettingsChangeEvent& sce);

	std::vector<ska::IniReader>& getMobSettings();

	//TODO Weather has nothing to do here...
	Weather& getFog();
	Weather& getWeather();

private:
	std::vector<ska::IniReader> m_mobSettings;
	// The weather and the fog shown : loaded again only when another map has others
	std::string m_weatherShown;
	std::string m_fogShown;
	Weather m_fog;
	Weather m_weather;
};
