#include <list>
#include "WorldImpl.h"
#include "../WGameCore.h"
#include "../CustomEntityManager.h"
#include "../../ska/Graphic/Draw/DrawableContainer.h"
#include "../../ska/Physic/ParticleManager.h"
#include "../../Utils/IDs.h"
#include "../../ska/World/Layer.h"
#include "../../ska/World/LayerE.h"
#include "../../ska/Graphic/SpritePath.h"

#include "../../ska/Exceptions/CorruptedFileException.h"
#include "../../ska/Utils/StringUtils.h"
#include "../../ska/Utils/RectangleUtils.h"
#include "../../ska/Inputs/Readers/IniReader.h"

#define WEATHER_ALPHA_LVL 85

WorldImpl::WorldImpl(const unsigned int tailleBloc, const unsigned int wWidth, const unsigned int wHeight) :
ska::World(tailleBloc, wWidth, wHeight),
m_fog(*this),
m_weather(*this) {m_fog.setMosaicEffect(true);
	/* TODO organisation des "priorities" fixes dans un enum pour mettre de la cohérence */
	m_weather.setPriority(INT_MAX - 3);
	m_fog.setPriority(INT_MAX - 4);
	m_fog.resetPos();
	m_fog.setMosaicEffect(true);
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

void WorldImpl::load(std::string fileName, std::string chipsetName) {
	World::load(fileName, chipsetName);
	const string stringDataFile = "."FILE_SEPARATOR"Levels"FILE_SEPARATOR"" + getGenericName() + ""FILE_SEPARATOR"" + getGenericName() + ".ini";
	loadWeatherFromData(stringDataFile);
	loadFogFromData(stringDataFile);
}

void WorldImpl::changeLevel(std::string fileName, std::string chipsetName) {
	World::changeLevel(fileName, chipsetName);
	const string stringDataFile = "."FILE_SEPARATOR"Levels"FILE_SEPARATOR"" + getGenericName() + ""FILE_SEPARATOR"" + getGenericName() + ".ini";
	loadWeatherFromData(stringDataFile);
	loadFogFromData(stringDataFile);
}

void WorldImpl::loadFogFromData(const std::string& stringDataFile) {
	
	ska::IniReader reader(stringDataFile);

	int number, xintensity, yintensity;
	bool transparency;
	int alpha;

	const std::string sprite = reader.getString("Fog sprite");
	xintensity = reader.getInt("Fog xintensity");
	yintensity = reader.getInt("Fog yintensity");
	number = reader.getInt("Fog number");
	transparency = reader.getBoolean("Fog transparency");


	if(sprite == "STRINGNOTFOUND") {
		std::clog << "Le brouillard est inexistant sur cette map" << std::endl;
		m_fog.hide(true);
		return;
	}

	alpha = transparency ? WEATHER_ALPHA_LVL : 255;
	m_fog.load(sprite, number, 100, xintensity, yintensity, alpha);
	m_fog.hide(false);

}

void WorldImpl::loadWeatherFromData(const std::string& stringDataFile) {
	ska::IniReader reader(stringDataFile);
	const std::string sprite = reader.getString("Weather sprite");

	if(sprite != "STRINGNOTFOUND") {
		int number, xintensity, yintensity;
		bool transparency;
		transparency = reader.getBoolean("Weather transparency");
		xintensity = reader.getInt("Weather xintensity");
		yintensity = reader.getInt("Weather yintensity");
		number = reader.getInt("Weather number");	
	
		m_weather.load(sprite, number, 100, xintensity, yintensity, transparency);
		m_weather.hide(false);
	} else {
		std::clog << "Le temps est inexistant sur cette map" << std::endl;
		m_weather.hide(true);
	}

}

WorldImpl::~WorldImpl() {
}
