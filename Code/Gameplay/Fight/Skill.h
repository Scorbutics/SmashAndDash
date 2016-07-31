#ifndef DEF_AMISSILE
#define DEF_AMISSILE

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include "../../ska/Inputs\Readers\IniReader.h"
#include "../../ska/Graphic\Texture.h"
#include "../../ska/Physic\Particle.h"


class Skill
{
    public:
	Skill(ska::IniReader* data);
	
	unsigned int getRange();
	int getKnockback();
	void setKnockback(int knockback);
    ska::Texture* getIcon();
    void setIcon(ska::Texture* icon);
    string getName();
	void setPosIcon(ska::Rectangle posIcone);
	ska::Rectangle getPosIcon();
    void setName(string name);
    string getDescription();
    void setDescription(string description);
	virtual void launch(ska::Point<int> dest) = 0;
    bool cooldownOK();
    ska::Texture* getSpriteRemainingCD();
    string getType();
    ska::Particle* getParticle(unsigned int number);
    void setType(string type);
    virtual void refresh() = 0;
	virtual void display() = 0;
    bool isActive(int number);
    int getNumber();
    virtual ~Skill();

    int getContext();

    protected:
    std::vector<ska::ParticlePtr> m_particles;
    Character* m_parent;
    SDL_Color m_color;
	ska::Rectangle m_src, m_dst, m_anim;
    vector<float> m_x, m_y; //m_x m_y seront des coordonnées précises (sans se limiter aux entiers pour les pixels)
    vector<bool> m_actif;
    ska::Texture m_cooldownText;
    int m_comptAnim, m_direction, m_context, m_id;
    int m_cooldown, m_lastTimeCD, m_timeCD, m_degats, m_knockback;
	unsigned int m_range;
    ska::Texture m_icone;
    string m_description, m_name, m_type, m_style1, m_style2;
	ska::Rectangle m_posIcone;
	unique_ptr<Statistics> m_statsBuffAlly, m_statsBuffEnemy;
	int m_statusAlterAlly, m_statusAlterEnemy;
	static const unsigned int m_fontSize;

    private:
    Skill(Skill const& skillACopier);// Empêche la copie
};
typedef std::unique_ptr<Skill> Skill_ptr;


class Projectile : public Skill
{
    public:
    Projectile(ska::IniReader* data, Character* parent);
    virtual void refresh();
    virtual ~Projectile();
	void setSpeed(float vitesse);
	float getSpeed();
	virtual void display();
	void collision();
	void launch(ska::Point<int> dest);
private:
    unsigned int m_t;
	int m_slopeNoise, m_noise, m_nombre;
	float m_vitesse;
};


class AOE : public Skill
{
public:
	AOE(ska::IniReader* data, Character* parent);
	virtual void refresh();
	virtual void launch(ska::Point<int> dest);
	virtual ~AOE();
	virtual void display();
private:
	bool m_active;

};


/*
class Melee : public Skill
{
    public:
    Melee(int id, string skill, string icone, Rectangle *posIcone, int degats, int knockback, string description, string nom, string type, int cooldown, int context, Character* parent);
    virtual void refresh();
    virtual ~Melee();

};*/

#endif
