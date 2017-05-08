#include "WGameCore.h"
#include "Inputs/KeyboardInputMapContext.h"
#include "Inputs/KeyboardInputGUIContext.h"
#include "Scene/SceneMap.h"
#include "MessageType.h"
#include "MessagePopup.h"

#define SKA_DEBUG

WGameCore::WGameCore():
	m_settings(m_eventDispatcher, "gamesettings.ini") {

	/* Configure inputs types */
	addInputContext(ska::EnumContextManager::CONTEXT_MAP, std::make_unique<ska::KeyboardInputMapContext>());
	addInputContext(ska::EnumContextManager::CONTEXT_GUI, std::make_unique<ska::KeyboardInputGUIContext>());

	m_guiMapScene = makeScene<SceneGUIMap>();
    m_guiMapScene->bindGUI(m_settings);

	/* Let's start on the map scene */
	m_worldScene = makeScene<WorldScene>(m_settings);
	m_worldScene->ska::SceneBase<CustomEntityManager, PokemonGameEventDispatcher>::linkSubScene(*m_guiMapScene.get());
	navigateToScene<SceneMap>(*m_worldScene, m_worldScene->getSaveGame().getStartMapName(), m_worldScene->getSaveGame().getStartChipsetName(), false).linkSubScene(*m_worldScene.get());

	//m_inv.load("." FILE_SEPARATOR "Menu" FILE_SEPARATOR "inventory_square.png", "." FILE_SEPARATOR "Menu" FILE_SEPARATOR "inventory_square_highlight.png");
	//m_fpsCalculator.setDisplayPriority(INT_MAX);

}

ska::GameApp& ska::GameApp::get() {
	static WGameCore wgc;
	return wgc;
}

int WGameCore::onTerminate(ska::TerminateProcessException& tpe) {
	std::clog << tpe.what() << std::endl;
	return 0;
}

int WGameCore::onException(ska::GenericException& e) {
	/* Handles Generics Game exceptions */
	std::cerr << e.what() << std::endl;
	ska::MessagePopup(ska::MessageType::Enum::Error, "Uncaught exception occured", e.what(), nullptr);

	return 0;
}






