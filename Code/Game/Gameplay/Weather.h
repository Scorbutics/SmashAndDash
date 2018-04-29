#pragma once
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
	class TileWorld;
}

class Weather : 
	public ska::DrawableFixedPriority {

public:
	Weather(ska::TileWorld& w, const std::string& wSprite, int number, int distance, int intensityX = 1, int intensityY = -1, int alpha = 128);
	explicit Weather(ska::TileWorld& w);
	Weather(const Weather&) = delete;
	Weather& operator=(const Weather&) = delete;
	 ~Weather() = default;

	 void load(const std::string& wSprite, int number, int distance, int intensityX = 1, int intensityY = -1, int alpha = 128);

	 void render(const ska::Renderer& render) const override;
	 bool isVisible() const override;
	 void hide(bool active);
     void resetRandomPos();

	 void update();

     void setNumber(int number);
     void setMosaicEffect(bool x);
     void resetPos();

protected:
     int m_intensityX, m_intensityY, m_number, m_distance;
     std::unique_ptr<ska::Texture> m_weather;
     std::vector<ska::Point<float>> m_pos;
     bool m_active, m_mosaic;
	 ska::TileWorld& m_world;

};

