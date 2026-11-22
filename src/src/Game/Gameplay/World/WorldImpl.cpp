#include <fstream>
#include <climits>
#include "Game/__internalConfig/LoggerConfig.h"
#include "../Data/Settings.h"
#include "WorldImpl.h"
#include "Core/Draw/DrawableContainer.h"
#include "Base/IO/Readers/IniReader.h"
#include "Base/IO/Files/FileUtils.h"
#include "WorldLoader.h"

#define WEATHER_ALPHA_LVL 85

WorldImpl::WorldImpl(PokemonGameEventDispatcher& ged, ska::Tileset& tileset, const ska::TileWorldLoader& loader) :
	TileWorld(ged, tileset, loader),
	SubObserver<SettingsChangeEvent>(std::bind(&WorldImpl::onSettingsChange, this, std::placeholders::_1), ged) ,
	m_fog(*this),
	m_weather(*this) {
	m_fog.setMosaicEffect(true);

	/* TODO organisation des "priorities" fixes dans un enum pour mettre de la cohérence */
	m_weather.setPriority(INT_MAX - 3);
	m_fog.setPriority(INT_MAX - 4);
	m_fog.resetPos();
	m_fog.setMosaicEffect(true);
}

std::vector<ska::IniReader>& WorldImpl::getMobSettings() {
	return m_mobSettings;
}

Weather& WorldImpl::getFog() {
	return m_fog;
}

Weather& WorldImpl::getWeather() {
	return m_weather;
}

void WorldImpl::load(const ska::TilesetCorrespondanceMapper& mapper, const std::string& fileName, ska::Tileset* tileset) {
	TileWorld::load(BuildWorldLoader(mapper, fileName), tileset);
	const ska::FileNameData fndata(fileName);
	
	const auto& dataFile = fndata.path + "/" + fndata.name + "/" + fndata.name + ".ini";
	loadWeatherFromData(dataFile);
	loadFogFromData(dataFile);

	// The wild Pokemon of the map : Levels/<map>/Monsters/0.ini, 1.ini... (MobSpawningSystem)
	m_mobSettings.clear();
	for (auto index = 0;; index++) {
		const auto file = fndata.path + "/" + fndata.name + "/Monsters/" + std::to_string(index) + ".ini";
		if (!std::ifstream{ file }) {
			break;
		}
		m_mobSettings.emplace_back(file);
	}
}

void WorldImpl::loadFogFromData(const std::string& stringDataFile) {
	ska::IniReader reader(stringDataFile);

	const std::string& fogSpritePath = "Fog sprite";
	const auto sprite = reader.exists(fogSpritePath);
	// (A map without fog has no fog settings)
	if(!sprite) {
		SLOG(ska::LogLevel::Info) << "Le brouillard est inexistant sur cette map";
		m_fog.hide(true);
		return;
	}

	const auto xintensity = reader.get<int>("Fog xintensity");
	const auto yintensity = reader.get<int>("Fog yintensity");
	const auto number = reader.get<int>("Fog number");
	const auto transparency = reader.get<bool>("Fog transparency");

	const auto alpha = transparency ? WEATHER_ALPHA_LVL : 255;
	m_fog.load(reader.get<std::string>(fogSpritePath), number, 100, xintensity, yintensity, alpha);
	m_fog.hide(false);
}

void WorldImpl::loadWeatherFromData(const std::string& stringDataFile) {
	ska::IniReader reader(stringDataFile);
	const std::string& weatherSpritePath = "Weather sprite";
	// (An empty sprite : no weather)
	const auto sprite = reader.exists(weatherSpritePath) && !reader.get<std::string>(weatherSpritePath).empty();

	if(sprite) {
		const auto transparency = reader.get<bool>("Weather transparency");
		const auto xintensity = reader.get<int>("Weather xintensity");
		const auto yintensity = reader.get<int>("Weather yintensity");
		const auto number = reader.get<int>("Weather number");

		m_weather.load(reader.get<std::string>(weatherSpritePath), number, 100, xintensity, yintensity, transparency);
		m_weather.hide(false);
	} else {
		std::clog << "Le temps est inexistant sur cette map" << std::endl;
		m_weather.hide(true);
	}

}

bool WorldImpl::onSettingsChange(SettingsChangeEvent& sce) {
	if ((static_cast<int>(sce.eventType) & static_cast<int>(SettingsChangeEventType::WEATHER)) == static_cast<int>(SettingsChangeEventType::WEATHER)) {
		m_weather.hide(!sce.settings.getWeatherActive());
	}

	if ((static_cast<int>(sce.eventType) & static_cast<int>(SettingsChangeEventType::FOG)) == static_cast<int>(SettingsChangeEventType::FOG)) {
		m_fog.hide(!sce.settings.getFogActive());
	}

	return true;
}
