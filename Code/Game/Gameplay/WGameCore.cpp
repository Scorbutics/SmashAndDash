#include "WGameCore.h"
#include "Inputs/KeyboardInputMapContext.h"
#include "Inputs/KeyboardInputGUIContext.h"
#include "Scene/SceneMap.h"

WGameCore::WGameCore(const std::string& title, const unsigned int w, const unsigned int h):
    GameCore(title, w, h),
	m_settings(m_eventDispatcher, "gamesettings.ini") {

	/* Configure inputs types */
	addInputContext(ska::EnumContextManager::CONTEXT_MAP, std::make_unique<ska::KeyboardInputMapContext>());
	addInputContext(ska::EnumContextManager::CONTEXT_GUI, std::make_unique<ska::KeyboardInputGUIContext>());

	/* Let's start on the map scene */
	m_worldScene = makeScene<WorldScene>(m_settings);
	navigateToScene<SceneMap>(*m_worldScene, m_worldScene->getSaveGame().getStartMapName(), m_worldScene->getSaveGame().getStartChipsetName(), false).linkSubScene(*m_worldScene.get());

	//m_inv.load("." FILE_SEPARATOR "Menu" FILE_SEPARATOR "inventory_square.png", "." FILE_SEPARATOR "Menu" FILE_SEPARATOR "inventory_square_highlight.png");
	//m_fpsCalculator.setDisplayPriority(INT_MAX);

}






