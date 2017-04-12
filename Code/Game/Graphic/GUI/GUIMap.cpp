#include <memory>
#include "GUIMap.h"
#include "../../Gameplay/Data/Settings.h"
#include "DialogMenu.h"
#include "WindowTeam.h"
#include "WindowSettings.h"
#include <GUI/Windows/GUIScrollButtonWindowIG.h>
#include <Core/Window.h>

GUIMap::GUIMap(ska::Window& w, ska::InputContextManager& playerICM, PokemonGameEventDispatcher& ged) :
	AbstractGameGUI(w, playerICM, ged),
	ska::Observer<SettingsChangeEvent>(std::bind(&GUIMap::onSettingsChange, this, std::placeholders::_1)),
	ska::Observer<EntityLoadEvent>(std::bind(&GUIMap::onEntityLoad, this, std::placeholders::_1)),
	DialogEventObserver(std::bind(&GUIMap::onDialogEvent, this, std::placeholders::_1)){

	initButtons(w);

	auto attachedToCursor = std::unique_ptr<ska::Widget>(std::make_unique<WindowMouseCursor>(this, this, this, ska::Rectangle{0, 0, 64, 96}, ska::Button::MENU_DEFAULT_THEME_PATH + "menu"));
	m_attachedToCursor = static_cast<WindowMouseCursor*>(addTopWidget(attachedToCursor));

	m_wMaster.addHandler<ska::HoverEventListener>([this](ska::Widget*, ska::HoverEvent& e) {
		m_attachedToCursor->move(e.getMousePosition() + ska::Point<int>(16, 16));
	});

	m_team = addWindow(std::make_unique<WindowTeam>(m_wMaster, m_attachedToCursor, ska::Point<int>(4 * TAILLEBLOCFENETRE, 4 * TAILLEBLOCFENETRE)), "team");
	m_team->show(false);
    //m_team->setPriority(0);

	auto wsettings = addWindow(std::make_unique<WindowSettings>(m_wMaster, ska::Point<int>(8 * TAILLEBLOCFENETRE, 4 * TAILLEBLOCFENETRE)), "settings");
    wsettings->show(false);
    //wsettings->setPriority(1);

	m_ged.ska::Observable<SettingsChangeEvent>::addObserver(*this);
	m_ged.ska::Observable<EntityLoadEvent>::addObserver(*this);
	m_ged.ska::Observable<DialogEvent>::addObserver(*this);
}

void GUIMap::bind(Settings& sets) {
	auto wsettings = reinterpret_cast<WindowSettings*>(getWindow("settings"));
	wsettings->bind(sets);
}

