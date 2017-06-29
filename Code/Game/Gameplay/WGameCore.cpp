#include "WGameCore.h"
#include "Inputs/KeyboardInputMapContext.h"
#include "Inputs/KeyboardInputGUIContext.h"
#include "State/StateMap.h"
#include "MessageType.h"
#include "MessagePopup.h"

#define SKA_DEBUG

WGameCore::WGameCore():
	m_settings(m_eventDispatcher, "gamesettings.ini") {

	/* Configure inputs types */
	addInputContext<ska::KeyboardInputMapContext>(ska::EnumContextManager::CONTEXT_MAP);
	addInputContext<ska::KeyboardInputGUIContext>(ska::EnumContextManager::CONTEXT_GUI);

	m_guiMapScene = makeState<StateGUIMap>();
    m_guiMapScene->bindGUI(m_settings);
	m_settings.load();

	/* Let's start on the map scene */
	m_worldScene = makeState<WorldState>(*this, m_settings);
	m_worldScene->linkSubState(*m_guiMapScene.get());
	navigateToState<StateMap>(*m_worldScene, m_worldScene->getSaveGame().getStartMapName(), m_worldScene->getSaveGame().getStartChipsetName(), false).linkSubState(*m_worldScene.get());

	//m_inv.load("." FILE_SEPARATOR "Menu" FILE_SEPARATOR "inventory_square.png", "." FILE_SEPARATOR "Menu" FILE_SEPARATOR "inventory_square_highlight.png");
}

float WGameCore::ticksWanted() const {
	static const unsigned int FPS = 50;
	static const float TICKS = 1000.F / FPS;
	return TICKS;
}

ska::GameApp& ska::GameApp::get() {
	static WGameCore wgc;
	return wgc;
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






