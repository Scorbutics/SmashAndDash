#ifndef DEF_PERSONNAGE
#define DEF_PERSONNAGE

#include <string>
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <memory>

#include "Animation.h"
#include "Bar.h"
#include "Path.h"
#include "PhysicSprite.h"
#include "SpriteAnimation.h"
#include "PokemonDescriptor.h"
#include "IniReader.h"

class Statistics;
class Path;
class Skill;
class Missile;
class Projectile;
class Melee;
class WGameCore;
class Layer;
class SpriteBank;
class World;

typedef std::unique_ptr<Skill> Skill_ptr;

class Character : public PhysicSprite
{

    public:
    Character(int id);

    bool isAlive();
    void findPath(SDL_Rect dest);
    //void addSkill(int stype, int id, std::string skillSprite, int nombre, std::string icone, SDL_Rect *posIcone, int degats, int knockback, std::string description, std::string nom, std::string type, int deviation, int noise, int cooldown, int vitesse);
	void addSkill(const string& pathAttackName);
	bool autoattack();
	bool damage(Character* src, unsigned int damages);
    bool launchSkill(unsigned int skillNumber, SDL_Rect dest);
    void displaySkills();
	void followEntity(Character* entity);
    virtual void refresh();
    virtual vector<SDL_Rect> move();
	void playAnimation(unsigned int characterAnim, bool waitFinish);

    std::vector<Skill_ptr> *getSkills();
    Bar* getHPBar();
    Texture* getFaceset();
    float getCountD();
    Path* getPath();
    unsigned int getHp();
    int getContext();
	PokemonDescriptor* getDescriptor();
	Statistics* getStatistics();

    void setHP(int hp);
    void setFaceset(SDL_Surface* faceset);
    void setID(int id);
    bool setSkill(int i, std::string genre, std::string skill, int nombre, std::string icone, SDL_Rect *posIcone, int degats, int knockback, std::string description, std::string nom, std::string type, int deviation = 0);
    void setVisible(bool v);
    void setCountD(float x);
	void setAutoattack(bool b);

	void nextLevel();
	void decrease(int stat, int value);
	void refreshStats(bool refreshHp = false);
	void refreshSkills();
	void display(int part, bool shadow = true);
	void run(bool b);
	bool addExperience(int exp);
    ~Character();


    private:
    void setHPBar(std::string styleName, std::string contentName, SDL_Rect pos);
    void displaySpeed();
    void displayGrassWalk();
	void loadData();
    //PAS DE COPIE
    Character(const Character &p);
	

	protected:
		unique_ptr<Statistics> m_stats;
		PokemonDescriptor m_descriptor;

		Texture m_faceset;
		unsigned int m_hp, m_attack, m_defense, m_spe_attack, m_spe_defense;
		int m_debuffSpeed, m_followCount;
		float m_compteurDeplacement;
		Bar m_hpbar;
		Path m_path;
		bool m_alive, m_autoAttack;
		SpriteAnimation m_grassSprite;
		std::vector<Skill_ptr> m_skill;
		Skill_ptr m_skillAutoAttack;
		Character* m_entityFollow;

};

typedef std::unique_ptr<Character> Character_ptr;
#endif
