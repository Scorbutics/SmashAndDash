#ifndef DEF_WGAMECORE
#define DEF_WGAMECORE

#include <windows.h>

#include <iostream>
#include <string>
#include <memory>
#include "../ska/Graphic\GUI\Window.h"
#include "Player.h"
#include "../ska/Graphic\Animation.h"
#include "Fight\Fight.h"
#include "Pokeball.h"
#include "AI\AI.h"
#include "../ska/Physic\ParticleManager.h"
#include "../Physic/RainParticleManager.h"
#include "Data\Settings.h"
#include "PokemonManager.h"
#include "EntityFactory.h"
#include "../ska/Graphic\SpriteAnimationManager.h"
#include "../Graphic\GUI\MouseCursor.h"
#include "../ska/Graphic\Rectangle.h"
#include "../ska/Inputs/RawInputListener.h"
#include "../Graphic\GUI\TrainerCard.h"
#include "../Graphic\ShakerManager.h"
#include "Mobs\MobSpawningManager.h"
#include "Data\SavegameManager.h"
#include "World\WorldImpl.h"
#include "../ska/Utils\Singleton_template.h"
#include "./Scene/SceneMap.h"
#include "./Scene/SceneFight.h"
#include "EnumScene.h"

#include "../ska/Inputs/RawInputListener.h"

class LayerE;
class Layer;
//class Character;

//typedef std::unique_ptr<Character> Character_ptr;

class WGameCore : public ska::Window, public ska::Singleton<WGameCore>
{
	friend class ska::Singleton<WGameCore>;

private:
    WGameCore();
	~WGameCore();

public:
	void graphicUpdate(void);
	void eventUpdate(bool movingDisallowed);

	void resize(unsigned int w, unsigned int h);
	bool refresh();
    void activeEcritureLog();
	bool isScrollingActive();
	void activeScrolling(bool b);
	void quitFlip();
	void quitter(bool transition);
	
	std::vector<ska::Rectangle> detectEntity(ska::Rectangle box);
	std::vector<ska::Rectangle> detectEntity(ska::Rectangle box, int direction);
	
	void switchScene(EnumScene::Enum scene);
	void initNewWorld();
	void transition(int type);
	void waitQuit(DialogMenu* window);

	ska::Rectangle& getOffsetHero();
	ska::Rectangle& getOffsetChipset();
	ska::Rectangle& getOffsetActualHero();
	ska::Rectangle& getORel();
	//Player* getHero();
	float getSpeedInertie();
	ska::Animation& getChipsetAnimation();
	GUI& getGUI();
	Fight& getFight();
	Pokeball& getPokeball();
	AI& getAI();
	Inventory& getInventory();
	Settings& getSettings();
	PokemonManager& getPokemonManager();
	//EntityFactory& getEntityFactory();
	ska::SpriteAnimationManager& getSpriteAnimationManager();
    MouseCursor& getMouseCursor();
	bool getContinue();
	TrainerCard& getTrainerCard();
	ShakerManager& getShakerManager();
	ska::World& getWorld();
	MobSpawningManager& getMobSpawningManager();
	SavegameManager& getSavegameManager();

	const ska::InputActionContainer& getActions() const;
	const ska::InputRangeContainer& getRanges() const;

	ska::ParticleManager& getParticleManager();
	RainParticleManager& getRainParticleManager();

    void setEntite(unsigned int i, int value);
    void setOffsetChipset(int x, int y, int w, int h);
    void setORel(int x, int y);
	void setORel(const ska::Rectangle &oRel);
    void setSpeedInertie(float x);
    //void setHero(Player* hero);
    void setChipset(SDL_Surface* chipset);
	void setContinue(bool b);
	
   

    protected:
		ska::Scene* m_sceneCursor;
		SceneMap m_sceneMap;
		SceneFight m_sceneFight;

		Settings m_settings;
		Inventory m_inv;
		ska::Rectangle m_OfChip, m_origineRelative;
		bool m_quitFlip, m_ecritureLog, m_continue;
		//Player *m_phero; //Character héro courant
		ska::RawInputListener m_rawInputListener;

		float m_speedInertie;
		ska::Animation m_chipsetAni;
		AI m_ai;
		vector<DialogMenu*> m_guiList;
		Pokeball m_pokeball;
		PokemonManager m_pkmnManager; //Une instance de gestion des pokémon de la team
		TrainerCard m_trainerCard;
		ShakerManager m_shaker;
		ska::SpriteAnimationManager m_spriteAnimManager;
		bool m_scrolling;
		//EntityFactory m_EntityFactory; //Une instance de gestion des personnages à l'écran (hors combat)
		Fight m_fight;
		MouseCursor m_mouseCursor;
		GUI m_gui;
		MobSpawningManager m_mobSpawner;
		SavegameManager m_saveManager;
		WorldImpl m_world;

		ska::ParticleManager m_particleManager;
		RainParticleManager m_rainParticleManager;

};

#endif
