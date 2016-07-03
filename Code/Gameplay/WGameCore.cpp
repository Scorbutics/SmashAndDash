#include <vector>
#include "../ska/Inputs/InputContext.h"
#include "../ska/Inputs/KeyboardInputMapContext.h"
#include "../ska/Inputs/KeyboardInputGUIContext.h"
#include "../Gameplay\WGameCore.h"
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

using namespace std;



WGameCore::WGameCore():
Window(), 
m_playerICM(m_rawInputListener),
m_settings("gamesettings.ini"),
m_worldScene(m_entityManager, m_sceneHolder, m_playerICM, m_laFenetre, m_loFenetre),
m_chipsetAni(3, 4, true) {

	m_OfChip = { 0, 0 };
	m_chipsetAni.setOffsetAndFrameSize(m_OfChip);

	/* MAP inputs */
	m_playerICM.addContext(ska::InputContextPtr(new ska::KeyboardInputMapContext()));

	/* GUI inputs */
	m_playerICM.addContext(ska::InputContextPtr(new ska::KeyboardInputGUIContext()));

	/* Let's start on the map */
	m_sceneHolder.nextScene(ska::ScenePtr(new SceneMap(m_sceneHolder, m_playerICM, m_worldScene)));
	m_sceneHolder.update();

	m_speedInertie = 0;
	m_inv.load("."FILE_SEPARATOR"Menu"FILE_SEPARATOR"inventory_square.png", "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"inventory_square_highlight.png");
	m_fpsCalculator.setDisplayPriority(INT_MAX);
}

void WGameCore::resize(unsigned int w, unsigned int h) {
	//SDL_RenderSetLogicalSize(m_renderer, w, h);
	m_laFenetre = w;
	m_loFenetre = h;
}

WorldScene& WGameCore::getWorldScene() {
	return m_worldScene;
}

ska::ScenePtr& WGameCore::getScene() {
	return m_sceneHolder.getScene();
}

Pokeball& WGameCore::getPokeball()
{
    return m_pokeball;
}

ska::Animation& WGameCore::getChipsetAnimation()
{
    return m_chipsetAni;
}

TrainerCard& WGameCore::getTrainerCard()
{
    return m_trainerCard;
}

void WGameCore::setOffsetChipset(int x, int y, int w, int h)
{
    m_OfChip.x = x;
    m_OfChip.y = y;
    m_OfChip.w = w;
    m_OfChip.h = h;
}

float WGameCore::getSpeedInertie()
{
    return m_speedInertie;
}

void WGameCore::setSpeedInertie(float x)
{
    m_speedInertie = x;
}


void WGameCore::transition(int type)  {
	//type : 1 = entrant, 0 = sortant
	ska::Texture fondu("."FILE_SEPARATOR"Sprites"FILE_SEPARATOR"fondu.png");
	WGameCore& wScreen = WGameCore::getInstance();
    unsigned int mosaicNumberX, mosaicNumberY;
	fondu.setBlendMode(SDL_BLENDMODE_BLEND);

    mosaicNumberX = wScreen.getWidth()/fondu.getWidth() + 1;
    mosaicNumberY = wScreen.getHeight()/fondu.getHeight() + 1;

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


		wScreen.flip();
        SDL_Delay(30);
    }
}

MouseCursor& WGameCore::getMouseCursor()
{
	return m_mouseCursor;
}

PokemonManager& WGameCore::getPokemonManager()
{
    return m_pkmnManager;
}

void WGameCore::addTaskToQueue(ska::RunnablePtr& t) {
	m_taskQueue.queueTask(t);
}

bool WGameCore::hasRunningTask() {
	return m_taskQueue.hasRunningTask();
}

void WGameCore::nextScene(std::unique_ptr<ska::Scene>& scene) {
	m_sceneHolder.nextScene(scene);
}

//Boucle principale gérant évènements et affichages du monde.
bool WGameCore::refresh() {
	//t et t0 sont les temps pour la gestion des fps
    long t = 0, t0 = 0; 

    m_ecritureLog = false;
    initNewWorld();

    
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
		/* If exists, an helper that executes current running task once */
		m_taskQueue.refresh();
	} catch (ska::SceneDiedException sde) {
	}
}


void WGameCore::initNewWorld() {
    // Récupère les zones de combat situées dans le fichier evenement du monde
    //m_fight.setAreasFromLayerEvent();   
}

void WGameCore::waitQuit(DialogMenu* window)
{
    int t = 0, t0 = 0; //t et t0 sont les temps pour la gestion de la durée (SDL_PollEvent).
/*    while(window->isVisible())
    {
        t = SDL_GetTicks();
        if (t - t0 > 30) // Si 30 ms se sont écoulées
        {
            this->graphicUpdate();
			m_gui.dialogRefresh();

            //Gère les évènements extérieurs (input)
            //m_kdListener.updateEvents();

            flip();

            t0 = t; // Le temps "actuel" devient le temps "precedent" pour nos futurs calculs
        }
        else // Si ça fait moins de 30ms depuis le dernier tour de boucle, on endort le programme le temps qu'il faut
            SDL_Delay(30 - (t - t0));
    }*/
}

Inventory& WGameCore::getInventory()
{
    return m_inv;
}

Settings& WGameCore::getSettings()
{
    return m_settings;
}

GUI& WGameCore::getGUI()
{
    return m_gui;
}

ska::World& WGameCore::getWorld()
{
	return m_worldScene.getWorld();
}

const ska::InputActionContainer& WGameCore::getActions() const {
	return m_playerICM.getActions();
}

const ska::InputRangeContainer& WGameCore::getRanges() const {
	return m_playerICM.getRanges();
}

const ska::InputToggleContainer& WGameCore::getToggles() const {
	return m_playerICM.getToggles();
}

WGameCore::~WGameCore() {
}






