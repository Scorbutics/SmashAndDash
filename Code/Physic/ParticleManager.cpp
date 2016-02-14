#include "ParticleManager.h"

#include <math.h>
#include "GravityParticle.h"
#include "Gameplay\WGameCore.h"
#include "Gameplay\World\Layer.h"

using namespace std;

ParticleManager::ParticleManager()
{
    m_wind = 0;
    m_countWind = 1;
    m_Twind = m_countWind * M_PI/96; //période du vent : simule une perturbation dans la pluie
    m_sens = 1;

    //3 types d'animations pour l'instant : pluie, éboulement et animation linéaire à une position donnée
    m_particles.resize(3);
    m_duration.resize(3);
    m_t0.resize(3);
    m_active.resize(3);
    m_origin.resize(3);

    for(unsigned int i = 0; i < m_active.size(); i++)
        m_active[i] = false;


}


void ParticleManager::addGravityParticle(unsigned int particleType, int idSprite, double lifetime, double splashtime, float acceleration, bool loop, bool relative)
{
    if(particleType >= m_particles.size())
    {
        cerr << "Erreur (class ParticleManager) : Le type de particle numéro " << particleType << " n'existe pas." << endl;
        return;
    }

    const size_t size = m_particles[particleType].size();
    for(size_t i = 0; i < size; i++)
    {
        if(m_particles[particleType][i] == NULL)
        {
            m_particles[particleType][i] = unique_ptr<GravityParticle>(new GravityParticle(idSprite, m_origin[particleType], lifetime, splashtime, acceleration, loop, relative));
            return;
        }
    }
	
    m_particles[particleType].push_back(unique_ptr<GravityParticle>(new GravityParticle(idSprite, m_origin[particleType], lifetime, splashtime, acceleration, loop, relative)));

}

void ParticleManager::addGravityParticle(unsigned int particleType, int idSprite, SDL_Rect pos, double lifetime, double splashtime, float acceleration, bool loop, bool relative)
{
    m_origin[particleType] = pos;
    this->addGravityParticle(particleType, idSprite, lifetime, splashtime, acceleration, loop, relative);
}

//void ParticleManager::addCircleParticle(int idSprite, unsigned int weight){}
//void ParticleManager::addLinearParticle(int idSprite, unsigned int weight){}

void ParticleManager::removeAll()
{
    const size_t size = m_particles.size();
    for(size_t i = 0; i < size; i++)
    {
        const size_t size2 = m_particles[i].size();
        for(size_t j = 0; j < size2; j++)
            this->remove((unsigned int)i, (unsigned int)j);

		m_active[i] = false;
		m_duration[i] = 0;
    }


}

void ParticleManager::removeAll(unsigned int particleType)
{
    if(particleType >= m_particles.size())
    {
        cerr << "Erreur (class ParticleManager) : Le type de particle numéro " << particleType << " n'existe pas." << endl;
        return;
    }

    const size_t size = m_particles[particleType].size();
    for(unsigned int i = 0; i < size; i++)
    {
		if(m_particles[particleType][i] != NULL && !m_particles[particleType][i]->isActive())
            this->remove(particleType, i);
    }


}

void ParticleManager::stop(unsigned int particleType)
{
	for (unique_ptr<Particle>& gp : m_particles[particleType])
	{
		if (gp != NULL)
			gp->setLoop(false);
	}
    if(particleType == PARTICLE_MANAGER_RAIN)
        m_rainWeather.free();

}

void ParticleManager::stop()
{
	for (unsigned int i = 0; i < 3; i++)
		stop(i);
}

void ParticleManager::remove(unsigned int particleType, unsigned int index)
{
    if(particleType >= m_particles.size())
    {
        cerr << "Erreur (class ParticleManager) : Le type de particle numéro " << particleType << " n'existe pas." << endl;
        return;
    }


    if(index < m_particles[particleType].size())
	{
		m_particles[particleType][index].reset();
        m_particles[particleType][index] = NULL;
    }
    else
        cerr << "Erreur (classe ParticleManager) : impossible d'accéder à l'élément numéro " << index << " des sprites de particules de type " << particleType << endl;

	if (m_particles[particleType].size() == 0)
	{
		m_active[particleType] = false;
		m_duration[particleType] = 0;
	}
}

void ParticleManager::display(unsigned int particleType)
{
	WGameCore& wScreen = WGameCore::getInstance();
	World& w = wScreen.getWorld();

	if(!wScreen.getSettings().getParticles())
		return;

	if(particleType >= m_particles.size())
	{
		cerr << "Erreur (class ParticleManager) : Le type de particle numéro " << particleType << " n'existe pas." << endl;
		return;
	}

	if(m_active[particleType])
	{
		const size_t size2 = m_particles[particleType].size();
		for(size_t j = 0; j < size2; j++)
		{
			if(m_particles[particleType][j] != NULL)
			{

				if(m_particles[particleType][j]->isActive())
					m_particles[particleType][j]->display();
			}
		}
	}
}

