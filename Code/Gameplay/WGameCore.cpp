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
m_playerICM(m_rawInputListener),
m_settings("gamesettings.ini"),
m_worldScene(m_entityManager, m_sceneHolder, m_playerICM, *this) {

	/* MAP inputs */
	auto mapicp = ska::InputContextPtr(new ska::KeyboardInputMapContext());
	m_playerICM.addContext(mapicp);

	/* GUI inputs */
	auto guiicp = ska::InputContextPtr(new ska::KeyboardInputGUIContext());
	m_playerICM.addContext(guiicp);

	/* Let's start on the map */
	auto scene = ska::ScenePtr(new SceneMap(*this, m_sceneHolder, m_playerICM, m_worldScene, false));
	m_sceneHolder.nextScene(scene);
	m_sceneHolder.update();

	//m_inv.load("."FILE_SEPARATOR"Menu"FILE_SEPARATOR"inventory_square.png", "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"inventory_square_highlight.png");
	m_fpsCalculator.setDisplayPriority(INT_MAX);
}

void WGameCore::resize(unsigned int w, unsigned int h) {
	//TODO
	//TODD Move to Window
	//SDL_RenderSetLogicalSize(m_renderer, w, h);
	m_width = w;
	m_height = h;
}

WorldScene& WGameCore::getWorldScene() {
	return m_worldScene;
}

ska::ScenePtr& WGameCore::getScene() {
	return m_sceneHolder.getScene();
}

void WGameCore::transition(int type)  {
	//type : 1 = entrant, 0 = sortant
	ska::Texture fondu("."FILE_SEPARATOR"Sprites"FILE_SEPARATOR"fondu.png");
    unsigned int mosaicNumberX, mosaicNumberY;
	fondu.setBlendMode(SDL_BLENDMODE_BLEND);

    mosaicNumberX = getWidth()/fondu.getWidth() + 1;
    mosaicNumberY = getHeight()/fondu.getHeight() + 1;

	ska::Rectangle buf;
    for(unsigned int i = 0; i <= 255; i += 50) {
        graphicUpdate();
        fondu.setAlpha((255-i)*type + (1-type)*i);

        buf.x = buf.y = 0;
        for(unsigned int j = 0; j < mosaicNumberX; j++) {
            for(unsigned int k = 0; k < mosaicNumberY; k++) {
				fondu.render(buf.x, buf.y);
                buf.y += fondu.getHeight();
            }
            buf.x += fondu.getWidth();
            buf.y = 0;
        }


		flip();
        SDL_Delay(30);
    }
}

void WGameCore::nextScene(std::unique_ptr<ska::Scene>& scene) {
	m_sceneHolder.nextScene(scene);
}

//Boucle principale gérant évènements et affichages du monde.
bool WGameCore::refresh() {
	//t et t0 sont les temps pour la gestion des fps
    long t = 0, t0 = 0;

	//Ici, transition entrante
    transition(1);
	static const int FPS = 63;
	static const int TICKS = 1000 / FPS;

    //BOUCLE PRINCIPALE A CHAQUE FRAME
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
        } else {
			/* Temporisation entre 2 frames */
			SDL_Delay(TICKS - (t - t0));
		}
    }


    return true;
}


void WGameCore::graphicUpdate(void) {
	ska::VectorDrawableContainer drawables;
	m_sceneHolder.getScene()->graphicUpdate(drawables);
	drawables.add(m_fpsCalculator.getRenderable());
	drawables.draw();
}


/* TODO éviter try catch chaque frame (PERFS ?) */
void WGameCore::eventUpdate(bool movingDisallowed) {
	try {
		/* Scene dependent event update */
		m_sceneHolder.getScene()->eventUpdate(movingDisallowed);
		m_sceneHolder.update();
	} catch (ska::SceneDiedException sde) {
	}
}


Settings& WGameCore::getSettings() {
    return m_settings;
}

ska::World& WGameCore::getWorld() {
	return m_worldScene.getWorld();
}

WGameCore::~WGameCore() {
}






