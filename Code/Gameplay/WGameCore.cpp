#include "WGameCore.h"
#include "../ska/Inputs/KeyboardInputMapContext.h"
#include "../ska/Inputs/KeyboardInputGUIContext.h"
#include "Scene/SceneMap.h"
#include "../ska/Graphic/Draw/VectorDrawableContainer.h"
#include "../ska/Exceptions/SceneDiedException.h"

WGameCore::WGameCore(const std::string& title, const unsigned int w, const unsigned int h):
Window(title, w, h),
m_playerICM(m_rawInputListener),
m_settings(m_eventDispatcher, "gamesettings.ini"),
m_worldScene(m_entityManager, m_sceneHolder, m_playerICM, *this, m_settings, m_eventDispatcher),
m_soundManager(m_eventDispatcher) {

	m_eventDispatcher.addMultipleObservers<ska::SoundEvent, ska::WorldEvent>(m_soundManager, m_soundManager);

	/* MAP inputs */
	auto mapicp = std::unique_ptr<ska::InputContext>(std::make_unique<ska::KeyboardInputMapContext>());
	m_playerICM.addContext(ska::EnumContextManager::CONTEXT_MAP, mapicp);

	/* GUI inputs */
	auto guiicp = std::unique_ptr<ska::InputContext>(std::make_unique<ska::KeyboardInputGUIContext>());
	m_playerICM.addContext(ska::EnumContextManager::CONTEXT_GUI, guiicp);

	/* Let's start on the map */
	auto scene = std::unique_ptr<ska::Scene>(std::make_unique<SceneMap>(*this, m_sceneHolder, m_eventDispatcher, m_playerICM, m_worldScene, false));
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
	auto continuer = true;
	while (continuer) {
		continuer = refreshInternal();
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
	static const auto FPS = 63;
	static const auto TICKS = 1000 / FPS;

	try {
		for (;;) {
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
			} else {
				/* Temporisation entre 2 frames */
				SDL_Delay(TICKS - (t - t0));
			}
		}
	} catch (ska::SceneDiedException sde) {
		return false;
	}

}





