#include "Game/__internalConfig/LoggerConfig.h"
#include "WGameCore.h"
#include "State/StateMap.h"
#include "GUI/MessageType.h"
#include "GUI/MessagePopup.h"
#include "Core/Exceptions/FileException.h"
#include "Graphic/SDLRenderer.h"
#include "CoreModule.h"
#include "GraphicModule.h"
#include "SoundModule.h"
#include "World/Inputs/KeyboardInputMapContext.h"
#include "World/Inputs/KeyboardInputGUIContext.h"
#include "Graphic/SDLWindow.h"
#include "Core/Draw/VectorDrawableContainer.h"
#include "Game/Gameplay/Data/SavePaths.h"
#include "Game/Gameplay/Data/SaveSlots.h"
#include "State/StateTitle.h"

WGameCore::WGameCore(CustomEntityManager& em, GameConfPtr&& gc):
	GameCore(std::forward<GameConfPtr>(gc)),
    m_settings(m_eventDispatcher, "gamesettings.ini"),
	m_entityManager(em),
	ska::SubObserver<MapEvent>(std::bind(&WGameCore::onTeleport, this, std::placeholders::_1), m_eventDispatcher),
	ska::SubObserver<ska::StateEvent>(std::bind(&WGameCore::onStateEvent, this, std::placeholders::_1), m_eventDispatcher),
	ska::SubObserver<ska::GameEvent>(std::bind(&WGameCore::onGameEvent, this, std::placeholders::_1), m_eventDispatcher) {

	SLOG(ska::LogLevel::Info) << "Game initialization";

	// The title screen ; a save given by the environment (SAD_SAVE : the agent runs) is played at once (its world
	// generated before, on the loading screen, when it is to generate)
	const auto direct = EnvironmentSaveName();
	if (!direct.empty() && SaveSlots::Exists(direct) && !SaveSlots::WorldToGenerate(direct)) {
		startWorld(direct);
	} else {
		m_titleShown = true;
		navigateToState<StateTitle>(m_eventDispatcher, [this](const std::string& save) { startWorld(save); }, direct);
	}
}

void WGameCore::startWorld(const std::string& save) {
	SLOG(ska::LogLevel::Info) << "Game : the save " << save;
	ChosenSaveRef() = save;
	m_worldPending = m_titleShown;
	m_worldState = &navigateToState<WorldState>(m_entityManager, m_eventDispatcher, m_settings);

	const auto& startMapName = m_worldState->getSaveGame().getStartMapName();
	const auto pathStartMapName = MapDirectory(startMapName);
	m_currentState = &static_cast<StateMap&>(m_worldState->addSubState(std::make_unique<StateMap>(m_entityManager, m_eventDispatcher, *m_worldState, pathStartMapName, m_worldState->getSaveGame().getStartChipsetName())));

	auto& guiState = static_cast<StateGUIMap&>(m_worldState->addSubState(std::make_unique<StateGUIMap>(m_entityManager, m_eventDispatcher)));
	guiState.bindGUI(m_settings);

	m_settings.load();
}

bool WGameCore::onStateEvent(ska::StateEvent& se) {
	// The world after the title screen : the window is ready for it too (the engine says it to the first state only)
	if (se.type == ska::StateEventType::STATE_CHANGE && m_worldPending) {
		m_worldPending = false;
		ska::GameEvent ge(ska::GameEventType::GAME_WINDOW_READY);
		ge.windowWidth = m_windowSize.x;
		ge.windowHeight = m_windowSize.y;
		m_eventDispatcher.ska::Observable<ska::GameEvent>::notifyObservers(ge);
		resetAccumulator();
	}
	return false;
}

bool WGameCore::onGameEvent(ska::GameEvent& ge) {
	if (ge.getEventType() == ska::GameEventType::GAME_WINDOW_READY || ge.getEventType() == ska::GameEventType::GAME_WINDOW_RESIZED) {
		m_windowSize = { static_cast<unsigned int>(ge.windowWidth), static_cast<unsigned int>(ge.windowHeight) };
	}
	return false;
}

float WGameCore::ticksWanted() const {
	static const unsigned int FPS = 50;
	static const float TICKS = 1000.F / FPS;
	return TICKS;
}

