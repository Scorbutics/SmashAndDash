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

#include "MobSpawningManager.h"
#include "IniReader.h"
#include "WGameCore.h"
#include "KeyInput.h"
#include "MouseInput.h"
#include "Chargemententitees.h"
#include "Scrolling.h"
#include "GestionEvents.h"
#include "DialogMenu.h"
#include "IDs.h"
#include "Skill.h"
#include "MouvementsMob.h"
#include "GestionAnimations.h"
#include "weather.h"
#include "ScriptRead.h"
#include "GUI.h"
#include "ChargementImages.h"
#include "TrainerCard.h"
#include "LayerE.h"
#include "Layer.h"
#include "Character.h"
#include "MouseCursor.h"
#include "SavegameManager.h"

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
	SDL_RenderSetLogicalSize(m_renderer, w, h);
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

//Boucle principale g�rant �v�nements et affichages du monde.
bool WGameCore::refresh()
{
	//PortableThread* eventThread, *graphicThread;
    int t = 0, t0 = 0; //t et t0 sont les temps pour la gestion de la dur�e (SDL_PollEvent).
	//eventThread = PortableThreadCreate();
	//graphicThread = PortableThreadCreate();

    m_continue = false;
    m_ecritureLog = false;
    m_quitFlip = false;
    this->initNewWorld();

    
	//Ici, transition entrante
    //this->transition(1);



    //BOUCLE PRINCIPALE A CHAQUE FRAME
    while (!m_quitFlip)
    {
        t = SDL_GetTicks();
        if (t - t0 > 30) // Si 30 ms se sont �coul�es
        {
            //Rafra�chissement � chaque frame : graphique puis �v�nementiel
			this->graphicUpdate();
			this->eventUpdate(false);

            
			flip();
			SDL_RenderClear(m_renderer);
            t0 = t; // Le temps "actuel" devient le temps "precedent" pour nos futurs calculs
        }
        else // Si �a fait moins de 30ms depuis le dernier tour de boucle, on endort le programme le temps qu'il faut
            SDL_Delay(30 - (t - t0));
    }


    this->quitter(m_continue);
	//PortableThreadFree(eventThread);
	//PortableThreadFree(graphicThread);

    return m_continue;


}