void ParticleManager::refresh(unsigned int particleType)
{
	WGameCore& wScreen = WGameCore::getInstance();
	World& w = wScreen.getWorld();

    if(!wScreen.getSettings().getParticles())
        return;

    if(particleType >= m_particles.size())
    {
        cerr << "Erreur (class ParticleManager) : Le type de particle numéro " << particleType << " n'existe pas." << endl;
        return;
    }

    if(m_active[particleType])
    {
        const size_t size2 = m_particles[particleType].size();
        for(size_t j = 0; j < size2; j++)
        {
            if(m_particles[particleType][j] != NULL)
            {
				
                if(m_particles[particleType][j]->isActive())
                {

                    m_particles[particleType][j]->refresh((int)((cos(m_Twind) > 0 && particleType == PARTICLE_MANAGER_RAIN ? 8*TAILLEBLOC*cos(m_Twind):0)));
                    if(cos(m_Twind) > 0  && particleType == PARTICLE_MANAGER_RAIN)
                    {
                        //ajoute un effet de vent en fonction de la pluie
                        w.getLayerMid()->setWind((int)(3*cos(m_Twind)+0.5));
                        w.getLayerBot()->setWind((int)(3*cos(m_Twind)+0.5));
                        w.getLayerTop()->setWind((int)(3*cos(m_Twind)+0.5));
                    }

                }
                else
                {
                    //Si la particule n'est pas prévue pour réitérer
                    if(!m_particles[particleType][j]->loop())
                        this->remove(particleType, (unsigned int)j); //On la supprime
                    else//Sinon, on la replace à sa position d'origine
                    {
                        SDL_Rect posBuf;
                        if(particleType == PARTICLE_MANAGER_RAIN)
                        {
                            posBuf.y = -3*TAILLEBLOC;
                            posBuf.x = rand()%( (j%(wScreen.getWidth()/TAILLEBLOC + 10))*TAILLEBLOC - ((j-1)%(wScreen.getWidth()/TAILLEBLOC + 10))*TAILLEBLOC+ 1) + ((j-1)%(wScreen.getWidth()/TAILLEBLOC + 10))*TAILLEBLOC - 5*TAILLEBLOC;
                        }
                        else
                        {
							posBuf = m_particles[particleType][j]->getOrigin();
                        }

                        if(m_particles[particleType][j]->isRelative())
                        {
                            posBuf.y -= wScreen.getORel().y;
                            posBuf.x -= wScreen.getORel().x;
                        }

                        m_particles[particleType][j]->launch(posBuf, m_particles[particleType][j]->getAngle(), m_particles[particleType][j]->getPower());


                    }

                }
            }
        }




        //Si le phénomène doit s'arrêter
        if(m_duration[particleType] != 0 && SDL_GetTicks() - m_t0[particleType] > m_duration[particleType])
            this->stop(particleType);


    }


}

void ParticleManager::displayRainFog()
{
	WGameCore& wScreen = WGameCore::getInstance();

    //Si la pluie est jouée, on met un effet "temps couvert"
    if(m_active[PARTICLE_MANAGER_RAIN])
    {
        unsigned int mosaicNumberX, mosaicNumberY;
        mosaicNumberX = wScreen.getWidth()/m_rainWeather.getWidth() + 1;
        mosaicNumberY = wScreen.getHeight()/m_rainWeather.getHeight() + 1;

        SDL_Rect buf;
        buf.x = buf.y = 0;
        for(unsigned int j = 0; j < mosaicNumberX; j++)
        {
            for(unsigned int k = 0; k < mosaicNumberY; k++)
            {
				m_rainWeather.render(buf.x, buf.y);
                buf.y += m_rainWeather.getHeight();
            }
            buf.x += m_rainWeather.getWidth();
            buf.y = 0;
        }

        if(m_countWind < 2000 && m_countWind != 0)
        {
            m_countWind += m_sens;
            m_Twind = m_countWind * M_PI/140;
        }
        else
        {
            m_sens *= -1;
            m_countWind += m_sens;
            m_Twind = m_countWind * M_PI/140;
        }

    }
}

void ParticleManager::refresh()
{
	WGameCore& wScreen = WGameCore::getInstance();

    if(!wScreen.getSettings().getParticles())
        return;

    const size_t size = m_particles.size();
    for(size_t i = 0; i < size; i++)
        this->refresh((unsigned int) i);

}

