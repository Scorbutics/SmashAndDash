#include <memory>
#include <SDL3/SDL.h>
#include "Game/__internalConfig/LoggerConfig.h"
#include "GUIMap.h"
#include "WindowBag.h"
#include "../../Gameplay/Data/Settings.h"
#include "DialogMenu.h"
#include "WindowTeam.h"
#include "WindowSettings.h"
#include <GUI/Windows/GUIScrollButtonWindowIG.h>
#include "FPSLabel.h"
#include "../../Gameplay/Fight/SkillsHolderComponent.h"
#include "../../Gameplay/CustomEntityManager.h"

#define TAILLEBLOCFENETRE 32

GUIMap::GUIMap(CustomEntityManager& em, PokemonGameEventDispatcher& ged) :
	ska::GUI(ged),
	ska::Observer<SettingsChangeEvent>(std::bind(&GUIMap::onSettingsChange, this, std::placeholders::_1)),
	ska::Observer<EntityLoadEvent>(std::bind(&GUIMap::onEntityLoad, this, std::placeholders::_1)),
	DialogEventObserver(std::bind(&GUIMap::onDialogEvent, this, std::placeholders::_1)),
	ska::Observer<InterfaceEvent>(std::bind(&GUIMap::onInterfaceEvent, this, std::placeholders::_1)),
	ska::Observer<BattleEvent>(std::bind(&GUIMap::onBattleEvent, this, std::placeholders::_1)),
	ska::Observer<InventoryEvent>(std::bind(&GUIMap::onInventoryEvent, this, std::placeholders::_1)),
	m_entityManager(em),
	m_ged(ged),
	m_dbgWindow(*this, ged) {
	initButtons();

	auto attachedToCursor = std::unique_ptr<ska::Widget>(std::make_unique<WindowMouseCursor>(this, this, this, ska::Rectangle{0, 0, 64, 96}));
	m_attachedToCursor = static_cast<WindowMouseCursor*>(addTopWidget(attachedToCursor));

	addMasterHandler<ska::HoverEventListener>([this](ska::Widget*, ska::HoverEvent& e) {
		m_attachedToCursor->move(e.getMousePosition() + ska::Point<int>(16, 16));
	});

	m_team = &addFocusableWindow<WindowTeam>("team", m_attachedToCursor, ska::Point<int>(4 * TAILLEBLOCFENETRE, 4 * TAILLEBLOCFENETRE));
	m_team->show(false);
	m_bag = &addFocusableWindow<WindowBag>("bag", ska::Point<int>(10 * TAILLEBLOCFENETRE, 4 * TAILLEBLOCFENETRE));
	m_bag->show(false);

	m_settings = &addFocusableWindow<WindowSettings>("settings", ska::Point<int>(12 * TAILLEBLOCFENETRE, 4 * TAILLEBLOCFENETRE));
	m_settings->show(false);

	m_ged.ska::Observable<SettingsChangeEvent>::addObserver(*this);
	m_ged.ska::Observable<EntityLoadEvent>::addObserver(*this);
	m_ged.ska::Observable<DialogEvent>::addObserver(*this);
	m_ged.ska::Observable<BattleEvent>::addObserver(*this);
	m_ged.ska::Observable<InterfaceEvent>::addObserver(*this);
	m_ged.ska::Observable<InventoryEvent>::addObserver(*this);
}

void GUIMap::refresh(unsigned int ellapsedTime) {
	GUI::refresh(ellapsedTime);
	m_dbgWindow.refresh(ellapsedTime);
	// The dialog closed : no other came in its box, it scrolls out
	if (m_closedDialog.has_value()) {
		m_closedDialog->time += ellapsedTime;
		if (m_closedDialog->time >= DialogChainDelay) {
			scrollOutDialog(m_closedDialog->name);
			m_closedDialog.reset();
		}
	}
}

void GUIMap::bind(Settings& sets) {
	auto wsettings = m_settings;
	wsettings->bind(sets);
}

