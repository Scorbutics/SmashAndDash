#include <memory>
#include "GUIMap.h"
#include "../../Gameplay/Data/Settings.h"
#include "WindowTeam.h"
#include "WindowSettings.h"

GUIMap::GUIMap(ska::Window& w, ska::InputContextManager& playerICM, PokemonGameEventDispatcher& ged) : 
	AbstractGameGUI(w, playerICM, ged),
	ska::Observer<SettingsChangeEvent>(std::bind(&GUIMap::onSettingsChange, this, std::placeholders::_1)) {

	auto rawWindowTeam = new WindowTeam(m_wMaster, ska::Point<int>(4 * TAILLEBLOCFENETRE, 4 * TAILLEBLOCFENETRE));
	rawWindowTeam->show(false);
	rawWindowTeam->addPokemon(25);

	auto windowTeam = std::unique_ptr<WindowTeam>(rawWindowTeam);
	addWindow<WindowTeam> (windowTeam, "team");


	auto rawWindowSettings = new WindowSettings(m_wMaster, ska::Point<int>(8 * TAILLEBLOCFENETRE, 4 * TAILLEBLOCFENETRE));
	rawWindowSettings->show(false);

	auto windowSettings = std::unique_ptr<WindowSettings>(rawWindowSettings);
	addWindow<WindowSettings, ska::KeyEventListener>(windowSettings, "settings");

	m_ged.ska::Observable<SettingsChangeEvent>::addObserver(*this);
}

void GUIMap::bind(Settings& sets) {
	auto wsettings = reinterpret_cast<WindowSettings*>(getWindow("settings"));
	wsettings->bind(sets);
}

bool GUIMap::onSettingsChange(SettingsChangeEvent& sce) {
	auto wsettings = reinterpret_cast<WindowSettings*>(getWindow("settings"));

	if((sce.eventType & SettingsChangeEventType::MUSIC_VOLUME) == SettingsChangeEventType::MUSIC_VOLUME) {	
		wsettings->setMusicVolume(sce.settings.getSoundVolume()/100);
	}

	if ((sce.eventType & SettingsChangeEventType::FOG) == SettingsChangeEventType::FOG) {
		wsettings->setFog(sce.settings.getFogActive());
	}

	if ((sce.eventType & SettingsChangeEventType::WEATHER) == SettingsChangeEventType::WEATHER) {
		wsettings->setWeather(sce.settings.getWeatherActive());
	}

	return true;
}

GUIMap::~GUIMap() {
	m_ged.ska::Observable<SettingsChangeEvent>::removeObserver(*this);
}

