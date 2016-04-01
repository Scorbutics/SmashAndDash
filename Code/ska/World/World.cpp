#include <cstdint>

#include "World.h"
#include "Layer.h"
#include "../Utils/StringUtils.h"
#include "../Utils/RectangleUtils.h"
#include "../Physic/PhysicObject.h"
#include "LayerE.h"
#include "../Graphic/System/CameraSystem.h"

using namespace std;

ska::World::World(const unsigned int tailleBloc, const unsigned int wWidth, const unsigned int wHeight) : 
//m_entityManager(entityManager), 
m_blockSize(tailleBloc), 
m_animBlocks(375, 4, true, 0, 0, tailleBloc, tailleBloc)
{
}

void ska::World::linkCamera(CameraSystem* cs) {
	m_cameraSystem = cs;
	if (m_cameraSystem != NULL) {
		m_cameraSystem->worldResized(getPixelWidth(), getPixelHeight());
	}
}

void ska::World::load(string fileName, string chipsetName)
{
	//m_bgm = NULL;
	m_chipset.load(chipsetName, DEFAULT_T_RED, DEFAULT_T_GREEN, DEFAULT_T_BLUE);
	m_chipsetName = chipsetName;
	m_fileName = fileName;
	m_genericName = fileName.substr(0, fileName.find_last_of('.'));
	m_worldName = m_genericName.substr(m_genericName.find_last_of('/') + 1, m_genericName.size());
	m_botLayerName = "."FILE_SEPARATOR"Levels"FILE_SEPARATOR + fileName + FILE_SEPARATOR + fileName + ".bmp";
	m_midLayerName = "."FILE_SEPARATOR"Levels"FILE_SEPARATOR + fileName + FILE_SEPARATOR + fileName + "M.bmp";
	m_topLayerName = "."FILE_SEPARATOR"Levels"FILE_SEPARATOR + fileName + FILE_SEPARATOR + fileName + "T.bmp";
	m_eventLayerName = fileName + "E.txt";
	m_windDirection = WIND_STOP;

	m_nbrBlockX = 0;
	m_nbrBlockY = 0;

	m_lBot = LayerPtr(new Layer(*this, m_botLayerName, chipsetName));
	m_lMid = LayerPtr(new Layer(*this, m_midLayerName, chipsetName, m_lBot.get()));
	m_lTop = LayerPtr(new Layer(*this, m_topLayerName, chipsetName, m_lMid.get()));

	layerE = LayerEPtr(new LayerE(*this, m_eventLayerName));

	if (m_cameraSystem != NULL) {
		m_cameraSystem->worldResized(getPixelWidth(), getPixelHeight());
	}
	getData();

}

ska::Texture& ska::World::getChipset()
{
	return m_chipset;
}

void ska::World::setWind(int wind) {
	m_windDirection = wind;
}

int ska::World::getWind() const {
	return m_windDirection;
}

ska::Animation& ska::World::getChipsetAnimation() {
	return m_animBlocks;
}

bool ska::World::isBlockDodgeable(const int i, const int j)
{
	return (m_lMid->getBlock(i, j)->getProperties() == BLOCK_PROP_JUMPWALL);
}

bool ska::World::getCollision(const int i, const int j)
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

bool ska::World::canMoveToPos(ska::Rectangle hitbox) {
	ska::Point<int> chd, chg, cbg;


	//position coin haut droit hitbox
	chd.x = hitbox.x + hitbox.w;
	chd.y = hitbox.y ;

	//position coin haut gauche hitbox
	chg.x = hitbox.x;
	chg.y = hitbox.y;

	//position coin bas gauche hitbox
	cbg.x = hitbox.x;
	cbg.y = hitbox.y + hitbox.h;

	const int yLimit = cbg.y / m_blockSize;
	const int xLimit = chd.x / m_blockSize;

	for (int y = chg.y / m_blockSize; y <= yLimit; y++) {
		for (int x = chg.x / m_blockSize; x <= xLimit; x++) {
			if (getCollision(x, y) /*&& !isBlockDodgeable(i, j)*/) {
				return false;
			}
		}
	}
	return true;
}

const ska::Rectangle* ska::World::getView() const {
	return m_cameraSystem == NULL ? NULL : m_cameraSystem->getDisplay();
}

ska::LayerPtr& ska::World::getLayerBot()
{
	return m_lBot;
}

ska::LayerPtr& ska::World::getLayerMid()
{
	return m_lMid;
}

ska::LayerPtr& ska::World::getLayerTop()
{
	return m_lTop;
}

ska::LayerEPtr& ska::World::getLayerEvent()
{
	return layerE;
}

string ska::World::getName()
{
    return m_worldName;
}

string ska::World::getGenericName()
{
    return m_genericName;
}

string ska::World::getChipsetName()
{
    return m_chipsetName;
}

void ska::World::changeLevel(string fileName, string chipsetname)
{
	//WGameCore& wScreen = WGameCore::getInstance();
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

	//wScreen.getParticleManager().removeAll();							//Suppression des particules

    m_lBot->reset(m_botLayerName, chipsetname);
	m_lMid->reset(m_midLayerName, chipsetname);
	m_lTop->reset(m_topLayerName, chipsetname);
    
	layerE->changeLevel(m_genericName + "E.txt");

	if (m_cameraSystem != NULL) {
		m_cameraSystem->worldResized(getPixelWidth(), getPixelHeight());
	}

	getData();
		
}

ska::Block* ska::World::getHigherBlock(const unsigned int i, const unsigned int j)
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

