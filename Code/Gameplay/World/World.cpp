#include <stdint.h>

#include "World.h"
#include "..\WGameCore.h"
#include "Layer.h"
#include "Utils\StringUtils.h"
#include "Utils\ChargementImages.h"
#include "LayerE.h"


using namespace std;

World::World() : m_animBlocks(375, 4, true, 0, 0, TAILLEBLOC, TAILLEBLOC)
{
}

void World::load(string fileName, string chipsetName, int windowWidth, int windowHeight)
{
	m_bgm = NULL;
	m_chipset.load(chipsetName, T_RED, T_GREEN, T_BLUE);
	m_chipsetName = chipsetName;
	m_fileName = fileName;
	m_genericName = fileName.substr(0, fileName.find_last_of('.'));
	m_worldName = m_genericName.substr(m_genericName.find_last_of('/') + 1, m_genericName.size());
	m_botLayerName = "."FILE_SEPARATOR"Levels"FILE_SEPARATOR + fileName + FILE_SEPARATOR + fileName + ".bmp";
	m_midLayerName = "."FILE_SEPARATOR"Levels"FILE_SEPARATOR + fileName + FILE_SEPARATOR + fileName + "M.bmp";
	m_topLayerName = "."FILE_SEPARATOR"Levels"FILE_SEPARATOR + fileName + FILE_SEPARATOR + fileName + "T.bmp";
	m_eventLayerName = fileName + "E.txt";

	m_lBot = unique_ptr<Layer>(new Layer(m_botLayerName, chipsetName, windowWidth, windowHeight));
	m_lMid = unique_ptr<Layer>(new Layer(m_midLayerName, chipsetName, windowWidth, windowHeight));
	m_lTop = unique_ptr<Layer>(new Layer(m_topLayerName, chipsetName, windowWidth, windowHeight));

	m_lEvent = unique_ptr<LayerE>(new LayerE(m_eventLayerName));
}

Texture* World::getChipset()
{
	return &m_chipset;
}

void World::displayLayers()
{
	SDL_Rect rectAnimBlocks = m_animBlocks.getRectOfCurrentFrame();
	WGameCore& wScreen = WGameCore::getInstance();

	//Liste de tous les personnages sur le monde courant
	list<Character*>& currentEntityList = wScreen.getEntityFactory().getCharacterList();

	//Première couche
	m_lBot->display(rectAnimBlocks);

	//Deuxième couche
	m_lMid->display(rectAnimBlocks);

	//Affichage des effets
	wScreen.getParticleManager().display(PARTICLE_MANAGER_EFFECT);

	//Curseur souris sur la map
	if(wScreen.getMouseCursor().isActiveCursor())
		wScreen.getMouseCursor().displayCursor();


	//Première partie des personnages
	for (Character* npc : currentEntityList)
	{
		if (npc->isVisible())
			npc->display(1);
	}


	//Ajouter un layer ici serait sympa ! (layer entre mid et top, qui cache le bas des persos mais pas le haut)

	//Deuxième partie des personnages (ceux au sol)
	for (Character* npc : currentEntityList)
	{
		if (npc->isVisible() && npc->getJumpHeight() < TAILLEBLOC)
			npc->display(2);
	}

	//Troisième couche
	m_lTop->display(rectAnimBlocks);

	//Deuxième partie des personnages (ceux en l'air)
	for (Character* npc : currentEntityList)
	{
		if (npc->isVisible() && npc->getJumpHeight() >= TAILLEBLOC)
			npc->display(2, false);
	}

}



void World::refreshEntities()
{
	WGameCore& wScreen = WGameCore::getInstance();


	//On refresh tous les personnages
	auto it = wScreen.getEntityFactory().getCharacterList().begin();
	while (it != wScreen.getEntityFactory().getCharacterList().end())
	{
		Character* npc = (*it);
		if (npc->isVisible() && npc->isAlive())
			npc->refresh();
			
		//Si jamais un personnage n'est plus vivant ou est notre Pokémon en combat alors que le combat est terminé, on le supprime
		if (!(npc->getEntityNumber() == ID_CURRENT_POKEMON && !wScreen.getFight().isFighting()))
			it++;
		else
			it = wScreen.getEntityFactory().getCharacterList().erase(it);
    }    


}

