#include <vector>
#include "../ska/Inputs/InputContext.h"
#include "../ska/Inputs/KeyboardInputMapContext.h"
#include "../ska/Inputs/KeyboardInputGUIContext.h"
#include "../Gameplay/WGameCore.h"
#include "../ska/World/World.h"
#include "../Gameplay/Weather.h"
#include "../ska/Graphic/Draw/VectorDrawableContainer.h"
#include "../ska/Utils/RectangleUtils.h"
#include "../ska/World/LayerE.h"
#include "../ska/Exceptions/CorruptedFileException.h"
#include "../ska/Exceptions/SceneDiedException.h"
#include "./Scene/SceneMap.h"
#include "../Graphic/GUI/DialogMenu.h"
#include "../Graphic/GUI/WindowSettings.h"
#include "../Graphic/GUI/WindowTeam.h"
#include "../Graphic/GUI/WindowShop.h"
#include "../Graphic/GUI/WindowBag.h"
#include "../Graphic/GUI/ToolBar.h"

WGameCore::WGameCore(const std::string& title, const unsigned int w, const unsigned int h):
Window(title, w, h),
Game(),
m_playerICM(m_rawInputListener),
m_settings("gamesettings.ini"),
m_worldScene(m_entityManager, m_sceneHolder, m_playerICM, *this, m_settings, m_eventDispatcher) {

	m_eventDispatcher.template addMultipleObservers<ska::SoundEvent, ska::WorldEvent>(getSoundManager(), getSoundManager());

	/* MAP inputs */
	auto mapicp = ska::InputContextPtr(new ska::KeyboardInputMapContext());
	m_playerICM.addContext(ska::EnumContextManager::CONTEXT_MAP, mapicp);

	/* GUI inputs */
	auto guiicp = ska::InputContextPtr(new ska::KeyboardInputGUIContext());
	m_playerICM.addContext(ska::EnumContextManager::CONTEXT_GUI, guiicp);

	/* Let's start on the map */
	auto scene = ska::ScenePtr(new SceneMap(*this, m_sceneHolder, m_eventDispatcher, m_playerICM, m_worldScene, false));
	m_sceneHolder.nextScene(scene);
	m_sceneHolder.update();

	//m_inv.load("."FILE_SEPARATOR"Menu"FILE_SEPARATOR"inventory_square.png", "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"inventory_square_highlight.png");
	m_fpsCalculator.setDisplayPriority(INT_MAX);

}

WorldScene& WGameCore::getWorldScene() {
	return m_worldScene;
}

ska::ScenePtr& WGameCore::getScene() {
	return m_sceneHolder.getScene();
}


void WGameCore::nextScene(std::unique_ptr<ska::Scene>& scene) {
	m_sceneHolder.nextScene(scene);
}

//Boucle principale gérant évènements et affichages du monde.
bool WGameCore::refresh() {
	while (true) {
		refreshInternal();
	}
    return true;
}


void WGameCore::graphicUpdate(void) {
	ska::VectorDrawableContainer drawables;
	m_sceneHolder.getScene()->graphicUpdate(drawables);
	drawables.add(m_fpsCalculator.getRenderable());
	drawables.draw();
}

void WGameCore::eventUpdate(bool movingDisallowed) {
	/* Scene dependent event update */
	m_sceneHolder.getScene()->eventUpdate(movingDisallowed);
	m_sceneHolder.update();

}


Settings& WGameCore::getSettings() {
    return m_settings;
}

ska::World& WGameCore::getWorld() {
	return m_worldScene.getWorld();
}

bool WGameCore::refreshInternal() {
	//t et t0 sont les temps pour la gestion des fps
	long t = 0;
	long t0 = 0;

	//Ici, transition entrante
	static const int FPS = 63;
	static const int TICKS = 1000 / FPS;

	try {
		while (true) {
			t = ska::TimeUtils::getTicks();
			if (t - t0 > TICKS)  {
				//Rafraîchissement à chaque frame : graphique puis évènementiel
				graphicUpdate();
				eventUpdate(false);

				flip();
				SDL_RenderClear(m_renderer);
				// Le temps "actuel" devient le temps "precedent" pour nos futurs calculs
				m_fpsCalculator.calculate(t - t0);
				t0 = t;
			}
			else {
				/* Temporisation entre 2 frames */
				SDL_Delay(TICKS - (t - t0));
			}
		}
	} catch (ska::SceneDiedException sde) {
		return false;
	}

	return true;
}