bool WGameCore::onTeleport(MapEvent& me) {
	// A battle starts on the map, the map goes on after a battle (no transition) ; another map : the screen fades to black, the map changes, the screen
	// comes back (the change is already in a transition : now)
	if (me.eventType == MapEventType::Switch_Battle || me.resume || m_worldState->getFade().running()) {
		switchState(me);
		return true;
	}
	m_worldState->getFade().start([this, event = me]() {
		switchState(event);
	});
	return true;
}

void WGameCore::switchState(const MapEvent& me) {
	if (m_currentState != nullptr) {
		auto& wn = m_worldState->scheduleRemoveSubState(*m_currentState);
		m_currentState = nullptr;
		if (me.eventType == MapEventType::Switch_Battle) {
			m_nextState = std::make_unique<StateFight>(m_entityManager, m_eventDispatcher, *m_worldState, me.position, *me.fightComponent);
			wn.then([&](ska::StateBase& worldState) {
				m_currentState = &static_cast<StateFight&>(m_worldState->addSubState(std::move(m_nextState)));
				resetAccumulator();
			});
		} else {
			m_nextState = std::make_unique<StateMap>(m_entityManager, m_eventDispatcher, *m_worldState, MapDirectory(me.mapName), me.chipsetName, me.position, me.walkIn, me.resume);
			wn.then([&](ska::StateBase& worldState) {
				m_currentState = &static_cast<StateMap&>(m_worldState->addSubState(std::move(m_nextState)));
				resetAccumulator();
			});
		}
	}
}

std::unique_ptr<ska::GameApp> ska::GameApp::get() {

	auto gc = std::make_unique<WGameCore::GameConf>();
	auto& core = gc->requireModule<ska::CoreModule<CustomEntityManager>>("Core", CustomEntityManager{ gc->getEventDispatcher() }, gc->getEventDispatcher());
	/* Configure inputs types */
	core.addInputContext<ska::KeyboardInputMapContext>(ska::EnumContextManager::CONTEXT_MAP);
	core.addInputContext<ska::KeyboardInputGUIContext>(ska::EnumContextManager::CONTEXT_GUI);

	auto widthBlocks = 30;
	auto heightBlocks = 20;

	std::string title;
	try {
		IniReader reader("gamesettings.ini");
		widthBlocks = reader.get<int>("Window width_blocks");
		heightBlocks = reader.get<int>("Window height_blocks");
		title = reader.get<std::string>("Window title");
	} catch (FileException& fe) {
		title = "Default title";
		std::cerr << "Error while loading game settings : " << fe.what() << std::endl;
	}

	static constexpr auto tailleblocFenetre = 32;
	auto window = std::make_unique<SDLWindow>(title, widthBlocks * tailleblocFenetre, heightBlocks * tailleblocFenetre);
	auto renderer = std::make_unique<SDLRenderer>(*window);
	auto vdc = std::make_unique<ska::VectorDrawableContainer>(*renderer);

	gc->requireModule<ska::GraphicModule>("Graphic", gc->getEventDispatcher(), std::move(vdc), std::move(renderer), std::move(window));
	gc->requireModule<ska::SoundModule<PokemonSoundRenderer>>("Sound", gc->getEventDispatcher());

	try {
		return std::make_unique<WGameCore>(core.getEntityManager(), std::move(gc));
	} catch (GenericException& ge) {
		SLOG_STATIC(ska::LogLevel::Error, WGameCore) << ge.what();
		ska::MessagePopup(ska::MessageType::Enum::Error, "Uncaught exception occured", ge.what(), nullptr);
		throw ge;
	}
	
}


int WGameCore::onTerminate(ska::TerminateProcessException& tpe) {
	SLOG(ska::LogLevel::Info) << tpe.what();
	return 0;
}

int WGameCore::onException(ska::GenericException& e) {
	/* Handles Generics Game exceptions */
	SLOG(ska::LogLevel::Error) << e.what();
	ska::MessagePopup(ska::MessageType::Enum::Error, "Uncaught exception occured", e.what(), nullptr);

	// The process fails : a launcher (tools/ska-run) sees the error
	return 1;
}
