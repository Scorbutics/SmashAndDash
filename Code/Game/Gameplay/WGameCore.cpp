#include "WGameCore.h"
#include "Inputs/KeyboardInputMapContext.h"
#include "Inputs/KeyboardInputGUIContext.h"
#include "State/StateMap.h"
#include "MessageType.h"
#include "MessagePopup.h"
#include "World/WorldStateChanger.h"
#include "Exceptions/FileException.h"
#include "Graphic/SDLRenderer.h"
#include "CoreModule.h"
#include "GraphicModule.h"
#include "SoundModule.h"

WGameCore::WGameCore(ska::GameConfiguration&& gc, RendererPtr&& renderer, WindowPtr&& window): 
	GameCore(std::forward<ska::GameConfiguration>(gc), std::forward<RendererPtr>(renderer), std::forward<WindowPtr>(window)),
                        m_settings(m_eventDispatcher, "gamesettings.ini"){
	SKA_LOG_INFO("Game initialization");

	/* Configure inputs types */
	addInputContext<ska::KeyboardInputMapContext>(ska::EnumContextManager::CONTEXT_MAP);
	addInputContext<ska::KeyboardInputGUIContext>(ska::EnumContextManager::CONTEXT_GUI);
	
	m_guiMapScene = makeState<StateGUIMap>(m_eventDispatcher);
	m_guiMapScene->bindGUI(m_settings);
	m_settings.load();

	/* Let's start on the map state */
	m_worldState = makeState<WorldState>(m_eventDispatcher,  m_settings);
	m_worldState->linkSubState(*m_guiMapScene.get());

	const auto& startMapName = m_worldState->getSaveGame().getStartMapName();
	const auto& pathStartMapName = "." FILE_SEPARATOR "Levels" FILE_SEPARATOR "" + startMapName + ".bmp";
	/*WorldStateChanger wsc(*m_worldState, pathStartMapName, m_worldState->getSaveGame().getStartChipsetName(), false,
	                      ska::Point<int>());*/
	navigateToState<StateMap>(m_eventDispatcher, *m_worldState, pathStartMapName, m_worldState->getSaveGame().getStartChipsetName()).linkSubState(*m_worldState.get());
}

float WGameCore::ticksWanted() const {
	static const unsigned int FPS = 50;
	static const float TICKS = 1000.F / FPS;
	return TICKS;
}

std::unique_ptr<ska::GameApp> ska::GameApp::get() {
	ska::GameConfiguration gc;
	gc.requireModule<ska::CoreModule>("Core");
	gc.requireModule<ska::GraphicModule>("Graphic");
	gc.requireModule<ska::SoundModule>("Sound");

	auto widthBlocks = 30;
	auto heightBlocks = 20;

	std::string title = "Default title";
	try {
		IniReader reader("gamesettings.ini");
		widthBlocks = reader.get<int>("Window width_blocks");
		heightBlocks = reader.get<int>("Window height_blocks");
		title = reader.get<std::string>("Window title");
	}
	catch (FileException& fe) {
		std::cerr << "Error while loading game settings : " << fe.what() << std::endl;
	}

	static constexpr auto tailleblocFenetre = 32;
	auto window = std::make_unique<Window>("ska physics", widthBlocks * tailleblocFenetre, heightBlocks * tailleblocFenetre);
	auto renderer = std::make_unique<SDLRenderer>(*window, -1, SDL_RENDERER_ACCELERATED);
	return std::make_unique<WGameCore>(std::move(gc), std::move(renderer), std::move(window));
}

void LogsConfiguration() {
	ska::LoggerFactory::setMaxLengthClassName(35);
	ska::LoggerFactory::staticAccess<ska::CollisionContact>().configureLogLevel(ska::EnumLogLevel::SKA_DISABLED);
	ska::LoggerFactory::staticAccess<ska::IADefinedMovementSystem>().configureLogLevel(ska::EnumLogLevel::SKA_DISABLED);
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
