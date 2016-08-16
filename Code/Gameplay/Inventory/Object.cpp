#include "Object.h"
#include <fstream>
#include "../../ska/Inputs/Readers/IniReader.h"
#include "../../ska/Utils\StringUtils.h"
#include "../../Utils/IDs.h"

Object::Object(int id): m_animation(525, 2, false)
{
	m_id = id;
	const std::string& strId = ska::StringUtils::intToStr(m_id);
	ska::IniReader reader("."FILE_SEPARATOR"Data"FILE_SEPARATOR"Items"FILE_SEPARATOR + strId  + ".ini");
    m_name = reader.get<std::string>("Description name");
	m_description = reader.get<std::string>("Description description");
	m_effect = reader.get<std::string>("Effect target");
	m_action = reader.get<std::string>("Effect action");
	m_spriteName = "."FILE_SEPARATOR"Sprites"FILE_SEPARATOR"Inventory"FILE_SEPARATOR + strId + ".png";
	m_sprite.load(m_spriteName , DEFAULT_T_RED, DEFAULT_T_GREEN, DEFAULT_T_BLUE);
    
    m_pos.x = m_pos.y = 0;
	
	//2 : nombre d'animations dans une seule image
    m_width = m_sprite.getWidth()/2; 
    m_height = m_sprite.getHeight();
    
	//initialisation de l'animation
	ska::Rectangle tmp = { m_pos.x, m_pos.y, m_width, m_height };
	m_animation.setOffsetAndFrameSize(tmp); 
}


void Object::display()
{
	ska::Rectangle animOffset = m_animation.getRectOfCurrentFrame(); //r�cup�ration de la partie courante de l'image pour cr�er un effet d'animation
	m_sprite.render(m_pos.x, m_pos.y, &animOffset);
}

unsigned int Object::getWidth()
{
	return m_width;
}

std::string Object::getSpriteName()
{
    return m_spriteName;
}

unsigned int Object::getHeight()
{
	return m_height;
}

void Object::setPos(int x, int y)
{
    m_pos.x = x;
    m_pos.y = y;
}

//utilisation d'un objet sans besoin de s�lectionner un pok�mon
bool Object::use()
{

    if(m_action[0] == '+')
    {
		if (std::string::npos != m_action.find("heal"))
        {
			std::string pv = m_action.substr(m_action.find("+"), m_action.find("heal"));
        }
    }
    else if(m_action == "capture")
    {
		//WGameCore& wScreen = WGameCore::getInstance();

		/*if(wScreen.getPokeball().isVisible() || wScreen.getPokemonManager().getPokemonTeamSize() >= POKEMON_TEAM_MAX_SIZE)
			return false;*/

		/*const ska::InputRange& mousePos = WGameCore::getInstance().getRanges()[ska::InputRangeType::MousePos];
		ska::Point<int> pos = mousePos;*/
/*
		pos.x -= wScreen.getORel().x;
		pos.y -= wScreen.getORel().y;*/
		//wScreen.getPokeball().launch(wScreen.getEntityFactory().getTrainer(), pos, PokeballLaunchReason::Capture);
    }

	return true;

}

//utilisation choisie sur le pok�mon num�ro i de l'�quipe
bool Object::use(int i)
{
	//WGameCore& wScreen = WGameCore::getInstance();
    if(m_action[0] == '+')
    {
		if (std::string::npos != m_action.find("heal"))
        {
			std::string pv = m_action.substr(m_action.find("+"), m_action.find("heal"));
			/*if (wScreen.getPokemonManager().getPokemon(i)->isAlive())
				wScreen.getPokemonManager().getPokemon(i)->setHP(wScreen.getPokemonManager().getPokemon(i)->getHp() + atoi(pv.c_str()));*/
        }
    }
    else
    {

    }
	return true;
}

const ska::Point<int>& Object::getPos()
{
    return m_pos;
}

ska::Texture* Object::getSprite()
{
    return &m_sprite;
}

ska::Animation* Object::getAnimation()
{
    return &m_animation;
}

std::string Object::getDescription()
{
    return m_description;
}

std::string Object::getName()
{
    return m_name;
}

std::string Object::getEffect()
{
    return m_effect;
}

std::string Object::getAction()
{
    return m_action;
}

int Object::getID()
{
    return m_id;
}

Object::~Object()
{
}
