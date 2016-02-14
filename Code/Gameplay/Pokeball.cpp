#include <stdint.h>
#include <math.h>
#include <fstream>
#include <limits.h>

#define OPEN_DELAY 50 //durée 50 frames

#include "Pokeball.h"
#include "Gameplay\WGameCore.h"
#include "Utils\ChargementImages.h"
#include "Data\Statistics.h"
#include "Weather.h"

using namespace std;


Pokeball::Pokeball(string spriteName, Character* hero, SDL_Rect destPos, SDL_Rect oRel) : m_gestionAnim(125, 4, false), m_gestionAnimVortex(175, 2, false)
{
    SDL_Rect animPos;
    animPos.x = 0;
    animPos.y = 0;
    animPos.w = 16;
    animPos.h = 16;
    m_gestionAnim.setOffsetAndFrameSize(animPos);
    animPos.w = 64;
    animPos.h = 64;
    m_gestionAnimVortex.setOffsetAndFrameSize(animPos);
    m_countOpenned = m_countOpenning = OPEN_DELAY;

    m_sprite.load(spriteName, T_RED, T_GREEN, T_BLUE);
    m_pokeballPos.x = hero->getCenterPos().x ;
    m_pokeballPos.y = hero->getPos().y ;
    m_finalPos.x = destPos.x + abs(oRel.x);
    m_finalPos.y = destPos.y + abs(oRel.y);
    m_a = 0;
    m_b = 0;
    m_c = 0;
    m_power = -75; //"puissance" du lancer d'une pokeball par un dresseur, par defaut, -75
    m_speed = 3;
    m_isOpenning = m_isInactive = m_show = false;

}


Pokeball::Pokeball() : m_gestionAnim(125, 4, false), m_gestionAnimVortex(175, 2, false)
{
    SDL_Rect animPos;
    animPos.x = 0;
    animPos.y = 0;
    animPos.w = 16;
    animPos.h = 16;
    m_gestionAnim.setOffsetAndFrameSize(animPos);
    animPos.w = 64;
    animPos.h = 64;
    m_gestionAnimVortex.setOffsetAndFrameSize(animPos);
    m_countOpenned = m_countOpenning = OPEN_DELAY;

    m_pokeballPos.x = 0;
    m_pokeballPos.y = 0;
    m_finalPos.x = 0;
    m_finalPos.y = 0;
    m_a = 0;
    m_b = 0;
    m_c = 0;
    m_power = -75; //"puissance" du lancer d'une pokeball par un dresseur, par defaut, -75
    m_speed = 3;
    m_isOpenning = m_isInactive = m_show = false;
}



void Pokeball::setSprites(string spriteName, string spriteOpenPokeball, string spritePokeballAura)
{
    if(spriteName.size() != 0)
        m_sprite.load(spriteName, T_RED, T_GREEN, T_BLUE);
    

    if(spriteOpenPokeball.size() != 0)
        m_openPokeball.load(spriteOpenPokeball, T_RED, T_GREEN, T_BLUE);
    

    if(spritePokeballAura.size() != 0)
        m_vortex.load(spritePokeballAura, T_RED, T_GREEN, T_BLUE, 128);
    
}


void Pokeball::launch(Character* hero, SDL_Rect destPos, PokeballLaunchReason::Enum launchReason)
{
    SDL_Rect heroPos = hero->getPos();

	m_capture = launchReason;
    hero->setDirection(GetDirectionFromPos(&heroPos, &destPos));

    if(m_show)
        return;

    int xD, yD, xF, yF;

    if(hero->getCenterPos().x < destPos.x)
    {
        xD = hero->getCenterPos().x ;
        yD = hero->getCenterPos().y ;

        xF = destPos.x;
        yF = destPos.y ;

        m_sens = 1;
    }
    else
    {
        xF = hero->getCenterPos().x;
        yF = hero->getCenterPos().y;

        xD = destPos.x;
        yD = destPos.y;

        m_sens = 0;
    }


    //pente de la droite passant par la mousePos et la pos centrale du hero
    float p;
    if(xF != xD)
         p = (float)(yF - yD) / (xF - xD);
    else
    {
        p = 100000;
        //cerr << "Erreur (classe Pokeball) : Impossible de diviser par 0"<< endl;
		return;
    }

    m_pokeballPos.x = hero->getCenterPos().x ;
    m_pokeballPos.y = hero->getCenterPos().y;
    m_finalPos.x = destPos.x;
    m_finalPos.y = destPos.y;
    m_speed = abs(m_finalPos.x - m_pokeballPos.x)/30 + 1; //on adapte la vitesse à la distance à parcourir pour éviter de privilégier les lancers de pokeball courts.


    //calcul (au préalable fait sur papier) des coefficients de la parabole en fonction de la position du heros et de l'endroit voulu du lancer.
    //théorique :
    //m_a = - (float) ((2*p*(xD - xF) + 4*m_power) + 4*sqrt(m_power*(m_power + p*(xD - xF)))) / (2*(xD - xF)*(xD - xF));


    //experimental (théorème de l'arrangement) :
    m_a = - (float) 5*((xD - xF + 2*m_power) + 2*sqrt((float)(abs(m_power*(m_power + xD - xF)))) ) / ((xD - xF)*(xD - xF));

    m_b = p - m_a*(xF + xD);
    m_c = yD - m_a*xD*xD - m_b*xD;

    m_show = true;

}

