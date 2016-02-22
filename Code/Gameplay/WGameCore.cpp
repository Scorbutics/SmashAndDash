#include <stdint.h>
#include <iostream>
#include <string>
#include <SDL2/SDL.h>
#include <fstream>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <windows.h>
#include <fmodex/fmod.h>
#include <math.h>
#include <vector>
#include <sstream>
#include "../Utils/IDs.h"
#include "../Gameplay\WGameCore.h"
#include "../Utils/Chargemententitees.h"
#include "../Utils/Observable.h"
#include "../Utils/ChargementImages.h"
#include "../Graphic/Scrolling.h"
#include "../Gameplay/World/World.h"
#include "../Gameplay/Weather.h"
#include "../Script/ScriptDispatcher.h"
using namespace std;


WGameCore::WGameCore():
    Window(), m_settings("gamesettings.ini"), m_chipsetAni(3, 4, true), m_mobSpawner(16000), m_saveManager("save1")
{
	m_phero = m_EntityFactory.getTrainer();

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
	m_kdListener.getKeyInput()->resetAll();
	m_kdListener.getMouseInput()->resetAll();

	m_saveManager.loadGame("save1");
	m_world.load(m_saveManager.getStartMapName(), m_saveManager.getStartChipsetName(), m_laFenetre, m_loFenetre);
	m_scrolling = true;
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

Animation& WGameCore::getChipsetAnimation()
{
    return m_chipsetAni;
}

TrainerCard& WGameCore::getTrainerCard()
{
    return m_trainerCard;
}

SDL_Rect& WGameCore::getOffsetChipset()
{
    return m_OfChip;
}

SDL_Rect& WGameCore::getORel()
{
    return m_origineRelative;
}

Player* WGameCore::getHero()
{
	if(m_phero == NULL)
		return NULL;
    return &(*m_phero);
}

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

void WGameCore::setORel(const SDL_Rect &oRel)
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
	Texture fondu("."FILE_SEPARATOR"Sprites"FILE_SEPARATOR"fondu.png");
	WGameCore& wScreen = WGameCore::getInstance();
    unsigned int mosaicNumberX, mosaicNumberY;
	fondu.setBlendMode(SDL_BLENDMODE_BLEND);

    mosaicNumberX = wScreen.getWidth()/fondu.getWidth() + 1;
    mosaicNumberY = wScreen.getHeight()/fondu.getHeight() + 1;

    SDL_Rect buf;
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

ParticleManager& WGameCore::getParticleManager()
{
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
	m_sceneCursor->graphicUpdate();
	
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

InputListener& WGameCore::getInputListener()
{
    return m_kdListener;
}

void WGameCore::eventUpdate(bool movingDisallowed)
{
	m_sceneCursor->eventUpdate(movingDisallowed);

}


void WGameCore::initNewWorld()
{

    // 2) Récupère les zones de combat situées dans le fichier evenement du monde
    m_fight.setAreasFromLayerEvent();
    clog << "Zone de combat installées" << endl;

    // 3) Chargement des entités apparaissant à l'écran. Pour m_persoEntite on va créer un tableau bidimensionnel :
    //1ere dimension: le nombre de types différents de mobs sur la map et 2eme dimension: le nombre de mobs du même type.
    //Ainsi que récupération des positions des évenements/entités.
    LoadEntities();
    m_phero->setID(0);
    clog << "Chargement des entités terminé" << endl;

    // 4) Rechargement des propriétés du monde (exemple : musique de fond, temps et brouillard, pluie, etc...)
    m_world.getData();
    clog << "Paramètres concernant le monde chargés" << endl << endl;

}



void WGameCore::setHero(Player* hero)
{
	m_kdListener.removeObserver(m_phero);
    m_phero = hero;
	m_kdListener.addObserver(m_phero);
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
            m_kdListener.updateEvents();

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
vector<SDL_Rect> WGameCore::detectEntity(SDL_Rect box)
{
    SDL_Rect posEvent;
    vector<SDL_Rect> id;
	list<Character*>& currentEntityList = m_EntityFactory.getCharacterList();

	for (Character* entity : currentEntityList)
	{
		posEvent = entity->getHitbox();

		if(CollisionBoxABoxB(box, posEvent))
		{
			SDL_Rect pos;
			pos.x = (Sint16)entity->getID();
			pos.y = (Sint16)entity->getEntityNumber();
			pos.w = 0;
			pos.h = 0;
			id.push_back(pos);
		}
    }

    return id;

}

vector<SDL_Rect> WGameCore::detectEntity(SDL_Rect box, int direction)
{
	static const int diagPitch = (int)(TAILLEBLOC / (2 * sqrt((float)2))) + 1;
	static const int directPitch = TAILLEBLOC / 2 + 1;
    SDL_Rect buf = box;
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

SpriteAnimationManager& WGameCore::getSpriteAnimationManager()
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


	//Désallocation mémoire
	m_particleManager.stop();
	m_EntityFactory.deleteAll();
	ScriptDispatcher::getInstance().clear();

}

GUI& WGameCore::getGUI()
{
    return m_gui;
}

World& WGameCore::getWorld()
{
	return m_world;
}


Fight& WGameCore::getFight()
{
    return m_fight;
}

EntityFactory& WGameCore::getEntityFactory()
{
    return m_EntityFactory;
}



WGameCore::~WGameCore()
{
   for(unsigned int i = 0; i < m_guiList.size(); i++)
        delete m_guiList[i];
    this->quitter(false);
}






