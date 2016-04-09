#include <math.h>
#include <fstream>
#include <limits.h>

#define OPEN_DELAY 50 //durée 50 frames

#include "../Utils/IDs.h"
#include "Pokeball.h"
#include "../Gameplay\WGameCore.h"
#include "../Utils\ChargementImages.h"
#include "Data\Statistics.h"
#include "Weather.h"
#include "../ska/Graphic/Draw/VectorDrawableContainer.h"
#include "../ska/Utils/RectangleUtils.h"


Pokeball::Pokeball() :
m_pokeballPos(0, 0),
m_finalPos(0 , 0),
m_gestionAnim(125, 4, false), 
m_gestionAnimVortex(175, 2, false) {
	m_gestionAnim.setOffsetAndFrameSize({ 0, 0, 16, 16 });
	m_gestionAnimVortex.setOffsetAndFrameSize({ 0, 0, 64, 64 });
    m_countOpenned = m_countOpenning = OPEN_DELAY;

	m_a = m_b = m_c = 0;
    m_power = -75; //"puissance" du lancer d'une pokeball par un dresseur, par defaut, -75
    m_speed = 3;
    m_isOpenning = m_isInactive = m_show = false;

	setSprites(
		"."FILE_SEPARATOR"Sprites"FILE_SEPARATOR"Fight"FILE_SEPARATOR"pokeball.png", 
		"."FILE_SEPARATOR"Sprites"FILE_SEPARATOR"Fight"FILE_SEPARATOR"pokeball-openned.png", 
		"."FILE_SEPARATOR"Sprites"FILE_SEPARATOR"Fight"FILE_SEPARATOR"pokeball-aura.png");
}

void Pokeball::setSprites(const std::string& spriteName, const std::string& spriteOpenPokeball, const std::string& spritePokeballAura) {
	if (spriteName.size() != 0) {
		m_sprite.load(spriteName);
	}
    

	if (spriteOpenPokeball.size() != 0) {
		m_openPokeball.load(spriteOpenPokeball);
	}
    

	if (spritePokeballAura.size() != 0) {
		m_vortex.load(spritePokeballAura, DEFAULT_T_RED, DEFAULT_T_GREEN, DEFAULT_T_BLUE, 128);
	}
    
}


void Pokeball::launch(const ska::Point<int>& src, const ska::Point<int>& destPos, PokeballLaunchReason::Enum launchReason) {
	m_capture = launchReason;
    
	/* Si déjà en cours de lancement de la Pokéball, on annule */
	if (m_show) {
		return;
	}

	m_pokeballPos = src;
	m_finalPos = destPos;

	
	ska::Point<int> leftPos;
	ska::Point<int> rightPos;
	if (m_pokeballPos.x < m_finalPos.x) {
		leftPos = m_pokeballPos;
		rightPos = m_finalPos;
		m_sens = 1;
	} else {
		rightPos = m_pokeballPos;
		leftPos = m_finalPos;
		m_sens = 0;
	}


	//pente de la droite passant par la mousePos et la pos centrale du hero
	float p;
	if (rightPos.x != leftPos.x) {
		p = (float)(rightPos.y - leftPos.y) / (rightPos.x - leftPos.x);
	} else {
		p = 100000;
	}
	
    m_speed = abs(m_finalPos.x - m_pokeballPos.x)/30 + 1; //on adapte la vitesse à la distance à parcourir pour éviter de privilégier les lancers de pokeball courts.


    //calcul (au préalable fait sur papier) des coefficients de la parabole en fonction de la position du heros et de l'endroit voulu du lancer.
    //théorique :
    //m_a = - (float) ((p*(xD - xF) + 2*m_power) + 2*sqrt(m_power*(m_power + p*(xD - xF)))) / ((xD - xF)*(xD - xF));


    //experimental (théorème de l'arrangement) :
	m_a = -(float)5 * ((leftPos.x - rightPos.x + 2 * m_power) + 2 * sqrt((float)(abs(m_power*(m_power + rightPos.x - leftPos.x))))) / ((rightPos.x - leftPos.x)*(rightPos.x - leftPos.x));
	m_b = p - m_a*(rightPos.x + leftPos.x);
	m_c = leftPos.y - m_a*leftPos.x*leftPos.x - m_b*leftPos.x;

    m_show = true;

}

