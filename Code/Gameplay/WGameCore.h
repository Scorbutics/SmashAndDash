#ifndef DEF_WGAMECORE
#define DEF_WGAMECORE

#include <windows.h>

#include <iostream>
#include <string>
#include <memory>
#include "Graphic\GUI\Window.h"
#include "Player.h"
#include "Graphic\Animation.h"
#include "Fight\Fight.h"
#include "Pokeball.h"
#include "AI\AI.h"
#include "Physic\ParticleManager.h"
#include "Data\Settings.h"
#include "PokemonManager.h"
#include "EntityFactory.h"
#include "Graphic\SpriteAnimationManager.h"
#include "Graphic\GUI\MouseCursor.h"
#include "Inputs\InputListener.h"
#include "Graphic\GUI\TrainerCard.h"
#include "Graphic\ShakerManager.h"
#include "Mobs\MobSpawningManager.h"
#include "Data\SavegameManager.h"
#include "World\World.h"
#include "Utils\Singleton_template.h"


class LayerE;
class Layer;
class Character;

typedef std::unique_ptr<Character> Character_ptr;

class WGameCore : public Window, public Singleton<WGameCore>
{
	friend class Singleton<WGameCore>;

    private:
    WGameCore();
	~WGameCore();

	public:
	void resize(unsigned int w, unsigned int h);
	bool refresh();
    void activeEcritureLog();
	bool isScrollingActive();
	void activeScrolling(bool b);
	void quitFlip();
	void quitter(bool transition);
	void* graphicUpdate(void);
    std::vector<SDL_Rect> detectEntity(SDL_Rect box);
    std::vector<SDL_Rect> detectEntity(SDL_Rect box, int direction);
	void eventUpdate(bool movingDisallowed);
	void initNewWorld();
	void transition(int type);
	void waitQuit(DialogMenu* window);

	SDL_Rect& getOffsetHero();
	SDL_Rect& getOffsetChipset();
	SDL_Rect& getOffsetActualHero();
	SDL_Rect& getORel();
	Player* getHero();
	float getSpeedInertie();
	Animation& getChipsetAnimation();
	GUI& getGUI();
	Fight& getFight();
	Pokeball& getPokeball();
	AI& getAI();
	ParticleManager& getParticleManager();
	Inventory& getInventory();
	Settings& getSettings();
	PokemonManager& getPokemonManager();
	EntityFactory& getEntityFactory();
	SpriteAnimationManager& getSpriteAnimationManager();
    MouseCursor& getMouseCursor();
	bool getContinue();
	InputListener& getInputListener();
	TrainerCard& getTrainerCard();
	ShakerManager& getShakerManager();
	World& getWorld();
	MobSpawningManager& getMobSpawningManager();
	SavegameManager& getSavegameManager();

    void setEntite(unsigned int i, int value);
    void setOffsetChipset(int x, int y, int w, int h);
    void setORel(int x, int y);
    void setORel(const SDL_Rect &oRel);
    void setSpeedInertie(float x);
    void setHero(Player* hero);
    void setChipset(SDL_Surface* chipset);
	void setContinue(bool b);
	
   

    protected:
		Settings m_settings;
		Inventory m_inv;
		SDL_Rect m_OfChip, m_origineRelative;
		bool m_quitFlip, m_ecritureLog, m_continue;
		Player *m_phero; //Character héro courant
		InputListener m_kdListener;
		float m_speedInertie;
		Animation m_chipsetAni;
		AI m_ai;
		vector<DialogMenu*> m_guiList;
		Pokeball m_pokeball;
		PokemonManager m_pkmnManager; //Une instance de gestion des pokémon de la team
		ParticleManager m_particleManager;
		TrainerCard m_trainerCard;
		ShakerManager m_shaker;
		SpriteAnimationManager m_spriteAnimManager;
		bool m_scrolling;
		EntityFactory m_EntityFactory; //Une instance de gestion des personnages à l'écran (hors combat)
		Fight m_fight;
		MouseCursor m_mouseCursor;
		GUI m_gui;
		MobSpawningManager m_mobSpawner;
		SavegameManager m_saveManager;
		World m_world;
};

#endif