//Gestion de l'ordre de l'affichage des �l�ments...
//gros bordel !
void* WGameCore::graphicUpdate(void)
{
    //Affiche les 2 layers composant la fen�tre monde ainsi que le layer des entit�s (personnages & mobs)
    m_world.displayLayers();

    //Affiche la pokeball si active
    m_pokeball.refresh();


    //Affichage des Skills
    if(m_fight.isFighting())
    {
        if(m_phero != NULL)
            m_phero->displaySkills();
		if(m_fight.getOpponent() != NULL)
			m_fight.getOpponent()->displaySkills();
    }

	m_spriteAnimManager.refresh();

	m_fight.display();

	m_particleManager.display(PARTICLE_MANAGER_CRUMBLING);
	m_particleManager.display(PARTICLE_MANAGER_RAIN);
    m_particleManager.displayRainFog();


    //Affiche la m�t�o si active
    if(m_world.getFog()->isActive() && *m_settings.getFogActive())
        m_world.getFog()->display();
    if(m_world.getWeather()->isActive() && *m_settings.getWeatherActive())
        m_world.getWeather()->display();


    /*//Affiche / Actualise les dialogues et fenetres
    if(m_gui.getRefreshPNJWindowCount() <= 0)
    {
        if(m_gui.getInfoPokemonWindow()->isVisible())
            m_gui.resetInfoPokemonWindow(m_phero);
        m_gui.setRefreshPNJWindowCount(REFRESH_PNJWINDOW_COUNT);
    }
    else
        m_gui.setRefreshPNJWindowCount(m_gui.getRefreshPNJWindowCount()-1);*/

    m_gui.dialogDisplay();
    if(m_gui.isVisible())
        m_gui.display();

    

    m_world.setBgmVolume(*m_settings.getSoundVolume());


    m_mouseCursor.displaySelectedPokemon();
    m_mouseCursor.displaySelectedObject();

    Scrolling();
	
	return NULL;
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

void WGameCore::eventUpdate(bool d)
{
	bool stuck = d;
	WGameCore& wScreen = WGameCore::getInstance();

    m_world.refreshEntities();
	
	//Gestion de l'intelligence artificielle en combat
    if(m_fight.isFighting())
        m_ai.act(m_fight);

	if(m_fight.isFighting())
	{
		if(m_phero != NULL)
			m_phero->refreshSkills();
		if(m_fight.isFighting() && m_fight.getOpponent() != NULL)
			m_fight.getOpponent()->refreshSkills();
	}

	m_particleManager.refresh(PARTICLE_MANAGER_EFFECT);

    //G�re les �v�nements ext�rieurs (input)
    if(!m_kdListener.refresh(stuck))
    {
        m_gui.getDialog()->hide(true);
        m_gui.getImgDialog()->hide(true);
        m_gui.hide(false);
    }

	m_gui.dialogRefresh();
	if(m_gui.isVisible())
		m_gui.refresh();

    //gere les evenements (notamment aid� par le systeme de scripts)
    GestionEvents();

	m_fight.refresh();

	//g�re l'apparition al�atoire de Pok�mon
	if(!m_fight.isFighting())
		m_mobSpawner.refresh();

    //gere le mouvement al�atoire des mobs
    MobMovements();

	m_particleManager.refresh(PARTICLE_MANAGER_CRUMBLING);
	m_particleManager.refresh(PARTICLE_MANAGER_RAIN);

	m_shaker.refresh();

}


void WGameCore::initNewWorld()
{
    ofstream clog("stdlog.txt", ios::app);

    // 1) Charge en m�moire le chipset du monde courant � afficher � l'�cran
    //m_sBank.setChipset(m_world->getChipsetName(), true);
    //clog << "Chipset charg�" << endl;

    // 2) R�cup�re les zones de combat situ�es dans le fichier evenement du monde
    m_fight.setAreasFromLayerEvent();
    clog << "Zone de combat install�es" << endl;

    // 3) Chargement des entit�s apparaissant � l'�cran. Pour m_persoEntite on va cr�er un tableau bidimensionnel :
    //1ere dimension: le nombre de types diff�rents de mobs sur la map et 2eme dimension: le nombre de mobs du m�me type.
    //Ainsi que r�cup�ration des positions des �venements/entit�s.
    LoadEntities();
    m_phero->setID(0);
    clog << "Chargement des entit�s termin�" << endl;

    // 4) Rechargement des propri�t�s du monde (exemple : musique de fond, temps et brouillard, pluie, etc...)
    m_world.getData();
    clog << "Param�tres concernant le monde charg�s" << endl << endl;

}



void WGameCore::setHero(Player* hero)
{
	m_kdListener.removeObserver(m_phero);
    m_phero = hero;
	m_kdListener.addObserver(m_phero);
}

void WGameCore::waitQuit(DialogMenu* window)
{
    int t = 0, t0 = 0; //t et t0 sont les temps pour la gestion de la dur�e (SDL_PollEvent).
    while(window->isVisible())
    {
        t = SDL_GetTicks();
        if (t - t0 > 30) // Si 30 ms se sont �coul�es
        {
            this->graphicUpdate();
			m_gui.dialogRefresh();

            //G�re les �v�nements ext�rieurs (input)
            m_kdListener.updateEvents();

            //gere le mouvement al�atoire des mobs
            MobMovements();
            flip();

            t0 = t; // Le temps "actuel" devient le temps "precedent" pour nos futurs calculs
        }
        else // Si �a fait moins de 30ms depuis le dernier tour de boucle, on endort le programme le temps qu'il faut
            SDL_Delay(30 - (t - t0));
    }
}

//fonction qui renvoie un tableau d'ids d'entit�s entrant en contact avec une box donn�e (il peut y avoir plusieurs entit�s superpos�es et donc le tableau sera de taille > 1)
//dans chaque .x : id pok�dex
//dans chaque .y : num�ro de l'entit�
vector<SDL_Rect> WGameCore::detectEntity(SDL_Rect box)
{
    SDL_Rect posEvent;
    vector<SDL_Rect> id;
	list<Character*>& currentEntityList = m_EntityFactory.getNPCList();

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


	posEvent = m_EntityFactory.getTrainer()->getHitbox();
	if (CollisionBoxABoxB(box, posEvent))
	{
		SDL_Rect pos;
		pos.x = 0;
		pos.y = 0;
		pos.w = 0;
		pos.h = 0;
		id.push_back(pos);
	}


    if(m_fight.isFighting()) //Si on est en combat il faut aussi v�rifier qu'on n'entre pas en collision avec le pokemon adverse ou que le pok�mon adverse n'entre pas en collision avec nous
    {
        posEvent = m_fight.getOpponent()->getHitbox();

        if(CollisionBoxABoxB(box, posEvent))
        {
			SDL_Rect pos;
			pos.x = (Sint16)m_fight.getOpponent()->getID();
			pos.y = INT16_MAX;
			pos.w = 0;
			pos.h = 0;

            //ID de l'opposant en combat
            id.push_back(pos);
        }

		if(m_fight.getPokemon() != NULL)
		{ 
			posEvent = m_fight.getPokemon()->getHitbox();

			if(CollisionBoxABoxB(box, posEvent))
			{
				SDL_Rect pos;
				pos.x = (Sint16)m_fight.getPokemon()->getID();
				pos.y = INT16_MAX-1;
				pos.w = 0;
				pos.h = 0;
				//ID du Pok�mon en combat
				id.push_back(pos);
			}
		}
    }


    return id;

}

vector<SDL_Rect> WGameCore::detectEntity(SDL_Rect box, int direction)
{
    SDL_Rect buf = box;
    switch(direction)
    {
        case 0:
            buf.y += TAILLEBLOC/2 + 1;
        break;

        case 1:
            buf.x += TAILLEBLOC/2 + 1;
        break;

        case 2:
            buf.y -= TAILLEBLOC/2 + 1;
        break;

        case 3:
            buf.x -= TAILLEBLOC/2 + 1;
        break;

        case 4:
			buf.y += (int)(TAILLEBLOC / (2 * sqrt((float)2))) + 1;
			buf.x += (int)(TAILLEBLOC / (2 * sqrt((float)2))) + 1;
        break;

        case 5:
			buf.y -= (int)(TAILLEBLOC / (2 * sqrt((float)2))) + 1;
			buf.x += (int)(TAILLEBLOC / (2 * sqrt((float)2))) + 1;
        break;

        case 6:
			buf.y -= (int)(TAILLEBLOC / (2 * sqrt((float)2))) + 1;
			buf.x -= (int)(TAILLEBLOC / (2 * sqrt((float)2))) + 1;
        break;

        case 7:
			buf.y += (int)(TAILLEBLOC / (2 * sqrt((float)2))) + 1;
			buf.x -= (int)(TAILLEBLOC / (2 * sqrt((float)2))) + 1;
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
	//if(transition)
		//this->transition(0);


	//D�sallocation m�moire
	m_particleManager.stop();
	m_EntityFactory.deleteAll();

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






