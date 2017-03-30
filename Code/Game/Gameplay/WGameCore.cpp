#include "WGameCore.h"
#include "Inputs/KeyboardInputMapContext.h"
#include "Inputs/KeyboardInputGUIContext.h"
#include "Scene/SceneMap.h"

WGameCore::WGameCore(const std::string& title, const unsigned int w, const unsigned int h):
    GameCore(title, w, h),
m_settings(getEventDispatcher(), "gamesettings.ini"),
m_worldScene(getEntityManager(), *this, getInputContext(), getWindow(), m_settings, getEventDispatcher()) {


	/* MAP inputs */
	auto mapicp = std::unique_ptr<ska::InputContext>(std::make_unique<ska::KeyboardInputMapContext>());
	addInputContext(ska::EnumContextManager::CONTEXT_MAP, mapicp);

	/* GUI inputs */
	auto guiicp = std::unique_ptr<ska::InputContext>(std::make_unique<ska::KeyboardInputGUIContext>());
	addInputContext(ska::EnumContextManager::CONTEXT_GUI, guiicp);

	/* Let's start on the map */
	auto scene = std::unique_ptr<ska::Scene>(std::make_unique<SceneMap>(getWindow(), *this, getEventDispatcher(), getInputContext(), m_worldScene, false));
	nextScene(scene);
	update();

	//m_inv.load("." FILE_SEPARATOR "Menu" FILE_SEPARATOR "inventory_square.png", "." FILE_SEPARATOR "Menu" FILE_SEPARATOR "inventory_square_highlight.png");
	m_fpsCalculator.setDisplayPriority(INT_MAX);

}

WorldScene& WGameCore::getWorldScene() {
	return m_worldScene;
}

Settings& WGameCore::getSettings() {
    return m_settings;
}

ska::World& WGameCore::getWorld() {
	return m_worldScene.getWorld();
}






