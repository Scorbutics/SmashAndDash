#include "Object.h"
#include "../../Gameplay\WGameCore.h"
#include <fstream>
#include "../../ska/Utils\StringUtils.h"
#include "../../Utils/IDs.h"

using namespace std;

Object::Object(int id): m_animation(525, 2, false)
{
	m_id = id;
	const std::string& strId = ska::StringUtils::intToStr(m_id);
	ska::IniReader reader("."FILE_SEPARATOR"Data"FILE_SEPARATOR"Items"FILE_SEPARATOR + strId  + ".ini");
    m_name = reader.getString("Description name");
	m_description = reader.getString("Description description");
	m_effect = reader.getString("Effect target");
	m_action = reader.getString("Effect action");
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
	ska::Rectangle animOffset = m_animation.getRectOfCurrentFrame(); //récupération de la partie courante de l'image pour créer un effet d'animation
	m_sprite.render(m_pos.x, m_pos.y, &animOffset);
}

unsigned int Object::getWidth()
{
	return m_width;
}

string Object::getSpriteName()
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

//utilisation d'un objet sans besoin de sélectionner un pokémon
bool Object::use()
{

    if(m_action[0] == '+')
    {
        if(string::npos != m_action.find("heal"))
        {
            string pv = m_action.substr(m_action.find("+"), m_action.find("heal"));
        }
    }
    else if(m_action == "capture")
    {
		WGameCore& wScreen = WGameCore::getInstance();

		if(wScreen.getPokeball().isVisible() || wScreen.getPokemonManager().getPokemonTeamSize() >= POKEMON_TEAM_MAX_SIZE)
			return false;

		const ska::InputRange& mousePos = WGameCore::getInstance().getRanges()[ska::InputRangeType::MousePos];
		ska::Point<int> pos = mousePos;
/*
		pos.x -= wScreen.getORel().x;
		pos.y -= wScreen.getORel().y;*/
		//wScreen.getPokeball().launch(wScreen.getEntityFactory().getTrainer(), pos, PokeballLaunchReason::Capture);
    }

	return true;

}

//utilisation choisie sur le pokémon numéro i de l'équipe
bool Object::use(int i)
{
	WGameCore& wScreen = WGameCore::getInstance();
    if(m_action[0] == '+')
    {
        if(string::npos != m_action.find("heal"))
        {
            string pv = m_action.substr(m_action.find("+"), m_action.find("heal"));
			if (wScreen.getPokemonManager().getPokemon(i)->isAlive())
				wScreen.getPokemonManager().getPokemon(i)->setHP(wScreen.getPokemonManager().getPokemon(i)->getHp() + atoi(pv.c_str()));
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

string Object::getDescription()
{
    return m_description;
}

string Object::getName()
{
    return m_name;
}

string Object::getEffect()
{
    return m_effect;
}

string Object::getAction()
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
