#include "ParticleManager.h"

#include "GravityParticle.h"
//#include "../Gameplay\WGameCore.h"
#include "../World/Layer.h"
#include "../Utils/NumberUtils.h"

using namespace std;

ska::ParticleManager::ParticleManager()
{
    m_sens = 1;
	m_active = false;
}

void ska::ParticleManager::addParticle(ParticlePtr& particle) {
	/* Mon dieu quelle horreur en terme de performances...
	   TODO un Set */
	const size_t size = m_particles.size();
	for (size_t i = 0; i < size; i++) {
		if (m_particles[i] == NULL)
		{
			m_particles[i] = move(particle);
			return;
		}
	}

	m_particles.push_back(move(particle));
}

//void ParticleManager::addCircleParticle(int idSprite, unsigned int weight){}
//void ParticleManager::addLinearParticle(int idSprite, unsigned int weight){}

void ska::ParticleManager::removeAll()
{
    const size_t size = m_particles.size();
    for(size_t i = 0; i < size; i++)
    {
		if (m_particles[i] != NULL && !m_particles[i]->isActive()) {
			this->remove((unsigned int)i);
		}
    }


}


void ska::ParticleManager::stop()
{
	for (unique_ptr<Particle>& gp : m_particles)
	{
		if (gp != NULL)
			gp->setLoop(false);
	}
	m_active = false;
	m_duration = 0;
   
}

void ska::ParticleManager::hide(bool h) {
	m_active = !h;
}

bool ska::ParticleManager::isVisible() const {
	return m_active;
}

void ska::ParticleManager::remove(unsigned int index)
{

    if(index < m_particles.size())
	{
		m_particles[index].reset();
        m_particles[index] = NULL;
    }
    else
        cerr << "Erreur (classe ParticleManager) : impossible d'accéder à l'élément numéro " << index << " des sprites de particules" << endl;

	if (m_particles.size() == 0)
	{
		m_active = false;
		m_duration = 0;
	}
}

void ska::ParticleManager::display()
{
	//WGameCore& wScreen = WGameCore::getInstance();
	//World& w = wScreen.getWorld();
		
	if(m_active)
	{
		const size_t size2 = m_particles.size();
		for(size_t j = 0; j < size2; j++)
		{
			if(m_particles[j] != NULL)
			{

				if(m_particles[j]->isActive())
					m_particles[j]->display();
			}
		}
	}
}

void ska::ParticleManager::refresh()
{
	//WGameCore& wScreen = WGameCore::getInstance();
	//World& w = wScreen.getWorld();

	if (!m_active) {
		return;
	}

    if(m_active)
    {
        const size_t size2 = m_particles.size();
        for(size_t j = 0; j < size2; j++)
        {
            if(m_particles[j] != NULL)
            {
				
                if(m_particles[j]->isActive())
                {
                    m_particles[j]->refresh();
                }
                else
                {
                    //Si la particule n'est pas prévue pour réitérer
                    if(!m_particles[j]->loop())
                        this->remove((unsigned int)j); //On la supprime
                    else//Sinon, on la replace à sa position d'origine
                    {
						ska::Point<int> posBuf;
						posBuf = m_particles[j]->getOrigin();

                        if(m_particles[j]->isRelative())
                        {
                            //posBuf.y -= wScreen.getORel().y;
                            //posBuf.x -= wScreen.getORel().x;
                        }

                        m_particles[j]->launch(posBuf, m_particles[j]->getAngle(), m_particles[j]->getPower());


                    }

                }
            }
        }




        //Si le phénomène doit s'arrêter
        if(m_duration != 0 && SDL_GetTicks() - m_t0 > m_duration)
            this->stop();


    }


}

//Effondrement
void ska::ParticleManager::playCrumbling(int idSprite, float acceleration, float density, unsigned int duration)
{
	//WGameCore& wScreen = WGameCore::getInstance();

	/*m_duration = duration;
    m_active = true;
    //this->removeAll();

	ska::Rectangle posBuf;
    for(int i = 0; i < wScreen.getWidth()* wScreen.getHeight()*density/(TAILLEBLOC*TAILLEBLOC); i++)
    {
		posBuf.y = -TAILLEBLOC;
		posBuf.x = rand() % wScreen.getWidth();

		this->addParticle(ParticlePtr(new GravityParticle(idSprite, posBuf, rand() % ((int)(90 - acceleration + 1)) + acceleration, acceleration, acceleration, true, false)));
        //ajoute une particule gravitationnelle d'id 0
    }*/

    m_t0 = SDL_GetTicks();
}

void ska::ParticleManager::playEffect(int idSprite, float acceleration, float density, unsigned int duration, ska::Rectangle rect)
{
    m_duration = duration;
    m_active = true;
	//removeAll();

	ska::Rectangle posBuf;
	if (density < 1 && density > 0) {
		/* Aucun affichage si on a une densité trop faible et pas de chance ;) */
		if (density < NumberUtils::random()) {
			return;
		}
	}

    for(int i = 0; i < density; i++)
    {
        posBuf.x = rect.x + rand()%(rect.w + 1);
        posBuf.y = rect.y + rand()%(rect.h + 1);
		this->addParticle(ParticlePtr(new GravityParticle(idSprite, posBuf, rand() % ((int)(20 + 1)), 2, acceleration, false, false)));
    }

    m_t0 = SDL_GetTicks();
}





void ska::ParticleManager::launch(float angle, unsigned int power)
{
    const size_t size = m_particles.size();
    for(size_t i = 0; i < size; i++)
    {
        if(m_particles[i] != NULL)
        {
            int state = m_particles[i]->getState();
			if (state == PARTICLE_STATE_END) {
				m_particles[i]->launch(m_particles[i]->getOrigin(), angle, power);
			}
        }
    }
}

int ska::ParticleManager::collisionNPC()
{
	//WGameCore& wScreen = WGameCore::getInstance();

    const size_t size = m_particles.size();
	vector<ska::Rectangle> ids, buf;
    size_t i;

    for(i = 0; i < size; i++)
    {
        //buf = wScreen.detectEntity(m_particles[i]->getPos());
        const size_t numberEntities = buf.size();
        for(unsigned int j = 0; j < numberEntities; j++)
            ids.push_back(buf[j]);
    }

    return (i == size ? -1 : (int) i);
}


ska::ParticleManager::~ParticleManager(){ this->removeAll(); }
