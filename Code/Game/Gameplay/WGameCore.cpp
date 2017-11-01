#include "WGameCore.h"
#include "Inputs/KeyboardInputMapContext.h"
#include "Inputs/KeyboardInputGUIContext.h"
#include "State/StateMap.h"
#include "MessageType.h"
#include "MessagePopup.h"
#include "World/WorldStateChanger.h"

#define SKA_DEBUG

WGameCore::WGameCore():
	m_settings(m_eventDispatcher, "gamesettings.ini") {

}

float WGameCore::ticksWanted() const {
	static const unsigned int FPS = 50;
	static const float TICKS = 1000.F / FPS;
	return TICKS;
}

std::unique_ptr<ska::GameApp> ska::GameApp::get() {
	auto wgc = std::make_unique<WGameCore>();
	wgc->init();
	return wgc;
}

void WGameCore::init() {
    SKA_LOG_INFO("Game initialization");

	/* Configure inputs types */
	addInputContext<ska::KeyboardInputMapContext>(ska::EnumContextManager::CONTEXT_MAP);
	addInputContext<ska::KeyboardInputGUIContext>(ska::EnumContextManager::CONTEXT_GUI);

	m_guiMapScene = makeState<StateGUIMap>();
	m_guiMapScene->bindGUI(m_settings);
	m_settings.load();

	/* Let's start on the map state */
	m_worldScene = makeState<WorldState>(*this, m_settings);
	m_worldScene->linkSubState(*m_guiMapScene.get());

	WorldStateChanger wsc(*m_worldScene, m_worldScene->getSaveGame().getStartMapName(), m_worldScene->getSaveGame().getStartChipsetName(), false, ska::Point<int>());
	navigateToState<StateMap>(wsc).linkSubState(*m_worldScene.get());
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
