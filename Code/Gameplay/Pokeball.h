#pragma once
#include "../ska/Graphic/Point.h"
#include "PokeballLaunchReasonEnum.h"
#include "../ska/Graphic/Animation.h"
#include "../ska/Graphic/Texture.h"
#include "../ska/Graphic/Draw/DrawableFixedPriority.h"

class Pokeball : public ska::DrawableFixedPriority {

public:
    Pokeball();

    void setPos(int x, int y);

	const ska::Point<int>& getPos();
    int getStatus() const;

    void capture(/*Character* pkmn*/);
	void launch(const ska::Point<int>& src, const ska::Point<int>& destPos, PokeballLaunchReason::Enum launchReason);
	void update();
	void display() const override;
	bool isVisible() const override;
    void hide(bool b);
    ~Pokeball();

private:
	void setSprites(const std::string& spriteName, const std::string& spriteOpenPokeball, const std::string& spritePokeballAura);

	ska::Point<int> m_pokeballPos, m_finalPos;
	ska::Texture m_sprite, m_openPokeball, m_vortex;							//m_vortex : sprite de l'aura violette lors de l'ouverture d'une Pokeball
    int m_power, m_speed, m_sens, m_countOpenning, m_countOpenned;		//m_countOpenning est le delai pendant lequel la pokeball s'ouvre, m_countOpen est le delai pendant lequel la pokeball reste au sol apres ouverture (en frames)
    float m_a, m_b, m_c;												//coefficients de la parabole
    bool m_show, m_isOpenning, m_isInactive;							//les différents états de la Pokeball
	ska::Animation m_gestionAnim, m_gestionAnimVortex;						//animation du lancer de pokeball, animation de l'aura de la pokeball
	PokeballLaunchReason::Enum m_capture;

};