void Pokeball::capture(Character* pkmn)
{
	WGameCore& wScreen = WGameCore::getInstance();
	World& w = wScreen.getWorld();
    SDL_Rect& oRel = wScreen.getORel();



	for(unsigned int i = 0; i < TAILLEBLOC; i++)
	{
		m_pokeballPos.x++;
		m_pokeballPos.y--;
		SDL_Rect buf = m_pokeballPos, animPos = m_gestionAnim.getRectOfCurrentFrame();

		buf.x += oRel.x;
		buf.y += oRel.y;

		w.displayLayers();
		m_sprite.render(buf.x, buf.y, &animPos);

		wScreen.getParticleManager().refresh();
		wScreen.getParticleManager().display(PARTICLE_MANAGER_CRUMBLING);
		wScreen.getParticleManager().display(PARTICLE_MANAGER_RAIN);
		wScreen.getParticleManager().displayRainFog();

		//Affiche la météo
		if(w.getFog()->isActive() && *wScreen.getSettings().getFogActive())
			w.getFog()->display();
		if(w.getWeather()->isActive() && *wScreen.getSettings().getWeatherActive())
			w.getWeather()->display();


		wScreen.flip();
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
		SDL_Rect buf = m_pokeballPos, animPos = m_gestionAnim.getRectOfCurrentFrame();
		buf.x += oRel.x;
		buf.y += oRel.y;
		m_pokeballPos.y+=4;

		w.displayLayers();
		m_sprite.render(buf.x, buf.y, &animPos);
			

		buf.x -= m_vortex.getWidth()/4; //sur 4 parce que l'image est composée de 2 sous-images pour l'animation
		buf.y -= m_vortex.getHeight()/4;

		SDL_Rect animVortexPos = m_gestionAnimVortex.getRectOfCurrentFrame();
		m_vortex.render(buf.x, buf.y, &animVortexPos);
        

		wScreen.getParticleManager().refresh();
		wScreen.getParticleManager().display(PARTICLE_MANAGER_CRUMBLING);
		wScreen.getParticleManager().display(PARTICLE_MANAGER_RAIN);
		wScreen.getParticleManager().displayRainFog();

		//Affiche la météo
		if(w.getFog()->isActive() && *wScreen.getSettings().getFogActive())
			w.getFog()->display();
		if(w.getWeather()->isActive() && *wScreen.getSettings().getWeatherActive())
			w.getWeather()->display();

		wScreen.flip();
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
			SDL_Rect buf = m_pokeballPos, animPos;
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

			w.displayLayers();
			m_sprite.render(buf.x, buf.y, &animPos);


			wScreen.getParticleManager().refresh();
			wScreen.getParticleManager().display(PARTICLE_MANAGER_CRUMBLING);
			wScreen.getParticleManager().display(PARTICLE_MANAGER_RAIN);
			wScreen.getParticleManager().displayRainFog();
			//Affiche la météo
			if(w.getFog()->isActive() && *wScreen.getSettings().getFogActive())
				w.getFog()->display();
			if(w.getWeather()->isActive() && *wScreen.getSettings().getWeatherActive())
				w.getWeather()->display();



			wScreen.flip();
			SDL_Delay(30);
			delay = SDL_GetTicks() - lastTime;
		}
	}

	if(randomChanceCapture == 0)
		pkmn->setVisible(true);
	else
	{
		wScreen.getPokemonManager().add(pkmn);
		wScreen.getFight().end(EndFightReason::Capture);
	}

}