void Pokeball::capture(/*Character* pkmn*/) {
	WGameCore& wScreen = WGameCore::getInstance();
	ska::World& w = wScreen.getWorld();
	ska::Rectangle oRel = { 0 };


	/* Oh mon dieu... */
	
	/*for(unsigned int i = 0; i < TAILLEBLOC; i++)
	{
		m_pokeballPos.x++;
		m_pokeballPos.y--;
		ska::Rectangle buf = m_pokeballPos, animPos = m_gestionAnim.getRectOfCurrentFrame();

		buf.x += oRel.x;
		buf.y += oRel.y;

		ska::VectorDrawableContainer drawables;
		//w.graphicUpdate(drawables);
		m_sprite.render(buf.x, buf.y, &animPos);

		/*		wScreen.getParticleManager().refresh();
		wScreen.getParticleManager().display(PARTICLE_MANAGER_CRUMBLING);
		wScreen.getParticleManager().display(PARTICLE_MANAGER_RAIN);
		wScreen.getParticleManager().displayRainFog();*/

		//Affiche la météo
		/*if(*wScreen.getSettings().getFogActive())
			w.getFog()->display();
		if(*wScreen.getSettings().getWeatherActive())
			w.getWeather()->display();*/


	/*	wScreen.flip();
		SDL_Delay(30);
	}

	pkmn->setVisible(false);
	unsigned int delay;
	unsigned int lastTime;
	unsigned int randomChanceCapture = 1;

	delay = 0;
	lastTime = SDL_GetTicks();
	while(delay <= 500)
	{
		ska::Rectangle buf = m_pokeballPos, animPos = m_gestionAnim.getRectOfCurrentFrame();
		buf.x += oRel.x;
		buf.y += oRel.y;
		m_pokeballPos.y+=4;

		ska::VectorDrawableContainer drawables;
		//w.graphicUpdate(drawables);
		m_sprite.render(buf.x, buf.y, &animPos);
			

		buf.x -= m_vortex.getWidth()/4; //sur 4 parce que l'image est composée de 2 sous-images pour l'animation
		buf.y -= m_vortex.getHeight()/4;

		ska::Rectangle animVortexPos = m_gestionAnimVortex.getRectOfCurrentFrame();
		m_vortex.render(buf.x, buf.y, &animVortexPos);
        

		/*wScreen.getParticleManager().refresh();
		wScreen.getParticleManager().display(PARTICLE_MANAGER_CRUMBLING);
		wScreen.getParticleManager().display(PARTICLE_MANAGER_RAIN);
		wScreen.getParticleManager().displayRainFog();*/

		//Affiche la météo
		/*if(*wScreen.getSettings().getFogActive())
			w.getFog()->display();
		if(*wScreen.getSettings().getWeatherActive())
			w.getWeather()->display();*/

	/*	wScreen.flip();
		SDL_Delay(30);
		delay = SDL_GetTicks() - lastTime;
	}




	//A partir de là on va faire "tourner" la pokéball pour les tentatives de capture
	for(unsigned int i = 0; i < 3 && randomChanceCapture != 0; i++)
	{
		delay = 0;
		lastTime = SDL_GetTicks();
		randomChanceCapture = rand()%((int)((float)3*pkmn->getStatistics()->getHpMax()/pkmn->getHp()) - 0 + 1) + 0;
		while(delay <= 3000)
		{
			ska::Rectangle buf = m_pokeballPos, animPos;
			buf.x += oRel.x;
			buf.y += oRel.y;

			animPos.y = 0;
			animPos.w = 16;
			animPos.h = 16;
			if(delay <= 600)
				animPos.x = 0;
			else if(delay <= 1200)
				animPos.x = 16;
			else if(delay <= 1800)
				animPos.x = 0;
			else if(delay <= 2400)
				animPos.x = 3*16;
			else
				animPos.x = 0;

		//	ska::VectorDrawableContainer drawables;
			//w.graphicUpdate(drawables);
		//	m_sprite.render(buf.x, buf.y, &animPos);


			/*wScreen.getParticleManager().refresh();
			wScreen.getParticleManager().display(PARTICLE_MANAGER_CRUMBLING);
			wScreen.getParticleManager().display(PARTICLE_MANAGER_RAIN);
			wScreen.getParticleManager().displayRainFog();*/
			//Affiche la météo
			/*if(*wScreen.getSettings().getFogActive())
				w.getFog()->display();
			if(*wScreen.getSettings().getWeatherActive())
				w.getWeather()->display();*/



		/*	wScreen.flip();
			SDL_Delay(30);
			delay = SDL_GetTicks() - lastTime;
		}
	}

	if(randomChanceCapture == 0)
		pkmn->setVisible(true);
	else
	{
		wScreen.getPokemonManager().add(pkmn);
		/*wScreen.getFight().end(EndFightReason::Capture);*/
	//}

}

