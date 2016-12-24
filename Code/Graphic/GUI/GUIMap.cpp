#include <memory>
#include <limits>
#include "GUIMap.h"
#include "../../Gameplay/Data/Settings.h"
#include "WindowTeam.h"
#include "WindowSettings.h"

GUIMap::GUIMap(ska::Window& w, ska::InputContextManager& playerICM, PokemonGameEventDispatcher& ged) : 
	AbstractGameGUI(w, playerICM, ged),
	ska::Observer<SettingsChangeEvent>(std::bind(&GUIMap::onSettingsChange, this, std::placeholders::_1)) {

	auto attachedToCursor = std::unique_ptr<ska::Widget>(new WindowMouseCursor(this, this, ska::Rectangle{ 0, 0, 64, 96 }, ska::Button::MENU_DEFAULT_THEME_PATH + "menu"));
	m_attachedToCursor = static_cast<WindowMouseCursor*>(addTopWidget(attachedToCursor));

	m_wMaster.addHandler<ska::HoverEventListener>([this](ska::Widget* tthis, ska::HoverEvent& e) {
		m_attachedToCursor->move(e.getMousePosition() + ska::Point<int>(16, 16));
	});

	auto rawWindowTeam = new WindowTeam(m_wMaster, m_attachedToCursor, ska::Point<int>(4 * TAILLEBLOCFENETRE, 4 * TAILLEBLOCFENETRE));
	m_team = rawWindowTeam;
	rawWindowTeam->show(false);
	/* Data Set Test */
	auto spd1 = std::make_unique<SlotPokemonData>();
	spd1->id = 25;
	spd1->hp = "25/25";
	spd1->name = "Pikachu";
	spd1->level = "5";
	spd1->type1 = "ELCTK";
	spd1->type2 = "";
	auto spd2 = std::make_unique<SlotPokemonData>();
	spd2->id = 1;
	spd2->hp = "20/20";
	spd2->name = "Bulbizarre";
	spd2->level = "4";
	spd2->type1 = "PLT";
	spd2->type2 = "";

	auto slot1 = rawWindowTeam->insertPokemon(nullptr, std::move(spd1));
	rawWindowTeam->insertPokemon(slot1, std::move(spd2));
	/* End Data Set Test */

	auto windowTeam = std::unique_ptr<WindowTeam>(rawWindowTeam);
	addWindow<WindowTeam, ska::ValueChangedEventListener<int>>(windowTeam, "team");

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

