#ifndef DEF_POKEBALL
#define DEF_POKEBALL

#include <iostream>
#include <string>
#include <memory>
#include <SDL2/SDL.h>
#include "PokeballLaunchReasonEnum.h"
#include "../Graphic/Animation.h"
#include "../Graphic/Texture.h"
#include "../Graphic/Draw/DrawableFixedPriority.h"

class Character;
class WGameCore;
typedef std::unique_ptr<Character> Character_ptr;

class Pokeball : public DrawableFixedPriority
{

    public:
    Pokeball(std::string spriteName, Character* hero, SDL_Rect destPos, SDL_Rect oRel);
    Pokeball();

    void setSprites(std::string spriteName, std::string spriteOpenPokeball, std::string spritePokeballAura);
    void setPos(int x, int y);

    const SDL_Rect* getPos();
    int getStatus();

    void capture(Character* pkmn);
    void launch(Character* hero, SDL_Rect mousePos, PokeballLaunchReason::Enum launchReason);
	void display() override;
	bool isVisible() const override;
    void hide(bool b);
    ~Pokeball();

    private:
    SDL_Rect m_pokeballPos, m_finalPos;
    Texture m_sprite, m_openPokeball, m_vortex;							//m_vortex : sprite de l'aura violette lors de l'ouverture d'une Pokeball
    int m_power, m_speed, m_sens, m_countOpenning, m_countOpenned;		//m_countOpenning est le delai pendant lequel la pokeball s'ouvre, m_countOpen est le delai pendant lequel la pokeball reste au sol apres ouverture (en frames)
    float m_a, m_b, m_c;												//coefficients de la parabole
    bool m_show, m_isOpenning, m_isInactive;							//les différents états de la Pokeball
    Animation m_gestionAnim, m_gestionAnimVortex;						//animation du lancer de pokeball, animation de l'aura de la pokeball
	PokeballLaunchReason::Enum m_capture;

};



#endif