void ska::World::setBgm(string bgm)
{
	//WGameCore& wScreen = WGameCore::getInstance();

    if(m_bgmName != bgm)
        m_bgmName = bgm;
    else
        return;

    /*if(m_bgm != NULL)
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
    }*/
}

void ska::World::setBgmVolume(int volPercent)
{
    /*FMOD_SOUNDGROUP* sg;
	WGameCore& wScreen = WGameCore::getInstance();
    FMOD_System_GetMasterSoundGroup(wScreen.getMusicSystem(), &sg);
    FMOD_SoundGroup_SetVolume(sg, (float)((volPercent*255)/100.));*/
}


void ska::World::playBgm(bool x)
{
	//WGameCore& wScreen = WGameCore::getInstance();
    //Gestion audio
    /*FMOD_System_GetMasterChannelGroup(wScreen.getMusicSystem(), &m_canal);

    if(x && !this->isBgmPlaying())
        FMOD_ChannelGroup_SetPaused(m_canal, 0);
    else if(!x && this->isBgmPlaying())
        FMOD_ChannelGroup_SetPaused(m_canal, 1);*/

}

bool ska::World::isBgmPlaying()
{
    /*FMOD_BOOL etat;
    FMOD_ChannelGroup_GetPaused(m_canal, &etat);

    if(etat == 1)
        return false;
    else
        return true;*/
	return false;
}


int ska::World::getNbrBlocX()
{
    return m_nbrBlockX;
}

unsigned int ska::World::getPixelWidth() const {
	return m_nbrBlockX*m_blockSize;
}

int ska::World::getNbrBlocY()
{
	return m_nbrBlockY;
}

unsigned int ska::World::getPixelHeight() const {
	return m_nbrBlockY*m_blockSize;
}

const unsigned int ska::World::getBlockSize() const {
	return m_blockSize;
}

void ska::World::setNbrBlocX(int nbrBlockX) {
	m_nbrBlockX = nbrBlockX;
}

void ska::World::setNbrBlocY(int nbrBlockY) {
	m_nbrBlockY = nbrBlockY;
}

void ska::World::getFogFromData(string stringDataFile)
{
    /*string sprite;
	int number, xintensity, yintensity;
	bool transparency;
    int alpha;

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
	m_brouillard->hide(false);*/

}

void ska::World::getRainFromData(string stringDataFile)
{
    int idsprite, acceleration, density;
	//WGameCore& wScreen = WGameCore::getInstance();
	IniReader reader(stringDataFile);

    if(reader.getString("Rain id_sprite") != "STRINGNOTFOUND")
    {
		idsprite = reader.getInt("Rain id_sprite");
		acceleration = reader.getInt("Rain acceleration");
		density = reader.getInt("Rain density");

		//wScreen.getRainParticleManager().playRain(idsprite, (float)acceleration, (float)(density / 100.), 0);
    }
    else
    {
        clog << "La pluie est inexistante sur cette map" << endl;
    }
}


void ska::World::getWeatherFromData(string stringDataFile)
{
    /*string sprite;
	int number, xintensity, yintensity;
    bool alpha, transparency;
	IniReader reader(stringDataFile);
	//WGameCore& wScreen = WGameCore::getInstance();

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
        clog << "Le temps est inexistant sur cette map" << endl;
        m_temps->hide(true);
    }*/

}

void ska::World::getBgmFromData(string stringDataFile)
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
        clog << "Il n'y a pas de musique de fond sur cette map" << endl;
    }

}

void ska::World::getMobSettingsFromData()
{
	m_mobSettings.clear();

	unsigned int i = 0;
	do 
	{
		m_mobSettings.push_back(IniReader( "."FILE_SEPARATOR"Levels"FILE_SEPARATOR"" + m_genericName + ""FILE_SEPARATOR"Monsters"FILE_SEPARATOR"" + ska::StringUtils::intToStr(i) + ".ini"));
		i++;
	} while(m_mobSettings[i-1].isLoaded());

	//le dernier élément est invalide, on le supprime donc
	m_mobSettings.pop_back();

}

void ska::World::getData()
{
	//WGameCore& wScreen = WGameCore::getInstance();
	/*m_brouillard = unique_ptr<Weather>(new Weather("", 1, 100, 0, 0, WEATHER_ALPHA_LVL));
	m_temps = unique_ptr<Weather>(new Weather("", 1, 100, 0, 0, WEATHER_ALPHA_LVL));*/

    string stringDataFile = "."FILE_SEPARATOR"Levels"FILE_SEPARATOR"" + m_genericName + ""FILE_SEPARATOR"" + m_genericName + ".ini";

    //this->getWeatherFromData(stringDataFile);
    this->getFogFromData(stringDataFile);
    this->getBgmFromData(stringDataFile);
    this->getRainFromData(stringDataFile);
	this->getMobSettingsFromData();
	//wScreen.getMobSpawningManager().spawnOnWorld(20);

    this->playBgm(true);

    /*m_brouillard->resetPos();
    m_brouillard->setMosaicEffect(true);*/

}
/*
Weather* ska::World::getWeather()
{
    return &(*m_temps);
}

Weather* ska::World::getFog()
{
    return &(*m_brouillard);
}
*/
ska::World::~World()
{
    //ScriptsActiver(m_genericName); //Réactualisation des scripts
    //FMOD_Sound_Release(m_bgm);
}

vector<ska::IniReader>& ska::World::getMobSettings()
{
	return m_mobSettings;
}