void GUIMap::initButtons() {
	ska::Rectangle buf;
    buf.w = (TAILLEBLOCFENETRE)* 2;
    buf.h = (TAILLEBLOCFENETRE)* 4;
    buf.x = 0;
    buf.y = 0;

	//TODO widget dans classe à part
	m_wAction = &addWindow<ska::TimeScrollableWindowIG<>>("actions", ska::Rectangle{ 0, 0, 13 * TAILLEBLOCFENETRE, 2 * TAILLEBLOCFENETRE });
	m_wAction->setPriority(0);

    auto& wAction = static_cast<ska::TimeScrollableWindowIG<>&>(*m_wAction);
    wAction.move(ska::Point<int>(getMaxWidth() - 13 * TAILLEBLOCFENETRE, getMaxHeight() - wAction.getBox().h / 2));

    auto scrollButtonLambda = [this](ska::Widget* tthis, ska::HoverEvent& e) {
        auto target = static_cast<ska::GUIScrollButtonWindowIG*>(tthis);
        if (e.getState() == ska::MOUSE_OUT) {
            target->scrollRewind();
        } else if (e.getState() == ska::MOUSE_ENTER) {
            target->scrollTo(ska::Point<int>(target->getRelativePosition().x, -target->getBox().w / 3), 5);
        }

    };

    auto& firstButton = wAction.addWidget<ska::GUIScrollButtonWindowIG>(buf);
	auto& bs = firstButton.addWidget<ska::ButtonSprite>(ska::Point<int>(), "", 102, [&](ska::Widget*, const ska::ClickEvent& e) {
        if (e.getState() == ska::MOUSE_CLICK) {
            auto widget = getWindow("team");
            widget->show(!widget->isVisible());
        }
    });

    firstButton.addHandler<ska::HoverEventListener>(scrollButtonLambda);

	static constexpr auto OFFSET_BETWEEN_BUTTONS = 2.5 * TAILLEBLOCFENETRE;

    buf.x += OFFSET_BETWEEN_BUTTONS;
    auto& secondButton = wAction.addWidget<ska::GUIScrollButtonWindowIG>(buf);
    secondButton.addHandler<ska::HoverEventListener>(scrollButtonLambda);
	// The bag
	secondButton.addWidget<ska::ButtonSprite>(ska::Point<int>(1, 1), "", 100, [&](ska::Widget*, const ska::ClickEvent& e) {
        if (e.getState() == ska::MOUSE_CLICK) {
            m_bag->show(!m_bag->isVisible());
        }
    });

	buf.x += OFFSET_BETWEEN_BUTTONS;
    auto& thirdButton = wAction.addWidget<ska::GUIScrollButtonWindowIG>(buf);
    thirdButton.addHandler<ska::HoverEventListener>(scrollButtonLambda);

	buf.x += OFFSET_BETWEEN_BUTTONS;
    auto& fourthButton = wAction.addWidget<ska::GUIScrollButtonWindowIG>(buf);
    fourthButton.addHandler<ska::HoverEventListener>(scrollButtonLambda);

	buf.x += OFFSET_BETWEEN_BUTTONS;
    auto& fifthButton = wAction.addWidget<ska::GUIScrollButtonWindowIG>(buf);
    fifthButton.addHandler<ska::HoverEventListener>(scrollButtonLambda);
	fifthButton.addWidget<ska::ButtonSprite>(ska::Point<int>(1, 1), "", 104, [&](ska::Widget*, const ska::ClickEvent& e) {
        if (e.getState() == ska::MOUSE_CLICK) {
            auto widget = getWindow("settings");
            widget->show(!widget->isVisible());
        }
    });
}

bool GUIMap::onInventoryEvent(InventoryEvent& ie) {
	if (m_bag != nullptr) {
		m_bag->refresh(ie);
	}
	return true;
}

bool GUIMap::onInterfaceEvent(InterfaceEvent& ie) {
	// The buttons come back with the interface, the windows opened by the buttons stay closed
	m_wAction->show(!ie.hidden);
	if (ie.hidden) {
		m_team->show(false);
		m_bag->show(false);
		if (auto* settings = getWindow("settings"); settings != nullptr) {
			settings->show(false);
		}
	}
	return true;
}

bool GUIMap::onDialogEvent(DialogEvent& de) {
    if(getWindow(de.name) == nullptr) {
        ska::Rectangle absoluteRect;
        absoluteRect.w = (de.box.w < 0) ? getMaxWidth() - 1 : de.box.w;
        absoluteRect.h = (de.box.h < 0) ? getMaxHeight() - 1 : de.box.h;
		const auto maxLimitY = de.box.y > static_cast<int>(getMaxHeight());
		const auto maxLimitX = de.box.x > static_cast<int>(getMaxHeight());
        absoluteRect.y = (de.box.y < 0) ? getMaxHeight() - absoluteRect.h - 1 : (maxLimitY ? (getMaxHeight() - absoluteRect.h) : de.box.y);
        absoluteRect.x = (de.box.x < 0) ? getMaxWidth() - absoluteRect.w - 1 : (maxLimitX ? (getMaxWidth() - absoluteRect.w) : de.box.x);

		// The dialog closed just before, in the same box : this one takes its place, without scrolling (out then in)
		auto chained = false;
		if (m_closedDialog.has_value()) {
			const auto& box = m_closedDialog->box;
			chained = de.scroll && box.x == absoluteRect.x && box.y == absoluteRect.y && box.w == absoluteRect.w && box.h == absoluteRect.h;
			if (chained) {
				if (auto* previous = getWindow(m_closedDialog->name); previous != nullptr) {
					previous->show(false);
				}
				ska::GUIEvent ge(ska::GUIEventType::REMOVE_WINDOW);
				ge.windowName = m_closedDialog->name;
				m_ged.ska::Observable<ska::GUIEvent>::notifyObservers(ge);
			} else {
				scrollOutDialog(m_closedDialog->name);
			}
			m_closedDialog.reset();
		}

		auto startRect = absoluteRect;
		if (!chained) {
			startRect.y += startRect.h - 1;
		}
        auto& dialogWindow = addFocusableWindow<DialogMenu>(de.name, de.message, de.name, startRect, DialogFontSize);
		
		m_dialogScript = de.callbackActive ? de.callback : std::optional<ska::ScriptSleepComponent>{};
		// (Each dialog has its own close callback : several scripts can show a dialog)
		auto dialogClosed = de.onClose;
		const auto dialogQuestion = de.question;

        if (chained) {
			listenDialogKeys(dialogWindow, absoluteRect, dialogClosed, dialogQuestion);
		} else if(de.scroll) {
            dialogWindow.scrollTo(ska::Point<int>(absoluteRect.x, absoluteRect.y), 8)
            .then([this, absoluteRect, dialogClosed, dialogQuestion](ska::TimeScrollableWindowIG<ska::KeyEventListener>& caller) {
				listenDialogKeys(static_cast<DialogMenu&>(caller), absoluteRect, dialogClosed, dialogQuestion);
            });
        }
    }
    return true;
}