void GUIMap::initButtons(const ska::Window& w) {
	ska::Rectangle buf;
    buf.w = (TAILLEBLOCFENETRE)* 2;
    buf.h = (TAILLEBLOCFENETRE)* 2;
    buf.x = 0;
    buf.y = 0;

    //m_wAction->clear();
    m_wAction->move(ska::Point<int>(w.getWidth() - 13 * TAILLEBLOCFENETRE, w.getHeight() - m_wAction->getBox().h / 2));

    auto scrollButtonLambda = [this](ska::Widget* tthis, ska::HoverEvent& e) {
        auto target = static_cast<ska::GUIScrollButtonWindowIG*>(tthis);
        if (e.getState() == ska::MOUSE_OUT) {
            target->scrollRewind();
        } else if (e.getState() == ska::MOUSE_ENTER) {
            target->scrollTo(ska::Point<int>(target->getRelativePosition().x, -target->getBox().w / 3), 5);
        }

    };

    auto firstButton = std::make_unique<ska::GUIScrollButtonWindowIG>(*m_wAction, buf, ska::Button::MENU_DEFAULT_THEME_PATH + "menu");
    auto bs = std::make_unique<ska::ButtonSprite>(*firstButton, ska::Point<int>(1, 1), "", 102, [&](ska::Widget*, const ska::ClickEvent& e) {
        if (e.getState() == ska::MOUSE_CLICK) {
            auto widget = m_windowAnnuary["team"];
            widget->show(!widget->isVisible());
        }
    });
    bs->setName("POKEBALL BUTTON");
    firstButton->addWidget(std::move(bs));
    firstButton->setName("POKEBALL MENU");

    firstButton->addHandler<ska::HoverEventListener>(scrollButtonLambda);
    m_wAction->addWidget(std::move(firstButton));
    m_wAction->setName("ACTIONS");

    buf.x += 5 * TAILLEBLOCFENETRE / 2;
    //m_buttonList.push_back(DialogMenuPtr(new DialogMenu("", "." FILE_SEPARATOR "Sprites" FILE_SEPARATOR "Icones" FILE_SEPARATOR "pokedex.png", "." FILE_SEPARATOR "Menu" FILE_SEPARATOR "toolsmenu.png", buf, 22, false)));
    auto secondButton = std::make_unique<ska::GUIScrollButtonWindowIG>(*m_wAction, buf, ska::Button::MENU_DEFAULT_THEME_PATH + "menu");
    secondButton->addHandler<ska::HoverEventListener>(scrollButtonLambda);
    m_wAction->addWidget(std::move(secondButton));
    //m_buttonList[1]->name("Pokédex");
    //m_buttonList[1]->setActionClic("pokedex");

    buf.x += 5 * TAILLEBLOCFENETRE / 2;
    //m_buttonList.push_back(DialogMenuPtr(new DialogMenu("", "." FILE_SEPARATOR "Sprites" FILE_SEPARATOR "Icones" FILE_SEPARATOR "bag.png", "." FILE_SEPARATOR "Menu" FILE_SEPARATOR "toolsmenu.png", buf, 22, false)));
    auto thirdButton = std::make_unique<ska::GUIScrollButtonWindowIG>(*m_wAction, buf, ska::Button::MENU_DEFAULT_THEME_PATH + "menu");
    thirdButton->addHandler<ska::HoverEventListener>(scrollButtonLambda);
    m_wAction->addWidget(std::move(thirdButton));
    //m_buttonList[2]->name("PokéSac");
    /*m_buttonList[2]->setClickHandler([&] {
    m_wBag->setPos(ska::Point<int>(m_wTeam->getRect().w, 0));
    m_wBag->reset();
    m_wBag->hide(false);
    });*/

    buf.x += 5 * TAILLEBLOCFENETRE / 2;
    //m_buttonList.push_back(DialogMenuPtr(new DialogMenu("", "." FILE_SEPARATOR "Sprites" FILE_SEPARATOR "Icones" FILE_SEPARATOR "card.png", "." FILE_SEPARATOR "Menu" FILE_SEPARATOR "toolsmenu.png", buf, 22, false)));
    auto fourthButton = std::make_unique<ska::GUIScrollButtonWindowIG>(*m_wAction, buf, ska::Button::MENU_DEFAULT_THEME_PATH + "menu");
    fourthButton->addHandler<ska::HoverEventListener>(scrollButtonLambda);
    m_wAction->addWidget(std::move(fourthButton));
    //m_buttonList[3]->setActionClic("trainer_card");
    //m_buttonList[3]->name("Carte dresseur");

    buf.x += 5 * TAILLEBLOCFENETRE / 2;
    //m_buttonList.push_back(DialogMenuPtr(new DialogMenu("", "." FILE_SEPARATOR "Sprites" FILE_SEPARATOR "Icones" FILE_SEPARATOR "tape.png", "." FILE_SEPARATOR "Menu" FILE_SEPARATOR "toolsmenu.png", buf, 22, false)));
    auto fifthButton = std::make_unique<ska::GUIScrollButtonWindowIG>(*m_wAction, buf, ska::Button::MENU_DEFAULT_THEME_PATH + "menu");
    fifthButton->addHandler<ska::HoverEventListener>(scrollButtonLambda);
    std::unique_ptr<ska::ButtonSprite> bsFifth = std::make_unique<ska::ButtonSprite>(*fifthButton, ska::Point<int>(1, 1), "", 104, [&](ska::Widget*, const ska::ClickEvent& e) {
        if (e.getState() == ska::MOUSE_CLICK) {
            auto widget = m_windowAnnuary["settings"];
            widget->show(!widget->isVisible());
        }
    });
    fifthButton->addWidget(std::move(bsFifth));
    m_wAction->addWidget(std::move(fifthButton));

}

bool GUIMap::onDialogEvent(DialogEvent& de) {
    if(getWindow(de.name) == nullptr) {
        ska::Rectangle absoluteRect;
        absoluteRect.w = (de.box.w < 0) ? getMaxWidth() - 1 : de.box.w;
        absoluteRect.h = (de.box.h < 0) ? getMaxHeight() - 1 : de.box.h;
        absoluteRect.y = (de.box.y < 0) ? getMaxHeight() - absoluteRect.h - 1 : (de.box.y > getMaxHeight() ? getMaxHeight() : de.box.y);
        absoluteRect.x = (de.box.x < 0) ? getMaxWidth() - absoluteRect.w - 1 : (de.box.x > getMaxWidth() ? getMaxWidth() : de.box.x);

        auto dialogWindow = addWindow(std::make_unique<DialogMenu>(*this, *this, *this, de.message, de.name, absoluteRect, 12), de.name);
        if(de.scroll) {
            dialogWindow->scrollTo(ska::Point<int>(absoluteRect.x, absoluteRect.y - absoluteRect.h - 1), 8);

            dialogWindow->addHandler<ska::KeyEventListener>([this] (ska::Widget* tthis, ska::KeyEvent& ke) {
                if(ke.getScanCode() == SDL_SCANCODE_RETURN) {
                    auto target = static_cast<DialogMenu*>(tthis);
                    target->scrollTo(ska::Point<int>(target->getRelativePosition().x, getMaxHeight() + 1), 8)
                    .then([this](ska::TimeScrollableWindowIG<ska::KeyEventListener>& caller) {
                          ska::GUIEvent ge(ska::GUIEventType::REMOVE_WINDOW);
                          ge.windowName = static_cast<DialogMenu&>(caller).getName();
                          m_ged.ska::Observable<ska::GUIEvent>::notifyObservers(ge);
                    });
                }
            });
        }
    }
    return true;
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
    m_ged.ska::Observable<DialogEvent>::removeObserver(*this);
	m_ged.ska::Observable<SettingsChangeEvent>::removeObserver(*this);
	m_ged.ska::Observable<EntityLoadEvent>::removeObserver(*this);
}