bool World::isBlockDodgeable(const int i, const int j)
{
	return (m_lMid->getBlock(i, j)->getProperties() == BLOCK_PROP_JUMPWALL);
}

bool World::getCollision(const int i, const int j)
{
	if (m_lBot == NULL)
		return false;

    if(m_lBot->getBlockCollision(i, j) == BLOCK_COL_YES &&			//Si on ne peut pas marcher sur le bot
        (m_lMid->getBlockCollision(i, j) != BLOCK_COL_NO))			//(ou inexistant)
        return true;

    if(m_lMid->getBlockCollision(i, j) == BLOCK_COL_YES)
        return true;

    return false;
}

bool World::canMoveToPos(SDL_Rect pos, PhysicObject* entityToMove)
{
    vector<SDL_Rect> ids;
    int id, entityNumber;
    bool ok = true;
	WGameCore& wScreen = WGameCore::getInstance();

    if(entityToMove != NULL)
    {
        id = entityToMove->getID();
        entityNumber = entityToMove->getEntityNumber();
	}
	else
	{
		id = 0;
		entityNumber = -1;
	}

    pos.w = 0;
    pos.h = 0;
    ids = wScreen.detectEntity(pos);
    const size_t size = ids.size();
    for(size_t i = 0; i < size; i++)
        if(!wScreen.getEntityFactory().getNPC(ids[i].x, ids[i].y)->getGhost() && (ids[i].x != id || ids[i].y != entityNumber)) //afin de ne pas tester la collision d'une entité avec elle-même...
        {
            ok = false;                                 // il suffit qu'il y ait au moins une entité sur le passage
        }
    

    if(ok == false)
        return ok;                                          //pour renvoyer false
    else if(this->getCollision(pos.x/TAILLEBLOC, pos.y/TAILLEBLOC))
        return false;
    else
        return true;
}

Layer* World::getLayerBot()
{
    return &(*m_lBot);
}

Layer* World::getLayerMid()
{
    return &(*m_lMid);
}

Layer* World::getLayerTop()
{
    return &(*m_lTop);
}

LayerE* World::getLayerEvent()
{
    return &(*m_lEvent);
}

string World::getName()
{
    return m_worldName;
}

string World::getGenericName()
{
    return m_genericName;
}

string World::getChipsetName()
{
    return m_chipsetName;
}

void World::changeLevel(string fileName, string chipsetname)
{
	WGameCore& wScreen = WGameCore::getInstance();
    m_lBot->clear();
    m_lTop->clear();
    m_lMid->clear();
	
    m_chipsetName = chipsetname;
	m_chipset.load(chipsetname);
    m_genericName = fileName.substr(0, fileName.find_last_of('.'));
    m_worldName = m_genericName.substr(m_genericName.find_last_of('/')+1, m_genericName.size());
    m_fileName = fileName;
    m_botLayerName = "."FILE_SEPARATOR"Levels"FILE_SEPARATOR"" + m_genericName + ""FILE_SEPARATOR"" + m_genericName + ".bmp";
    m_midLayerName = "."FILE_SEPARATOR"Levels"FILE_SEPARATOR"" + m_genericName + ""FILE_SEPARATOR"" + m_genericName + "M.bmp";
    m_topLayerName = "."FILE_SEPARATOR"Levels"FILE_SEPARATOR"" + m_genericName + ""FILE_SEPARATOR"" + m_genericName + "T.bmp";
    m_eventLayerName = m_genericName + "E.txt";

    //ScriptsActiver(m_genericName);									//Réactualisation des scripts
	wScreen.getParticleManager().removeAll();							//Suppression des particules

    m_lBot->reset(m_botLayerName, chipsetname);
    m_lTop->reset(m_topLayerName, chipsetname);
    m_lMid->reset(m_midLayerName, chipsetname);
	m_lEvent->changeLevel(m_genericName + "E.txt");

    //m_lBot->printCollisionProfile();
    //m_lMid->printCollisionProfile();
    //m_lTop->printCollisionProfile();

	wScreen.setContinue(1);
	wScreen.quitFlip();

}

