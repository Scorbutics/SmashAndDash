#include "../Data/Settings.h"
#include "WorldImpl.h"
#include "../CustomEntityManager.h"
#include "Draw/DrawableContainer.h"
#include "Physic/ParticleManager.h"
#include "../../Utils/IDs.h"
#include "World/Layer.h"
#include "World/LayerE.h"
#include "Utils/SpritePath.h"

#include "Exceptions/CorruptedFileException.h"
#include "Utils/StringUtils.h"
#include "Utils/RectangleUtils.h"
#include "Inputs/Readers/IniReader.h"

#define WEATHER_ALPHA_LVL 85

WorldImpl::WorldImpl(PokemonGameEventDispatcher& ged, unsigned int tailleBloc, const unsigned int wWidth, const unsigned int wHeight) :
World(tailleBloc, wWidth, wHeight),
Observer<SettingsChangeEvent>(bind(&WorldImpl::onSettingsChange, this, std::placeholders::_1)),
m_fog(*this),
m_weather(*this),
m_ged(ged) {
	m_fog.setMosaicEffect(true);
	/* TODO organisation des "priorities" fixes dans un enum pour mettre de la cohérence */
	m_weather.setPriority(INT_MAX - 3);
	m_fog.setPriority(INT_MAX - 4);
	m_fog.resetPos();
	m_fog.setMosaicEffect(true);

	m_ged.ska::Observable<SettingsChangeEvent>::addObserver(*this);
}

void WorldImpl::graphicUpdate(ska::DrawableContainer& drawables) {

	//Affichage des effets
	/*ska::ParticleManager& particleManager = wScreen.getParticleManager();
	drawables.addHead(particleManager);

	//Curseur souris sur la map
	drawables.addHead(wScreen.getMouseCursor());*/

	/* Météo */
	drawables.add(m_weather);
	drawables.add(m_fog);
}

void WorldImpl::load(const std::string& fileName, const std::string& chipsetName) {
	if (getFileName() == fileName && chipsetName == m_chipset.getName()) {
		return;
	}

	World::load(fileName, chipsetName);
	const std::string& stringDataFile = "." FILE_SEPARATOR "Levels" FILE_SEPARATOR "" + getGenericName() + "" FILE_SEPARATOR "" + getGenericName() + ".ini";
	loadWeatherFromData(stringDataFile);
	loadFogFromData(stringDataFile);
}

void WorldImpl::loadFogFromData(const std::string& stringDataFile) {

	ska::IniReader reader(stringDataFile);

	int number, xintensity, yintensity;
	bool transparency;
	int alpha;

	const std::string& fogSpritePath = "Fog sprite";
	const bool sprite = reader.exists(fogSpritePath);
	xintensity = reader.get<int>("Fog xintensity");
	yintensity = reader.get<int>("Fog yintensity");
	number = reader.get<int>("Fog number");
	transparency = reader.get<bool>("Fog transparency");


	if(!sprite) {
		std::clog << "Le brouillard est inexistant sur cette map" << std::endl;
		m_fog.hide(true);
		return;
	}

	alpha = transparency ? WEATHER_ALPHA_LVL : 255;
	m_fog.load(reader.get<std::string>(fogSpritePath), number, 100, xintensity, yintensity, alpha);
	m_fog.hide(false);

}

void WorldImpl::loadWeatherFromData(const std::string& stringDataFile) {
	ska::IniReader reader(stringDataFile);
	const std::string& weatherSpritePath = "Weather sprite";
	const bool sprite = reader.exists(weatherSpritePath);

	if(sprite) {
		int number, xintensity, yintensity;
		bool transparency;
		transparency = reader.get<bool>("Weather transparency");
		xintensity = reader.get<int>("Weather xintensity");
		yintensity = reader.get<int>("Weather yintensity");
		number = reader.get<int>("Weather number");

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

WorldImpl::~WorldImpl() {
	m_ged.ska::Observable<SettingsChangeEvent>::removeObserver(*this);
}