void GUIMap::listenDialogKeys(DialogMenu& dialog, const ska::Rectangle box, std::function<void(bool)> onClose, const bool question) {
	// The dialog is closed once (a key pressed again while it waits or scrolls out is ignored)
	auto closing = std::make_shared<bool>(false);
	dialog.addHandler<ska::KeyEventListener>([this, closing, box, onClose, question] (ska::Widget* tthis, ska::KeyEvent& ke) {
		const auto validated = ke.getScanCode() == SDL_SCANCODE_RETURN;
		const auto refused = question && ke.getScanCode() == SDL_SCANCODE_BACKSPACE;
		if (*closing || !(validated || refused)) {
			return;
		}
		*closing = true;
		// It stays a moment (the next dialog, if any, replaces it) ; then it scrolls out (refresh)
		if (m_closedDialog.has_value()) {
			scrollOutDialog(m_closedDialog->name);
		}
		m_closedDialog = ClosedDialog{ static_cast<DialogMenu*>(tthis)->getName(), box, 0 };

		// The scripts go on now : the next dialog comes while this one is still shown
		// (Copied : the script can show a dialog, which removes this window and this function)
		const auto closed = onClose;
		auto script = std::move(m_dialogScript);
		m_dialogScript.reset();
		if (closed) {
			closed(validated);
		}
		if (script.has_value()) {
			/* triggers callback */
			const auto scriptEntity = m_entityManager.createEntity();
			auto events = std::vector<ska::ScriptEventType>();
			events.push_back(ska::ScriptEventType::ScriptCreate);
			auto se = ska::ScriptEvent{ std::move(events), std::move(*script), scriptEntity, {}, {}, {}, {} };
			m_ged.Observable<ska::ScriptEvent>::notifyObservers(se);
		}
	});
}

void GUIMap::scrollOutDialog(const std::string& name) {
	auto* window = getWindow(name);
	if (window == nullptr) {
		return;
	}
	auto& dialog = static_cast<DialogMenu&>(*window);
	dialog.scrollTo(ska::Point<int>(dialog.getRelativePosition().x, getMaxHeight() + 1), 8)
	.then([this](ska::TimeScrollableWindowIG<ska::KeyEventListener>& caller) {
		ska::GUIEvent ge(ska::GUIEventType::REMOVE_WINDOW);
		ge.windowName = static_cast<DialogMenu&>(caller).getName();
		m_ged.ska::Observable<ska::GUIEvent>::notifyObservers(ge);
	});
}

bool GUIMap::onScreenResized(unsigned int width, unsigned int height) {
	auto& wAction = static_cast<ska::TimeScrollableWindowIG<>&>(*m_wAction);
	wAction.move(ska::Point<int>(width - 13 * TAILLEBLOCFENETRE, height - wAction.getBox().h / 2));

	return true;
}

bool GUIMap::onSettingsChange(SettingsChangeEvent& sce) {
	auto wsettings = m_settings;

	if((static_cast<int>(sce.eventType) & static_cast<int>(SettingsChangeEventType::MUSIC_VOLUME)) == static_cast<int>(SettingsChangeEventType::MUSIC_VOLUME)) {
		wsettings->setMusicVolume(sce.settings.getSoundVolume()/100);
	}

	if ((static_cast<int>(sce.eventType) & static_cast<int>(SettingsChangeEventType::FOG)) == static_cast<int>(SettingsChangeEventType::FOG)) {
		wsettings->setFog(sce.settings.getFogActive());
	}

	if ((static_cast<int>(sce.eventType) & static_cast<int>(SettingsChangeEventType::WEATHER)) == static_cast<int>(SettingsChangeEventType::WEATHER)) {
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

bool GUIMap::onBattleEvent(BattleEvent& be) {
	m_dbgWindow.show(be.type != BATTLE_START);
	return true;
}

GUIMap::~GUIMap() {
	m_ged.ska::Observable<DialogEvent>::removeObserver(*this);
	m_ged.ska::Observable<InterfaceEvent>::removeObserver(*this);
	m_ged.ska::Observable<BattleEvent>::removeObserver(*this);
	m_ged.ska::Observable<InventoryEvent>::removeObserver(*this);
	m_ged.ska::Observable<SettingsChangeEvent>::removeObserver(*this);
	m_ged.ska::Observable<EntityLoadEvent>::removeObserver(*this);
}

