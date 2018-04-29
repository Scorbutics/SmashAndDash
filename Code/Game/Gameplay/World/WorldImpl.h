#pragma once
#include "World/TileWorld.h"
#include "../Data/SettingsChangeEvent.h"
#include "../PokemonGameEventDispatcher.h"
#include "../Weather.h"
#include "Utils/SubObserver.h"

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
	void load(const std::string& fileName, const std::string& chipsetName);

	void loadWeatherFromData(const std::string& stringDataFile);
	void loadFogFromData(const std::string& stringDataFile);

	bool onSettingsChange(SettingsChangeEvent& sce);

	void graphicUpdate(unsigned int ellapsedTime, ska::DrawableContainer& drawables) override;
	std::vector<ska::IniReader>& getMobSettings();

private:
	std::vector<ska::IniReader> m_mobSettings;
	Weather m_fog;
	Weather m_weather;
};
