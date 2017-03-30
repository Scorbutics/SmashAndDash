#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "Graphic/Texture.h"
#include "Point.h"
#include "Draw/DrawableFixedPriority.h"

/*Class Weather
 .refresh();                          //Gestion de déplacement du temps + affichage
 .hide();                             //Permet de cacher/afficher le temps
 .modify(SDL_Surface* weatherSprite); //Modifie l'apparence du sprite

 .setFrequency(int frequency);
 .setDirection(int direction);
 .setIntensity(int intensity);

 .getFrequency();
 .getDirection();
 .getIntensity();
*/
namespace ska {
	class World;
}

class Weather : public ska::DrawableFixedPriority
{

public:
	Weather(ska::World& w, const std::string& wSprite, int number, int distance, int intensityX = 1, int intensityY = -1, int alpha = 128);
	explicit Weather(ska::World& w);
	Weather(const Weather&) = delete;
	Weather& operator=(const Weather&) = delete;
	 ~Weather();

	 void load(const std::string& wSprite, int number, int distance, int intensityX = 1, int intensityY = -1, int alpha = 128);

	 void display() const override;
	 bool isVisible() const override;
	 void hide(bool active);
     void resetRandomPos();

	 void update();

     void setDirection(int direction);
     void setNumber(int number);
     void setMosaicEffect(bool x);
     void resetPos();

     int getDirection();
     int getIntensityX();
     int getIntensityY();
     int getNumber();

protected:
     int m_intensityX, m_intensityY, m_number, m_distance;
     std::unique_ptr<ska::Texture> m_weather;
     std::vector<ska::Point<float>> m_pos;
     bool m_active, m_mosaic;
	 ska::World& m_world;

};

