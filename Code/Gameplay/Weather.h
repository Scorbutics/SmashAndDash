#ifndef DEF_WEATHER
#define DEF_WEATHER

#include <iostream>
#include <string>
#include <SDL2/SDL.h>
#include <vector>
#include "Graphic/Texture.h"

/*Class Weather
 .refresh();                          //Gestion de déplacement du temps + affichage
 .isActive();                         //Renvoie si le temps courant est actif ou non
 .hide();                             //Permet de cacher/afficher le temps
 .modify(SDL_Surface* weatherSprite); //Modifie l'apparence du sprite

 .setFrequency(int frequency);
 .setDirection(int direction);
 .setIntensity(int intensity);

 .getFrequency();
 .getDirection();
 .getIntensity();
*/
class World;

class Weather
{

    public:
     Weather(std::string wSprite, int number, int distance, int intensityX = 1, int intensityY = -1, int alpha = 128);
     ~Weather();
	 void display();
	 bool isActive();
     void hide(bool active);
     void resetRandomPos();
     void changeSprite(std::string spriteName);

     void setDirection(int direction);
     void setIntensity(int intensityX, int intensityY);
     void setNumber(int number);
     void setMX(float x, unsigned int i);
     void setMY(float y, unsigned int i);
     void setMosaicEffect(bool x);
     void resetPos();

     int getDirection();
     int getIntensityX();
     int getIntensityY();
     int getNumber();

    protected:
     int m_intensityX, m_intensityY, m_number, m_distance;
     std::unique_ptr<Texture> m_weather;
     std::vector<float> m_x, m_y;
     bool m_active, m_mosaic;

};

#endif