/*
int World::spawnMob(SDL_Rect pos, unsigned int rmin, unsigned int rmax, float distanceSpawns, unsigned int idMob)
{
	if(distanceSpawns > 2*rmin)
	{
		cerr << "Erreur (class World) : lors d'un spawn, la distance entre deux spawns ne doit pas excéder le diamètre minimal (2*rmin)" << endl;
		return;
	}

	if(distanceSpawns == 0)
		return;

	World* w = wScreen.getWorld();
	float angle = 0;
	unsigned int radius;

	//attention, ce while bugue
	while(angle < 360 && angle > -360)
	{
		radius = rmin + rand()%(rmax - rmin + 1);
		SDL_Rect dest;
		dest.x = radius*cos(angle) + pos.x;
		dest.y = radius*sin(angle) + pos.y;
		dest.x = (dest.x/TAILLEBLOC) * TAILLEBLOC;
		dest.y = (dest.y/TAILLEBLOC) * TAILLEBLOC;

		if(canMoveToPos(dest, NULL))
			wScreen.getEntityFactory().addNPC(idMob, dest, intToStr(P_RANDOM));
		angle += acos(1 - ((distanceSpawns*distanceSpawns)/(2*rmin*rmin)));

	}
}
*/

//Fonction d'apparition d'un mob à une position pos
int World::spawnMob(SDL_Rect pos, unsigned int rmin, unsigned int rmax, unsigned int nbrSpawns, IniReader* dataSpawn)
{
	if(nbrSpawns == 0)
		return 0;

	WGameCore& wScreen = WGameCore::getInstance();

	vector<unsigned int> idBlocks;
	for (unsigned int i = 0; dataSpawn->get("Spawn on_blockid_" + StringUtils::intToStr(i)); i++)
		idBlocks.push_back(dataSpawn->getInt("Spawn on_blockid_" + StringUtils::intToStr(i)));

	unsigned int idMob = dataSpawn->getInt("Data id");
	World* w = this;
	float angle = (float) (( 2*M_PI* (rand()%360))/360);
	unsigned int radius;
	int successfulSpawns = 0;
	unique_ptr<Character> mob = unique_ptr<Character>(new Character(idMob));




	for(unsigned int i = 0; i < nbrSpawns; i++)
	{
		radius = rmin + rand()%(rmax - rmin + 1);

		SDL_Rect dest;
		dest.x = (int)(radius*cos(angle) + pos.x);
		dest.y = (int)(radius*sin(angle) + pos.y);
		dest.x = (dest.x/TAILLEBLOC) * TAILLEBLOC;
		dest.y = (dest.y/TAILLEBLOC) * TAILLEBLOC;

		SDL_Rect boxWorld, boxDest;
		boxWorld.x = 0;
		boxWorld.y = 0;
		boxWorld.w = getNbrBlocX()*TAILLEBLOC;
		boxWorld.h = getNbrBlocY()*TAILLEBLOC;
		boxDest.x = dest.x - radius;
		boxDest.y = dest.y - radius;
		boxDest.h = boxDest.w = 2*radius;

		if(canMoveToPos(dest, NULL) && IsPositionInBox(&dest, &boxWorld) && wScreen.detectEntity(boxDest).size() < SPAWN_LIMIT_ALLOWED)
		{
			bool spawnAllowed = true;
			for(unsigned int j = 0; j < idBlocks.size(); j++)
			{
				Block* b = getHigherBlock(dest.x/TAILLEBLOC, dest.y/TAILLEBLOC);
				if(b != NULL && b->getID() == idBlocks[i])
					spawnAllowed = false;
			} 
			

			if(spawnAllowed)
			{

				int level = rand()%(dataSpawn->getInt("Data level_min") + dataSpawn->getInt("Data level_max") +1) + dataSpawn->getInt("Data level_min");
				mob->getPath()->setPathString(dataSpawn->getString("Data path_type"));
				mob->setID(idMob);
				mob->teleport(dest.x, dest.y);
				mob->setOffset(0, 10);
				mob->setOffset(1, 20);
				mob->setOffset(2, 37);
				mob->setOffset(3, 20);

				wScreen.getEntityFactory().addNPC(&(*mob));
				successfulSpawns++;
			}
		}
		angle += (float)((2.0*M_PI) / nbrSpawns);

	}
	return successfulSpawns;
}

