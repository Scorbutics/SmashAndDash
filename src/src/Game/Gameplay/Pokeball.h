#pragma once
#include "Core/Point.h"
#include "PokeballLaunchReasonEnum.h"
#include "Graphic/Animation.h"
#include "Graphic/Texture.h"
#include "Core/Draw/DrawableFixedPriority.h"

class Pokeball : 
	public ska::DrawableFixedPriority {

public:
    Pokeball();

    void setPos(int x, int y);

	const ska::Point<int>& getPos();
    int getStatus() const;

    void capture(/*Character* pkmn*/);
	void launch(const ska::Point<int>& src, const ska::Point<int>& destPos, PokeballLaunchReason::Enum launchReason);
	void update();
	void render(ska::Renderer& renderer) const override;
	bool isVisible() const;
    void hide(bool b);
    ~Pokeball();

private:
	void setSprites(const std::string& spriteName, const std::string& spriteOpenPokeball, const std::string& spritePokeballAura);

	ska::Point<int> m_pokeballPos, m_finalPos;
	ska::Texture m_sprite, m_openPokeball, m_vortex;					
    int m_power, m_speed, m_sens, m_countOpenning, m_countOpenned;		
    float m_a, m_b, m_c;												
    bool m_show, m_isOpenning, m_isInactive;							
	ska::Animation m_gestionAnim, m_gestionAnimVortex;					
	PokeballLaunchReason::Enum m_capture;

};


