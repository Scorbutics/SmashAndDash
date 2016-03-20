#include <vector>
#include "../Utils/IDs.h"
#include "../Gameplay\WGameCore.h"
#include "../ska/Utils/Observable.h"
#include "../Utils/ChargementImages.h"
#include "../Graphic/Scrolling.h"
#include "../ska/World/World.h"
#include "../Gameplay/Weather.h"
#include "../ska/Script/ScriptDispatcher.h"
#include "../ska/Graphic/Draw/VectorDrawableContainer.h"
#include "../ska/Utils/RectangleUtils.h"
#include "../ska/World/LayerE.h"
#include "../ska/Exceptions/CorruptedFileException.h"

using namespace std;


WGameCore::WGameCore():
Window(),  m_settings("gamesettings.ini"), m_chipsetAni(3, 4, true), m_mobSpawner(16000), m_saveManager("save1"), m_world(TAILLEBLOC, m_laFenetre, m_loFenetre),
m_sceneMap(m_world.getEntityManager(), m_rawInputListener), m_sceneFight(m_world.getEntityManager(), m_rawInputListener), m_inputSystem(m_sceneMap.getInputContextManager(), m_world.getEntityManager()) {
	//m_phero = m_EntityFactory.getTrainer();

	m_OfChip.y = 0;
	m_OfChip.x = 0;
	m_OfChip.w = TAILLEBLOC;
	m_OfChip.h = TAILLEBLOC;
	m_chipsetAni.setOffsetAndFrameSize(m_OfChip);

	/* Let's start on the map */
	m_sceneCursor = &m_sceneMap;

	m_origineRelative.x = 0;
	m_origineRelative.y = 0;
	m_origineRelative.w = m_laFenetre;
	m_origineRelative.h = m_loFenetre;

	m_speedInertie = 0;
	m_pokeball.setSprites("."FILE_SEPARATOR"Sprites"FILE_SEPARATOR"Fight"FILE_SEPARATOR"pokeball.png", "."FILE_SEPARATOR"Sprites"FILE_SEPARATOR"Fight"FILE_SEPARATOR"pokeball-openned.png", "."FILE_SEPARATOR"Sprites"FILE_SEPARATOR"Fight"FILE_SEPARATOR"pokeball-aura.png");
	m_inv.load("."FILE_SEPARATOR"Menu"FILE_SEPARATOR"inventory_square.png", "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"inventory_square_highlight.png");
	//m_kdListener.getKeyInput()->resetAll();
	//m_kdListener.getMouseInput()->resetAll();

	m_saveManager.loadGame("save1");
	m_world.load(m_saveManager.getStartMapName(), m_saveManager.getStartChipsetName(), m_saveManager.getPathName());

	ska::IniReader reader("."FILE_SEPARATOR"Data"FILE_SEPARATOR"Saves"FILE_SEPARATOR + m_saveManager.getPathName() + FILE_SEPARATOR"trainer.ini");

	ska::Point<int> startPos;
	startPos.x = reader.getInt("Trainer start_posx");
	startPos.y = reader.getInt("Trainer start_posy");
	std::string startMapName = reader.getString("Trainer start_map_name");

	std::string buf = "."FILE_SEPARATOR"Levels"FILE_SEPARATOR;
	buf += startMapName;
	buf += FILE_SEPARATOR;
	buf += startMapName;
	buf += ".ini";

	ska::IniReader mapReader(buf);
	std::string startMapChipset = mapReader.getString("Chipset file");
	if (startMapChipset == "STRINGNOTFOUND") {
		throw ska::CorruptedFileException("Erreur : impossible de trouver le nom du chipset de la map de depart");
	}

	m_world.getEntityManager().createTrainer(startPos, m_world.getBlockSize());

	//m_scrolling = true;
}

void WGameCore::resize(unsigned int w, unsigned int h)
{
	//SDL_RenderSetLogicalSize(m_renderer, w, h);
	m_laFenetre = w;
	m_loFenetre = h;
}

SavegameManager& WGameCore::getSavegameManager()
{
	return m_saveManager;
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

ska::Rectangle& WGameCore::getOffsetChipset()
{
    return m_OfChip;
}

ska::Rectangle& WGameCore::getORel()
{
    return m_origineRelative;
}

/*
Player* WGameCore::getHero()
{
	if(m_phero == NULL)
		return NULL;
    return &(*m_phero);
}
*/

void WGameCore::setOffsetChipset(int x, int y, int w, int h)
{
    m_OfChip.x = x;
    m_OfChip.y = y;
    m_OfChip.w = w;
    m_OfChip.h = h;
}

void WGameCore::setORel(int x, int y)
{
    m_origineRelative.x = x;
    m_origineRelative.y = y;
}

MobSpawningManager& WGameCore::getMobSpawningManager()
{
	return m_mobSpawner;
}

ShakerManager& WGameCore::getShakerManager()
{
    return m_shaker;
}

void WGameCore::setORel(const ska::Rectangle &oRel)
{
    m_origineRelative = oRel;
}


void WGameCore::quitFlip()
{
    m_quitFlip = true;
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

void WGameCore::switchScene(EnumScene::Enum scene) {
	switch (scene) {
	case EnumScene::FIGHT:
		m_sceneCursor = &m_sceneFight;
		break;
	case EnumScene::MAP:
		m_sceneCursor = &m_sceneMap;
		break;
	}
	
}

//Boucle principale gérant évènements et affichages du monde.
bool WGameCore::refresh()
{
	//t et t0 sont les temps pour la gestion des fps
    long t = 0, t0 = 0; 

    m_continue = false;
    m_ecritureLog = false;
    m_quitFlip = false;
    this->initNewWorld();

    
	//Ici, transition entrante
    this->transition(1);



    //BOUCLE PRINCIPALE A CHAQUE FRAME
    while (!m_quitFlip)
    {
        t = SDL_GetTicks();
        if (t - t0 > 30) // Si 30 ms se sont écoulées
        {
            //Rafraîchissement à chaque frame : graphique puis évènementiel
			this->graphicUpdate();
			this->eventUpdate(false);

            
			flip();
			SDL_RenderClear(m_renderer);
            t0 = t; // Le temps "actuel" devient le temps "precedent" pour nos futurs calculs
        }
        else // Si ça fait moins de 30ms depuis le dernier tour de boucle, on endort le programme le temps qu'il faut
            SDL_Delay(30 - (t - t0));
    }


    this->quitter(m_continue);

    return m_continue;


}


//Gestion de l'ordre de l'affichage des éléments...
//gros bordel !
void WGameCore::graphicUpdate(void)
{
	ska::VectorDrawableContainer drawables;
	m_sceneCursor->graphicUpdate(drawables);
	drawables.draw();
	//m_graphicSystem.refreshAll();

	/* TODO faire une gestion de caméra externe (non uniquement focus sur le héro) */
	//Scrolling();
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

void WGameCore::setContinue(bool b)
{
    m_continue = b;
}

bool WGameCore::getContinue()
{
    return m_continue;
}

void WGameCore::eventUpdate(bool movingDisallowed) {
	m_sceneCursor->eventUpdate(movingDisallowed);
	/* Game Logic Input System */
	m_inputSystem.refresh();
}


void WGameCore::initNewWorld()
{

    // 2) Récupère les zones de combat situées dans le fichier evenement du monde
    m_fight.setAreasFromLayerEvent();

    // 3) Chargement des entités apparaissant à l'écran. Pour m_persoEntite on va créer un tableau bidimensionnel :
    //1ere dimension: le nombre de types différents de mobs sur la map et 2eme dimension: le nombre de mobs du même type.
    //Ainsi que récupération des positions des évenements/entités.


	
    //m_phero->setID(0);

    // 4) Rechargement des propriétés du monde (exemple : musique de fond, temps et brouillard, pluie, etc...)
    m_world.getData();

}


/*void WGameCore::setHero(Player* hero)
{
	//m_kdListener.removeObserver(m_phero);
    //m_phero = hero;
	//m_kdListener.addObserver(m_phero);
}*/

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

//fonction qui renvoie un tableau d'ids d'entités entrant en contact avec une box donnée (il peut y avoir plusieurs entités superposées et donc le tableau sera de taille > 1)
//dans chaque .x : id pokédex
//dans chaque .y : numéro de l'entité
vector<ska::Rectangle> WGameCore::detectEntity(ska::Rectangle box)
{
	ska::Rectangle posEvent;
	vector<ska::Rectangle> id;
	//list<Character*>& currentEntityList = m_EntityFactory.getCharacterList();

	/*for (Character* entity : currentEntityList)
	{
		posEvent = entity->getHitbox();

		if(ska::RectangleUtils::collisionBoxABoxB(box, posEvent))
		{
			ska::Rectangle pos;
			pos.x = (Sint16)entity->getID();
			pos.y = (Sint16)entity->getEntityNumber();
			pos.w = 0;
			pos.h = 0;
			id.push_back(pos);
		}
    }
	*/
    return id;

}

vector<ska::Rectangle> WGameCore::detectEntity(ska::Rectangle box, int direction)
{
	static const int diagPitch = (int)(TAILLEBLOC / (2 * sqrt((float)2))) + 1;
	static const int directPitch = TAILLEBLOC / 2 + 1;
	ska::Rectangle buf = box;
    switch(direction)
    {
        case 0:
			buf.y += directPitch;
        break;

        case 1:
			buf.x += directPitch;
        break;

        case 2:
			buf.y -= directPitch;
        break;

        case 3:
			buf.x -= directPitch;
        break;

        case 4:
			buf.y += diagPitch;
			buf.x += diagPitch;
        break;

        case 5:
			buf.y -= diagPitch;
			buf.x += diagPitch;
        break;

        case 6:
			buf.y -= diagPitch;
			buf.x -= diagPitch;
        break;

        case 7:
			buf.y += diagPitch;
			buf.x -= diagPitch;
        break;

        default:
        break;
    }

    return this->detectEntity(buf);
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

void WGameCore::quitter(bool transition)
{
	/*if (transition) {
		this->transition(0);
	}*/

	m_rainParticleManager.stop();
	m_particleManager.stop();

	//Désallocation mémoire
	//m_EntityFactory.deleteAll();
	
}

GUI& WGameCore::getGUI()
{
    return m_gui;
}

ska::World& WGameCore::getWorld()
{
	return m_world;
}


Fight& WGameCore::getFight()
{
    return m_fight;
}

/*EntityFactory& WGameCore::getEntityFactory()
{
    return m_EntityFactory;
}*/

const ska::InputActionContainer& WGameCore::getActions() const {
	return m_sceneCursor->getInputContextManager().getActions();
}

const ska::InputRangeContainer& WGameCore::getRanges() const {
	return m_sceneCursor->getInputContextManager().getRanges();
}

const ska::InputToggleContainer& WGameCore::getToggles() const {
	return m_sceneCursor->getInputContextManager().getToggles();
}

WGameCore::~WGameCore()
{
   for(unsigned int i = 0; i < m_guiList.size(); i++)
        delete m_guiList[i];
    this->quitter(false);
}