void Pokeball::setPos(int x, int y)
{
    m_pokeballPos.x = x;
    m_pokeballPos.y = y;
}

const SDL_Rect* Pokeball::getPos()
{
    return &m_pokeballPos;
}

void Pokeball::refresh()
{
	WGameCore& wScreen = WGameCore::getInstance();
	World& w = wScreen.getWorld();

    if(!m_show)
        return;

    vector<SDL_Rect> ids;
    if((m_pokeballPos.x > m_finalPos.x &&  m_sens == 0) || (m_pokeballPos.x < m_finalPos.x &&  m_sens == 1) ) //Si la Pokeball est en l'air
    {
        SDL_Rect buf, animPos = m_gestionAnim.getRectOfCurrentFrame(), oRel = wScreen.getORel();

        if(m_pokeballPos.x < m_finalPos.x)
            m_pokeballPos.x += m_speed;
        else
            m_pokeballPos.x -= m_speed;
        m_pokeballPos.y = (int) (m_a*m_pokeballPos.x*m_pokeballPos.x + m_b*m_pokeballPos.x + m_c); //calcul des coordonnées à l'aide de l'équation de parabole préalablement calculée dans Pokeball::launch

        buf = m_pokeballPos;
        //Prise en compte de l'origine relative
        buf.x += oRel.x;
        buf.y += oRel.y;

		m_sprite.render(buf.x, buf.y, &animPos);



    }
    else if(!m_isInactive) //sinon lorsqu'elle tombe par terre
        m_isOpenning = true;



    if(m_isOpenning) //Si la Pokeball est en statut d'ouverture, on l'affiche ouverte ainsi que son aura violette (statut présent pour raison de fluidité de l'animation)
    {
        SDL_Rect animVortexPos = m_gestionAnimVortex.getRectOfCurrentFrame(), buf, oRel = wScreen.getORel();
        buf = m_pokeballPos;
        buf.x += oRel.x;
        buf.y += oRel.y;

		m_openPokeball.render(buf.x, buf.y);

        buf.x -= m_vortex.getWidth()/4; //sur 4 parce que l'image est composée de 2 sous-images pour l'animation
        buf.y -= m_vortex.getHeight()/4;
		m_vortex.render(buf.x, buf.y, &animVortexPos);
        
		if(m_capture == PokeballLaunchReason::Capture)
		{
			SDL_Rect boxCapture = m_pokeballPos;
			boxCapture.x -= TAILLEBLOC;
			boxCapture.y -= TAILLEBLOC;
			boxCapture.w = 2*TAILLEBLOC;
			boxCapture.h = 2*TAILLEBLOC;

			ids = wScreen.detectEntity(boxCapture);
			const size_t idSize = ids.size();
			for(size_t i = 0; i < idSize; i++)
				if(ids[i].y == ID_CURRENT_OPPONENT)
				{
					m_capture = PokeballLaunchReason::Recall;
					this->capture(wScreen.getFight().getOpponent());
					return;
				}

		}

        m_countOpenning--;
        if(m_countOpenning <= 0)
        {
            m_countOpenning = OPEN_DELAY;
            m_isOpenning = false;
            m_isInactive = true;
        }

    }
    else if(m_isInactive) //Si la Pokeball est ouverte, inactive (statut présent pour raison de fluidité de l'animation)
    {
        SDL_Rect buf, oRel = wScreen.getORel();
        buf = m_pokeballPos;
        buf.x += oRel.x;
        buf.y += oRel.y;

		m_openPokeball.render(buf.x, buf.y);

        m_countOpenned--;
        if(m_countOpenned <= 0)
        {
            m_countOpenned = OPEN_DELAY;
            m_isInactive = false;
            m_show = false;
        }
        m_capture = PokeballLaunchReason::Recall;
    }


}

int Pokeball::getStatus()
{
    if(m_isOpenning)
        return 0;
    else
        return 1;

}

void Pokeball::hide(bool b)
{
    m_show = !b;
}

bool Pokeball::isVisible()
{
    return m_show;
}

Pokeball::~Pokeball()
{
}