void Pokeball::setPos(int x, int y) {
    m_pokeballPos.x = x;
    m_pokeballPos.y = y;
}

const ska::Point<int>& Pokeball::getPos() {
    return m_pokeballPos;
}

void Pokeball::display() {
	/* En fait tout ce truc dégueulasse en bas c'est une pseudo machine à état qui marche avec des variables membres booléennes.
	   Mais les vrais font ça avec des classes bien implémentées héritant d'une base commune et un pointeur sur l'état courant
	   qui refresh le bon état par polymorphisme. TODO ! */

	WGameCore& wScreen = WGameCore::getInstance();
	ska::World& w = wScreen.getWorld();

	if (!m_show) {
		return;
	}    

	vector<ska::Rectangle> ids;
	
	//Si la Pokeball est en l'air
	if ((m_pokeballPos.x > m_finalPos.x &&  m_sens == 0) || (m_pokeballPos.x < m_finalPos.x &&  m_sens == 1)) {
		ska::Rectangle animPos = m_gestionAnim.getRectOfCurrentFrame(), oRel = { 0 };

		if (m_pokeballPos.x < m_finalPos.x) {
			m_pokeballPos.x += m_speed;
		} else {
			m_pokeballPos.x -= m_speed;
		}

		//calcul des coordonnées à l'aide de l'équation de parabole préalablement calculée dans Pokeball::launch
        m_pokeballPos.y = (int) (m_a*m_pokeballPos.x*m_pokeballPos.x + m_b*m_pokeballPos.x + m_c); 

		m_sprite.render(m_pokeballPos.x, m_pokeballPos.y, &animPos);
    } else if (!m_isInactive)  {
		//sinon lorsqu'elle tombe par terre
		m_isOpenning = true;
	}



    if(m_isOpenning)  {
		//Si la Pokeball est en statut d'ouverture, on l'affiche ouverte ainsi que son aura violette (statut présent pour raison de fluidité de l'animation)
		ska::Rectangle animVortexPos = m_gestionAnimVortex.getRectOfCurrentFrame();
		ska::Point<int> buf = m_pokeballPos;

		m_openPokeball.render(buf.x, buf.y);

		//sur 4 parce que l'image est composée de 2 sous-images pour l'animation
        buf.x -= m_vortex.getWidth()/4; 
        buf.y -= m_vortex.getHeight()/4;
		m_vortex.render(buf.x, buf.y, &animVortexPos);
        
		if(m_capture == PokeballLaunchReason::Capture) {
			ska::Rectangle boxCapture = { m_pokeballPos.x, m_pokeballPos.y, 0, 0 };
			boxCapture.x -= TAILLEBLOC;
			boxCapture.y -= TAILLEBLOC;
			boxCapture.w = 2*TAILLEBLOC;
			boxCapture.h = 2*TAILLEBLOC;

			//TODO capture

			/*ids = wScreen.detectEntity(boxCapture);*/
			const size_t idSize = ids.size();
			for (size_t i = 0; i < idSize; i++) {
				if (ids[i].y == ID_CURRENT_OPPONENT) {
					m_capture = PokeballLaunchReason::Recall;
					capture(/*wScreen.getFight().getOpponent()*/);
					return;
				}
			}

		}

        m_countOpenning--;
        if(m_countOpenning <= 0) {
            m_countOpenning = OPEN_DELAY;
            m_isOpenning = false;
            m_isInactive = true;
        }

    } else if(m_isInactive) {
		//Si la Pokeball est ouverte, inactive (statut présent pour raison de fluidité de l'animation) 
		m_openPokeball.render(m_pokeballPos.x, m_pokeballPos.y);

        m_countOpenned--;
        if(m_countOpenned <= 0) {
            m_countOpenned = OPEN_DELAY;
            m_isInactive = false;
            m_show = false;
        }
        m_capture = PokeballLaunchReason::Recall;
    }


}

int Pokeball::getStatus() const {
	return (m_isOpenning) ? 0 : 1;
}

void Pokeball::hide(bool b)
{
    m_show = !b;
}

bool Pokeball::isVisible() const
{
    return m_show;
}

Pokeball::~Pokeball()
{
}
