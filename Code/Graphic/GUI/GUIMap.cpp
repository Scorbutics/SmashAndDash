#include <memory>
#include <limits>
#include "GUIMap.h"
#include "../../Gameplay/Data/Settings.h"
#include "WindowTeam.h"
#include "WindowSettings.h"

GUIMap::GUIMap(ska::Window& w, ska::InputContextManager& playerICM, PokemonGameEventDispatcher& ged) : 
	AbstractGameGUI(w, playerICM, ged),
	ska::Observer<SettingsChangeEvent>(std::bind(&GUIMap::onSettingsChange, this, std::placeholders::_1)),
	ska::Observer<EntityLoadEvent>(std::bind(&GUIMap::onEntityLoad, this, std::placeholders::_1)){

	auto attachedToCursor = std::unique_ptr<ska::Widget>(new WindowMouseCursor(this, this, ska::Rectangle{ 0, 0, 64, 96 }, ska::Button::MENU_DEFAULT_THEME_PATH + "menu"));
	m_attachedToCursor = static_cast<WindowMouseCursor*>(addTopWidget(attachedToCursor));

	m_wMaster.addHandler<ska::HoverEventListener>([this](ska::Widget*, ska::HoverEvent& e) {
		m_attachedToCursor->move(e.getMousePosition() + ska::Point<int>(16, 16));
	});

	auto rawWindowTeam = new WindowTeam(m_wMaster, m_attachedToCursor, ska::Point<int>(4 * TAILLEBLOCFENETRE, 4 * TAILLEBLOCFENETRE));
	m_team = rawWindowTeam;
	rawWindowTeam->show(false);
	addWindow<WindowTeam, ska::ValueChangedEventListener<int>>(std::unique_ptr<WindowTeam>(rawWindowTeam), "team");

	auto rawWindowSettings = new WindowSettings(m_wMaster, ska::Point<int>(8 * TAILLEBLOCFENETRE, 4 * TAILLEBLOCFENETRE));
	rawWindowSettings->show(false);
	addWindow<WindowSettings, ska::KeyEventListener>(std::unique_ptr<WindowSettings>(rawWindowSettings), "settings");

	m_ged.ska::Observable<SettingsChangeEvent>::addObserver(*this);
	m_ged.ska::Observable<EntityLoadEvent>::addObserver(*this);
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

bool GUIMap::onEntityLoad(EntityLoadEvent& ele) {
	/* add a Pokemon to the GUI team (from the event) */
	auto& wTeam = static_cast<WindowTeam&>(*getWindow("team"));

	auto spd1 = std::make_unique<SlotPokemonData>();
	/* TODO data from Pkmn Database */
	spd1->id = ele.id;
	spd1->hp = ska::StringUtils::uintToStr(ele.currentHp);
	spd1->name = ele.description->name;
	spd1->level = ska::StringUtils::intToStr(ele.stats->getLevel());
	spd1->type1 = ele.description->type1;
	spd1->type2 = ele.description->type2;

	wTeam.insertPokemon(nullptr, move(spd1));
	
	return true;
}

GUIMap::~GUIMap() {
	m_ged.ska::Observable<SettingsChangeEvent>::removeObserver(*this);
	m_ged.ska::Observable<EntityLoadEvent>::removeObserver(*this);
}

