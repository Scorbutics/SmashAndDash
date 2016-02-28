#include "Object.h"
#include "../../Gameplay\WGameCore.h"
#include <fstream>
#include "../../Utils\StringUtils.h"

using namespace std;

Object::Object(int id): m_animation(525, 2, false)
{
	m_id = id;
	const std::string& strId = ska::StringUtils::intToStr(m_id);
	IniReader reader("."FILE_SEPARATOR"Data"FILE_SEPARATOR"Items"FILE_SEPARATOR + strId  + ".ini");
    m_name = reader.getString("Description name");
	m_description = reader.getString("Description description");
	m_effect = reader.getString("Effect target");
	m_action = reader.getString("Effect action");
	m_spriteName = "."FILE_SEPARATOR"Sprites"FILE_SEPARATOR"Inventory"FILE_SEPARATOR + strId + ".png";
	m_sprite.load(m_spriteName , T_RED, T_GREEN, T_BLUE);
    
    m_pos.x = m_pos.y = 0;
    m_pos.w = m_sprite.getWidth()/2; //2 : nombre d'animations dans une seule image
    m_pos.h = m_sprite.getHeight();
    

    m_animation.setOffsetAndFrameSize(m_pos); //initialisation de l'animation
}


void Object::display()
{
	ska::Rectangle animOffset = m_animation.getRectOfCurrentFrame(); //récupération de la partie courante de l'image pour créer un effet d'animation
	m_sprite.render(m_pos.x, m_pos.y, &animOffset);
}

unsigned int Object::getWidth()
{
    return m_pos.w;
}

string Object::getSpriteName()
{
    return m_spriteName;
}

unsigned int Object::getHeight()
{
    return m_pos.h;
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

		ska::Rectangle mousePos = wScreen.getInputListener().getMouseInput()->getMousePos();
		mousePos.x -= wScreen.getORel().x;
		mousePos.y -= wScreen.getORel().y;
		wScreen.getPokeball().launch(wScreen.getEntityFactory().getTrainer(), mousePos, PokeballLaunchReason::Capture);
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

void Object::setPos(ska::Rectangle pos)
{
    m_pos.x = pos.x;
    m_pos.y = pos.y;
}

const ska::Rectangle* Object::getPos()
{
    return &m_pos;
}

Texture* Object::getSprite()
{
    return &m_sprite;
}

Animation* Object::getAnimation()
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