Block* World::getHigherBlock(const unsigned int i, const unsigned int j)
{
	Block* bBot = m_lBot->getBlock(i, j);
	Block* bMid = m_lMid->getBlock(i, j);
	Block* bTop = m_lTop->getBlock(i, j);

	if(bTop != NULL && m_lTop->getBlock(i, j)->getID() != BLOCK_ID_AIR)
		return m_lTop->getBlock(i, j);

	if(bMid != NULL && m_lMid->getBlock(i, j)->getID() != BLOCK_ID_AIR)
		return m_lMid->getBlock(i, j);

	if(bBot != NULL && m_lBot->getBlock(i, j)->getID() != BLOCK_ID_AIR)
		return m_lBot->getBlock(i, j);

	return NULL;
}

void World::setBgm(string bgm)
{
	WGameCore& wScreen = WGameCore::getInstance();

    if(m_bgmName != bgm)
        m_bgmName = bgm;
    else
        return;

    if(m_bgm != NULL)
        FMOD_Sound_Release(m_bgm);

    m_bgmResult = FMOD_System_CreateSound(wScreen.getMusicSystem(), bgm.c_str(), FMOD_SOFTWARE | FMOD_2D | FMOD_CREATESTREAM | FMOD_LOOP_NORMAL, 0, &m_bgm);

    if (m_bgmResult != FMOD_OK)
        cerr << "Impossible de lire le fichier mp3\n" << endl;
    else
    {
        FMOD_Sound_SetLoopCount(m_bgm, -1);
        FMOD_System_PlaySound(wScreen.getMusicSystem(), FMOD_CHANNEL_FREE, m_bgm, 0, NULL);
        FMOD_System_GetMasterChannelGroup(wScreen.getMusicSystem(), &m_canal);
        FMOD_ChannelGroup_SetPaused(m_canal, 1);
    }
}

void World::setBgmVolume(int volPercent)
{
    FMOD_SOUNDGROUP* sg;
	WGameCore& wScreen = WGameCore::getInstance();
    FMOD_System_GetMasterSoundGroup(wScreen.getMusicSystem(), &sg);
    FMOD_SoundGroup_SetVolume(sg, (float)((volPercent*255)/100.));
}


void World::playBgm(bool x)
{
	WGameCore& wScreen = WGameCore::getInstance();
    //Gestion audio
    FMOD_System_GetMasterChannelGroup(wScreen.getMusicSystem(), &m_canal);

    if(x && !this->isBgmPlaying())
        FMOD_ChannelGroup_SetPaused(m_canal, 0);
    else if(!x && this->isBgmPlaying())
        FMOD_ChannelGroup_SetPaused(m_canal, 1);

}

bool World::isBgmPlaying()
{
    FMOD_BOOL etat;
    FMOD_ChannelGroup_GetPaused(m_canal, &etat);

    if(etat == 1)
        return false;
    else
        return true;
}


int World::getNbrBlocX()
{
    return m_lBot->getNbrBlocX();
}

int World::getNbrBlocY()
{
    return m_lBot->getNbrBlocY();
}


void World::getFogFromData(string stringDataFile)
{
    string sprite;
	int number, xintensity, yintensity;
	bool transparency;
    int alpha;
    ofstream clog("stdlog.txt", ios::app);

	IniReader reader(stringDataFile);

	sprite = reader.getString("Fog sprite");
	xintensity = reader.getInt("Fog xintensity");
	yintensity = reader.getInt("Fog yintensity");
	number = reader.getInt("Fog number");
	transparency = reader.getBoolean("Fog transparency");


	if(sprite == "STRINGNOTFOUND")
	{
		clog << "Le brouillard est inexistant sur cette map" << endl;
		m_brouillard->hide(true);
		return;
	}


	if(transparency)
		alpha = WEATHER_ALPHA_LVL;
	else
		alpha = 255;


	m_brouillard = unique_ptr<Weather>(new Weather(sprite, number, 100, xintensity, yintensity, alpha));
	m_brouillard->hide(false);

}

