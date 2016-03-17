#include <iostream>
#include <string>
#include <fstream>
#include <math.h>
#include "Weather.h"
#include "../Gameplay\WGameCore.h"
#include "../Utils/IDs.h"

using namespace std;

Weather::Weather(string wSprite, int number, int distance, int intensityX, int intensityY, int alpha)
{
	WGameCore& wScreen = WGameCore::getInstance();
	ska::World& w = wScreen.getWorld();

    m_intensityX = intensityX;
    m_intensityY = intensityY;
    m_number = number;
    m_active = false;
    m_mosaic = false;

	if(wSprite != "")
		m_weather = ska::TexturePtr(new ska::Texture(wSprite, DEFAULT_T_RED, DEFAULT_T_GREEN, DEFAULT_T_BLUE, alpha));


    for(int i = 0; i < m_number; i++)
    {
        m_x.push_back(0);
        m_y.push_back(0);
    }

    for(int i = 0; i < m_number; i++)
    {
        do
        {
            m_x[i] = (float) (rand()%(w.getNbrBlocX()*TAILLEBLOC));
			m_y[i] = (float) (rand() % (w.getNbrBlocY()*TAILLEBLOC));
        }while(sqrt((m_x[i])*(m_x[i]) + (m_y[i])*(m_y[i])) <= distance);

    }

}

bool Weather::isVisible() const {
	return m_active;
}

void Weather::setIntensity(int intensityX, int intensityY)
{
    m_intensityY = intensityY;
    m_intensityX = intensityX;
}

void Weather::changeSprite(string wSprite)
{
    m_weather = ska::TexturePtr(new ska::Texture(wSprite, DEFAULT_T_RED, DEFAULT_T_GREEN, DEFAULT_T_BLUE, 128));
}

void Weather::setNumber(int number)
{
    for(int i = 0; i < m_number; i++)
    {
        m_x.pop_back();
        m_y.pop_back();
    }

    m_number = number;

    for(int i = 0; i < m_number; i++)
    {
        m_x.push_back(0);
        m_y.push_back(0);
    }
}

void Weather::setMosaicEffect(bool x)
{
    m_mosaic = x;
}

void Weather::resetPos()
{
    for(int i = 0; i < m_number; i++)
    {
        m_x[i] = 0;
        m_y[i] = 0;
    }

}

void Weather::resetRandomPos()
{
	WGameCore& wScreen = WGameCore::getInstance();
	ska::World& w = wScreen.getWorld();

    for(int i = 0; i < m_number; i++)
    {
        do
        {
			m_x[i] = (float)(rand() % (w.getNbrBlocX()*TAILLEBLOC));
			m_y[i] = (float)(rand() % (w.getNbrBlocY()*TAILLEBLOC));

        }while(sqrt((float) ((m_x[i])*(m_x[i]) + (m_y[i])*(m_y[i]))) <= (TAILLEBLOC/3 * sqrt((float) (w.getNbrBlocX() * w.getNbrBlocX() + w.getNbrBlocY() * w.getNbrBlocY()))));

    }
}

void Weather::display()
{
	WGameCore& wScreen = WGameCore::getInstance();
	ska::World& w = wScreen.getWorld();
	ska::Rectangle& oRel = wScreen.getORel();

	if (!m_active) {
		return;
	}

    for(int i = 0; i < m_number; i++)
    {

		ska::Rectangle buf;
			m_x[i] += (float)(m_intensityX / 5.);
			m_y[i] += (float)(m_intensityY / 5.);

            buf.x = (int)m_x[i] + oRel.x;
            buf.y = (int)m_y[i] + oRel.y;

			

            if((m_x[i]+m_weather->getWidth()) < 0 && m_intensityX < 0)
            {
				m_x[i] = (float)w.getNbrBlocX()*TAILLEBLOC;
            }
            else if(m_x[i] > w.getNbrBlocX()*TAILLEBLOC && m_intensityX > 0)
            {
				m_x[i] = (float)-((int)m_weather->getWidth());
            }

			if ((m_y[i] + m_weather->getHeight())< 0 && m_intensityY < 0)
            {
				m_y[i] = (float)w.getNbrBlocY()*TAILLEBLOC;
            }
            else if(m_y[i] > w.getNbrBlocY()*TAILLEBLOC && m_intensityY > 0)
            {
				m_y[i] = (float)-((int)m_weather->getHeight());
            }


        if(m_mosaic)
        {
            int nbrMosaicX, nbrMosaicY;
			nbrMosaicX = w.getNbrBlocX()*TAILLEBLOC / m_weather->getWidth() + 1;
			nbrMosaicY = w.getNbrBlocY()*TAILLEBLOC / m_weather->getHeight() + 1;

            for(int i = 0; i < nbrMosaicX; i++)
            {
				buf.x = i * m_weather->getWidth() + oRel.x;
                for(int j = 0; j < nbrMosaicY; j++)
                {
					buf.y = j*m_weather->getHeight() + oRel.y;
					if (buf.x + m_weather->getWidth() >= 0 && buf.x <= w.getNbrBlocX()*TAILLEBLOC && buf.y + m_weather->getHeight() >= 0 && buf.y <= w.getNbrBlocY()*TAILLEBLOC)
						m_weather->render(buf.x, buf.y);
                }
            }

        }
		else if (buf.x + m_weather->getWidth() >= 0 && buf.x <= w.getNbrBlocX()*TAILLEBLOC && buf.y + m_weather->getHeight() >= 0 && buf.y <= w.getNbrBlocY()*TAILLEBLOC)
			m_weather->render(buf.x, buf.y);
    }
}

void Weather::setMX(float x, unsigned int i)
{
    if(i < m_x.size())
        m_x[i] = x;
    else
        cerr << "Buffer overflow au niveau de la coordonnée x de la position du i_ème sprite météo" << endl;

}

void Weather::setMY(float y, unsigned int i)
{
    if(i < m_y.size())
        m_y[i] = y;
    else
        cerr << "Buffer overflow au niveau de la coordonnée y de la position du i_ème sprite météo" << endl;

}

void Weather::hide(bool active)
{
    m_active = !active;
}

Weather::~Weather()
{
}
