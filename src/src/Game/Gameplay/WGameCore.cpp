#include "WGameCore.h"
#include "State/StateMap.h"
#include "MessageType.h"
#include "MessagePopup.h"
#include "Exceptions/FileException.h"
#include "Graphic/SDLRenderer.h"
#include "CoreModule.h"
#include "GraphicModule.h"
#include "SoundModule.h"
#include "World/Inputs/KeyboardInputMapContext.h"
#include "World/Inputs/KeyboardInputGUIContext.h"
#include "Graphic/SDLWindow.h"
#include "Draw/VectorDrawableContainer.h"

void LogsConfiguration() {
	ska::LoggerFactory::setMaxLengthClassName(35);
	//ska::LoggerFactory::staticAccess<ska::CollisionContact>().configureLogLevel(ska::EnumLogLevel::SKA_DISABLED);
	ska::LoggerFactory::staticAccess<ska::IADefinedMovementSystem>().configureLogLevel(ska::EnumLogLevel::SKA_DISABLED);
	ska::LoggerFactory::staticAccess<WorldImpl>().configureLogLevel(ska::EnumLogLevel::SKA_DISABLED);
	//ska::LoggerFactory::staticAccess<ska::WorldCollisionResponse>().configureLogLevel(ska::EnumLogLevel::SKA_DISABLED);
}

WGameCore::WGameCore(CustomEntityManager& em, GameConfPtr&& gc):
	GameCore(std::forward<GameConfPtr>(gc)),
    m_settings(m_eventDispatcher, "gamesettings.ini"),
	m_entityManager(em),
	ska::SubObserver<MapEvent>(bind(&WGameCore::onTeleport, this, std::placeholders::_1), m_eventDispatcher) {

	SKA_LOG_INFO("Game initialization");

	/* Let's start on the map state */
	m_worldState = &navigateToState<WorldState>(m_entityManager, m_eventDispatcher,  m_settings);
	
	const auto& startMapName = m_worldState->getSaveGame().getStartMapName();
	const auto& pathStartMapName = "./Levels/" + startMapName;
	m_currentState = &static_cast<StateMap&>(m_worldState->addSubState(std::make_unique<StateMap>(m_entityManager, m_eventDispatcher, *m_worldState, pathStartMapName, m_worldState->getSaveGame().getStartChipsetName())));
	
	auto& guiState = static_cast<StateGUIMap&>(m_worldState->addSubState(std::make_unique<StateGUIMap>(m_entityManager, m_eventDispatcher)));
	guiState.bindGUI(m_settings);

	m_settings.load();

}

float WGameCore::ticksWanted() const {
	static const unsigned int FPS = 50;
	static const float TICKS = 1000.F / FPS;
	return TICKS;
}

bool WGameCore::onTeleport(MapEvent& me) {
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
			m_nextState = std::make_unique<StateMap>(m_entityManager, m_eventDispatcher, *m_worldState, "./Levels/" + me.mapName, me.chipsetName, me.position);
			wn.then([&](ska::StateBase& worldState) {
				m_currentState = &static_cast<StateMap&>(m_worldState->addSubState(std::move(m_nextState)));
				resetAccumulator();
			});
		}
	}
	
	
	return true;
}

std::unique_ptr<ska::GameApp> ska::GameApp::get() {
	LogsConfiguration();

	auto gc = std::make_unique<WGameCore::GameConf>();
	auto& core = gc->requireModule<ska::CoreModule<CustomEntityManager>>("Core", gc->getEventDispatcher());
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
	auto renderer = std::make_unique<SDLRenderer>(*window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
	auto vdc = std::make_unique<ska::VectorDrawableContainer>(*renderer);

	gc->requireModule<ska::GraphicModule>("Graphic", gc->getEventDispatcher(), std::move(vdc), std::move(renderer), std::move(window));
	gc->requireModule<ska::SoundModule<PokemonSoundRenderer>>("Sound", gc->getEventDispatcher());

	try {
		return std::make_unique<WGameCore>(core.getEntityManager(), std::move(gc));
	} catch (GenericException& ge) {
		SKA_STATIC_LOG_ERROR(WGameCore)(ge.what());
		ska::MessagePopup(ska::MessageType::Enum::Error, "Uncaught exception occured", ge.what(), nullptr);
		throw ge;
	}
	
}


int WGameCore::onTerminate(ska::TerminateProcessException& tpe) {
	SKA_LOG_MESSAGE(tpe.what());
	return 0;
}

int WGameCore::onException(ska::GenericException& e) {
	/* Handles Generics Game exceptions */
	SKA_LOG_ERROR(e.what());
	ska::MessagePopup(ska::MessageType::Enum::Error, "Uncaught exception occured", e.what(), nullptr);

	return 0;
}
