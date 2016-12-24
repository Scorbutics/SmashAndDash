#include <memory>
#include <limits>
#include "GUIMap.h"
#include "../../Gameplay/Data/Settings.h"
#include "WindowTeam.h"
#include "WindowSettings.h"

GUIMap::GUIMap(ska::Window& w, ska::InputContextManager& playerICM, PokemonGameEventDispatcher& ged) : 
	AbstractGameGUI(w, playerICM, ged),
	ska::Observer<SettingsChangeEvent>(std::bind(&GUIMap::onSettingsChange, this, std::placeholders::_1)) {

	auto rawWindowTeam = new WindowTeam(m_wMaster, ska::Point<int>(4 * TAILLEBLOCFENETRE, 4 * TAILLEBLOCFENETRE));
	m_team = rawWindowTeam;
	rawWindowTeam->show(false);
	rawWindowTeam->loadPokemon(0, 25);
	rawWindowTeam->loadPokemon(1, 25);
	rawWindowTeam->addHandler<ska::ValueChangedEventListener<SlotPokemonData>>([this](ska::Widget* tthis, ska::ValueChangedEvent<SlotPokemonData>& vce) {
		auto lastPokemonId = m_attachedToCursor->loadPokemon((unsigned int)vce.getValue().id);
		if (m_attachedToCursor->isVisible()) {
			m_team->loadPokemon(vce.getValue().slotNumber, lastPokemonId);
			m_attachedToCursor->unloadPokemon();
		} else {
			m_team->unloadPokemon(vce.getValue().slotNumber);
			m_attachedToCursor->show(true);
		}
	});

	auto windowTeam = std::unique_ptr<WindowTeam>(rawWindowTeam);
	addWindow<WindowTeam, ska::ValueChangedEventListener<int>>(windowTeam, "team");

	auto rawWindowSettings = new WindowSettings(m_wMaster, ska::Point<int>(8 * TAILLEBLOCFENETRE, 4 * TAILLEBLOCFENETRE));
	rawWindowSettings->show(false);

	auto windowSettings = std::unique_ptr<WindowSettings>(rawWindowSettings);
	addWindow<WindowSettings, ska::KeyEventListener>(windowSettings, "settings");

	auto attachedToCursor = std::unique_ptr<ska::Widget>(new WindowMouseCursor(this, this, ska::Rectangle{ 0, 0, 64, 96 }, ska::Button::MENU_DEFAULT_THEME_PATH + "menu"));
	m_attachedToCursor = static_cast<WindowMouseCursor*>(addTopWidget(attachedToCursor));

	m_wMaster.addHandler<ska::HoverEventListener>([this](ska::Widget* tthis, ska::HoverEvent& e) {
		m_attachedToCursor->move(e.getMousePosition() + ska::Point<int>(16, 16));
	});

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