void World::getRainFromData(string stringDataFile)
{
    int idsprite, acceleration, density;
    ofstream clog("stdlog.txt", ios::app);
	WGameCore& wScreen = WGameCore::getInstance();
	IniReader reader(stringDataFile);

    if(reader.getString("Rain id_sprite") != "STRINGNOTFOUND")
    {
		idsprite = reader.getInt("Rain id_sprite");
		acceleration = reader.getInt("Rain acceleration");
		density = reader.getInt("Rain density");

		wScreen.getParticleManager().playRain(idsprite, (float)acceleration, (float)(density / 100.), 0);
    }
    else
    {
        clog << "La pluie est inexistante sur cette map" << endl;
    }
}


void World::getWeatherFromData(string stringDataFile)
{
    string sprite;
	int number, xintensity, yintensity;
    bool alpha, transparency;
	IniReader reader(stringDataFile);
	WGameCore& wScreen = WGameCore::getInstance();

	sprite = reader.getString("Weather sprite");


    if(sprite != "STRINGNOTFOUND")
    {

        transparency = reader.getBoolean("Weather transparency");
		xintensity = reader.getInt("Weather xintensity");
		yintensity = reader.getInt("Weather yintensity");
		number = reader.getInt("Weather number");

        if(transparency)
            alpha = true;
        else
            alpha = false;

        m_temps = unique_ptr<Weather>(new Weather(sprite, number, 100, xintensity, yintensity, alpha));
        m_temps->hide(false);

    }
    else
    {
        ofstream clog("stdlog.txt", ios::app);
        clog << "Le temps est inexistant sur cette map" << endl;
        m_temps->hide(true);
    }

}

void World::getBgmFromData(string stringDataFile)
{
    string file;
	int volume;
	IniReader reader(stringDataFile);

	file = reader.getString("Bgm file");

    if(file != "STRINGNOTFOUND")
    {

        volume = reader.getInt("Bgm volume");
        this->setBgm(file);
		this->setBgmVolume(volume);

    }
    else
    {
        ofstream clog("stdlog.txt", ios::app);
        clog << "Il n'y a pas de musique de fond sur cette map" << endl;
    }

}

void World::getMobSettingsFromData()
{
	m_mobSettings.clear();

	unsigned int i = 0;
	do 
	{
		m_mobSettings.push_back(IniReader( "."FILE_SEPARATOR"Levels"FILE_SEPARATOR"" + m_genericName + ""FILE_SEPARATOR"Monsters"FILE_SEPARATOR"" + StringUtils::intToStr(i) + ".ini"));
		i++;
	} while(m_mobSettings[i-1].isLoaded());

	//le dernier élément est invalide, on le supprime donc
	m_mobSettings.pop_back();

}

void World::getData()
{
	WGameCore& wScreen = WGameCore::getInstance();
	m_brouillard = unique_ptr<Weather>(new Weather("", 1, 100, 0, 0, WEATHER_ALPHA_LVL));
	m_temps = unique_ptr<Weather>(new Weather("", 1, 100, 0, 0, WEATHER_ALPHA_LVL));

    string stringDataFile = "."FILE_SEPARATOR"Levels"FILE_SEPARATOR"" + m_genericName + ""FILE_SEPARATOR"" + m_genericName + ".ini";

    this->getWeatherFromData(stringDataFile);
    this->getFogFromData(stringDataFile);
    this->getBgmFromData(stringDataFile);
    this->getRainFromData(stringDataFile);
	this->getMobSettingsFromData();
	wScreen.getMobSpawningManager().spawnOnWorld(20);

    this->playBgm(true);

    m_brouillard->resetPos();
    m_brouillard->setMosaicEffect(true);

}

Weather* World::getWeather()
{
    return &(*m_temps);
}

Weather* World::getFog()
{
    return &(*m_brouillard);
}

World::~World()
{
    //ScriptsActiver(m_genericName); //Réactualisation des scripts
    FMOD_Sound_Release(m_bgm);
}

vector<IniReader>& World::getMobSettings()
{
	return m_mobSettings;
}