//Effondrement
void ParticleManager::playCrumbling(int idSprite, float acceleration, float density, unsigned int duration)
{
	WGameCore& wScreen = WGameCore::getInstance();

    m_duration[PARTICLE_MANAGER_CRUMBLING] = duration;
    m_active[PARTICLE_MANAGER_CRUMBLING] = true;
    this->removeAll(PARTICLE_MANAGER_CRUMBLING);

    SDL_Rect posBuf;
    for(int i = 0; i < wScreen.getWidth()* wScreen.getHeight()*density/(TAILLEBLOC*TAILLEBLOC); i++)
    {
		posBuf.y = -TAILLEBLOC;
		posBuf.x = rand() % wScreen.getWidth();
        this->addGravityParticle(PARTICLE_MANAGER_CRUMBLING, idSprite, posBuf, rand()%((int)(90 - acceleration + 1)) + acceleration, acceleration, acceleration, true, false);
        //ajoute une particule gravitationnelle d'id 0
    }

    m_t0[PARTICLE_MANAGER_CRUMBLING] = SDL_GetTicks();
}

void ParticleManager::playEffect(int idSprite, float acceleration, float density, unsigned int duration, SDL_Rect rect)
{
    m_duration[PARTICLE_MANAGER_EFFECT] = duration;
    m_active[PARTICLE_MANAGER_EFFECT] = true;
	removeAll(PARTICLE_MANAGER_EFFECT);

    SDL_Rect posBuf;
    for(int i = 0; i < density; i++)
    {
        posBuf.x = rect.x + rand()%(rect.w + 1);
        posBuf.y = rect.y + rand()%(rect.h + 1);
        this->addGravityParticle(PARTICLE_MANAGER_EFFECT, idSprite, posBuf, rand()%((int)(20 + 1)), 2, acceleration, false, false);
    }

    m_t0[PARTICLE_MANAGER_EFFECT] = SDL_GetTicks();
}

void ParticleManager::playRain(int idSprite, float acceleration, float density, unsigned int duration)
{
	WGameCore& wScreen = WGameCore::getInstance();

    m_duration[PARTICLE_MANAGER_RAIN] = duration;
    m_active[PARTICLE_MANAGER_RAIN] = true;
    //this->removeAll(PARTICLE_MANAGER_RAIN);

    SDL_Rect posBuf;
    for(int i = 0; i < wScreen.getWidth()* wScreen.getHeight()*density/(TAILLEBLOC*TAILLEBLOC); i++)
    {
        posBuf.y = wScreen.getHeight()/2;
        posBuf.x = wScreen.getWidth()/2;
        this->addGravityParticle(PARTICLE_MANAGER_RAIN, idSprite, posBuf, rand()%((int)(90 - acceleration + 1)) + acceleration, acceleration, acceleration, true, true);
        //ajoute une particule gravitationnelle d'id 0
    }

    m_rainWeather.load("."FILE_SEPARATOR"Sprites"FILE_SEPARATOR"Meteo"FILE_SEPARATOR"gloomy.png", -1, -1, -1, 128);
    m_t0[PARTICLE_MANAGER_RAIN] = SDL_GetTicks();

}



void ParticleManager::launch(unsigned int particleType, float angle, unsigned int power)
{
    if(particleType >= m_particles.size())
    {
        cerr << "Erreur (class ParticleManager) : Le type de particle numéro " << particleType << " n'existe pas." << endl;
        return;
    }

    size_t size = m_particles[particleType].size();
    for(size_t i = 0; i < size; i++)
    {
        if(m_particles[particleType][i] != NULL)
        {
            int state = m_particles[particleType][i]->getState();
            if(state == PARTICLE_STATE_END)
                m_particles[particleType][i]->launch(m_particles[particleType][i]->getOrigin(), angle, power);
        }
    }
}

int ParticleManager::collisionNPC(unsigned int particleType)
{
	WGameCore& wScreen = WGameCore::getInstance();

    if(particleType >= m_particles.size())
    {
        cerr << "Erreur (class ParticleManager) : Le type de particle numéro " << particleType << " n'existe pas." << endl;
        return -1;
    }

    const size_t size = m_particles[particleType].size();
    vector<SDL_Rect> ids, buf;
    size_t i;

    for(i = 0; i < size; i++)
    {
        buf = wScreen.detectEntity(m_particles[particleType][i]->getPos());
        const size_t numberEntities = buf.size();
        for(unsigned int j = 0; j < numberEntities; j++)
            ids.push_back(buf[j]);
    }

    return (i == size ? -1 : (int) i);
}


ParticleManager::~ParticleManager(){this->removeAll();}
