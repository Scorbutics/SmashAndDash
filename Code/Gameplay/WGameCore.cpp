#include <vector>
#include "../Utils/IDs.h"
#include "../Gameplay\WGameCore.h"
#include "../ska/Utils/Observable.h"
#include "../Utils/ChargementImages.h"
#include "../ska/World/World.h"
#include "../Gameplay/Weather.h"
#include "../ska/Graphic/Draw/VectorDrawableContainer.h"
#include "../ska/Utils/RectangleUtils.h"
#include "../ska/World/LayerE.h"
#include "../ska/Exceptions/CorruptedFileException.h"
#include "../ska/Exceptions/SceneDiedException.h"
#include "../ska/ECS/PrefabEntityManager.h"
#include "./Scene/SceneMap.h"

using namespace std;


WGameCore::WGameCore():
Window(), 
m_inputCManager(m_rawInputListener),
m_settings("gamesettings.ini"),
m_worldScene(m_entityManager, m_sceneHolder, m_inputCManager, m_laFenetre, m_loFenetre),
m_chipsetAni(3, 4, true) {

	m_OfChip.y = 0;
	m_OfChip.x = 0;
	/*m_OfChip.w = m_world.getBlockSize();
	m_OfChip.h = m_world.getBlockSize();*/
	m_chipsetAni.setOffsetAndFrameSize(m_OfChip);

	/* Let's start on the map */
	m_sceneHolder.nextScene(ska::ScenePtr(new SceneMap(m_sceneHolder, m_inputCManager, m_worldScene)));

	m_speedInertie = 0;
	m_pokeball.setSprites("."FILE_SEPARATOR"Sprites"FILE_SEPARATOR"Fight"FILE_SEPARATOR"pokeball.png", "."FILE_SEPARATOR"Sprites"FILE_SEPARATOR"Fight"FILE_SEPARATOR"pokeball-openned.png", "."FILE_SEPARATOR"Sprites"FILE_SEPARATOR"Fight"FILE_SEPARATOR"pokeball-aura.png");
	m_inv.load("."FILE_SEPARATOR"Menu"FILE_SEPARATOR"inventory_square.png", "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"inventory_square_highlight.png");


	//m_world.load(m_saveManager.getStartMapName(), m_saveManager.getStartChipsetName(), m_saveManager.getPathName());
}

void WGameCore::resize(unsigned int w, unsigned int h)
{
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


ShakerManager& WGameCore::getShakerManager()
{
    return m_shaker;
}

float WGameCore::getSpeedInertie()
{
    return m_speedInertie;
}

void WGameCore::setSpeedInertie(float x)
{
    m_speedInertie = x;
}


void WGameCore::transition(int type) //type : 1 = entrant, 0 = sortant
{
	ska::Texture fondu("."FILE_SEPARATOR"Sprites"FILE_SEPARATOR"fondu.png");
	WGameCore& wScreen = WGameCore::getInstance();
    unsigned int mosaicNumberX, mosaicNumberY;
	fondu.setBlendMode(SDL_BLENDMODE_BLEND);

    mosaicNumberX = wScreen.getWidth()/fondu.getWidth() + 1;
    mosaicNumberY = wScreen.getHeight()/fondu.getHeight() + 1;

	ska::Rectangle buf;
    for(unsigned int i = 0; i <= 255; i += 50)
    {
		
        this->graphicUpdate();
        fondu.setAlpha((255-i)*type + (1-type)*i);

        buf.x = buf.y = 0;
        for(unsigned int j = 0; j < mosaicNumberX; j++)
        {
            for(unsigned int k = 0; k < mosaicNumberY; k++)
            {
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

ska::ParticleManager& WGameCore::getParticleManager() {
    return m_particleManager;
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



    //BOUCLE PRINCIPALE A CHAQUE FRAME
    while (1) {
        t = ska::TimeUtils::getTicks();
		
		if (t - t0 > 20)  {
            //Rafraîchissement à chaque frame : graphique puis évènementiel
			graphicUpdate();
			eventUpdate(false);

			flip();
			SDL_RenderClear(m_renderer);
			// Le temps "actuel" devient le temps "precedent" pour nos futurs calculs
            t0 = t; 
        } else {
			/* Temporisation entre 2 frames */
			SDL_Delay(20 - (t - t0));
		}
    }


    return true;
}


void WGameCore::graphicUpdate(void) {
	ska::VectorDrawableContainer drawables;
	m_sceneHolder.getScene()->graphicUpdate(drawables);
	drawables.draw();
}

RainParticleManager& WGameCore::getRainParticleManager() {
	return m_rainParticleManager;
}

bool WGameCore::isScrollingActive()
{
	return (!m_fight.isFighting() && m_scrolling);
}

void WGameCore::activeScrolling(bool b)
{
	m_scrolling = b;
}

void WGameCore::eventUpdate(bool movingDisallowed) {
	try {
		m_sceneHolder.getScene()->eventUpdate(movingDisallowed);
	}
	catch (ska::SceneDiedException sde) {
	}
}


void WGameCore::initNewWorld() {
    // Récupère les zones de combat situées dans le fichier evenement du monde
    //m_fight.setAreasFromLayerEvent();   
}

void WGameCore::waitQuit(DialogMenu* window)
{
    int t = 0, t0 = 0; //t et t0 sont les temps pour la gestion de la durée (SDL_PollEvent).
    while(window->isVisible())
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
    }
}

AI& WGameCore::getAI()
{
    return m_ai;
}

Inventory& WGameCore::getInventory()
{
    return m_inv;
}

ska::SpriteAnimationManager& WGameCore::getSpriteAnimationManager()
{
	return m_spriteAnimManager;
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


Fight& WGameCore::getFight()
{
    return m_fight;
}

const ska::InputActionContainer& WGameCore::getActions() const {
	return m_inputCManager.getActions();
}

const ska::InputRangeContainer& WGameCore::getRanges() const {
	return m_inputCManager.getRanges();
}

const ska::InputToggleContainer& WGameCore::getToggles() const {
	return m_inputCManager.getToggles();
}

WGameCore::~WGameCore()
{
	for (unsigned int i = 0; i < m_guiList.size(); i++) {
		delete m_guiList[i];
	}
}






