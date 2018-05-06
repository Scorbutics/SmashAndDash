#include "../Data/Settings.h"
#include "WorldImpl.h"
#include "Draw/DrawableContainer.h"
#include "Inputs/Readers/IniReader.h"
#include "Utils/FileUtils.h"
#include "WorldLoader.h"

#define WEATHER_ALPHA_LVL 85

WorldImpl::WorldImpl(PokemonGameEventDispatcher& ged, ska::Tileset& tileset, const ska::TileWorldLoader& loader) :
	TileWorld(ged, tileset, loader),
	SubObserver<SettingsChangeEvent>(bind(&WorldImpl::onSettingsChange, this, std::placeholders::_1), ged) ,
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
}

void WorldImpl::loadFogFromData(const std::string& stringDataFile) {
	ska::IniReader reader(stringDataFile);

	const std::string& fogSpritePath = "Fog sprite";
	const auto sprite = reader.exists(fogSpritePath);
	const auto xintensity = reader.get<int>("Fog xintensity");
	const auto yintensity = reader.get<int>("Fog yintensity");
	const auto number = reader.get<int>("Fog number");
	const auto transparency = reader.get<bool>("Fog transparency");

	if(!sprite) {
		SKA_LOG_MESSAGE("Le brouillard est inexistant sur cette map");
		m_fog.hide(true);
		return;
	}

	const auto alpha = transparency ? WEATHER_ALPHA_LVL : 255;
	m_fog.load(reader.get<std::string>(fogSpritePath), number, 100, xintensity, yintensity, alpha);
	m_fog.hide(false);
}

void WorldImpl::loadWeatherFromData(const std::string& stringDataFile) {
	ska::IniReader reader(stringDataFile);
	const std::string& weatherSpritePath = "Weather sprite";
	const auto sprite = reader.exists(weatherSpritePath);

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
	if ((sce.eventType & SettingsChangeEventType::WEATHER) == SettingsChangeEventType::WEATHER) {
		m_weather.hide(!sce.settings.getWeatherActive());
	}

	if ((sce.eventType & SettingsChangeEventType::FOG) == SettingsChangeEventType::FOG) {
		m_fog.hide(!sce.settings.getFogActive());
	}

	return true;
}
