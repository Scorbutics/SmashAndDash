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
#include "../ska/Graphic\SpriteAnimationManager.h"
#include "../Graphic\GUI\MouseCursor.h"
#include "../ska/Graphic\Rectangle.h"
#include "../ska/Inputs/RawInputListener.h"
#include "../Graphic\GUI\TrainerCard.h"
#include "../Graphic\ShakerManager.h"
#include "Data\SavegameManager.h"
#include "World\WorldImpl.h"
#include "../ska/Utils\Singleton_template.h"
#include "../ska/Inputs/InputContextManager.h"
#include "../ska/Scene/SceneHolder.h"
#include "../ska/Inputs/RawInputListener.h"
#include "../ska/Graphic/Rectangle.h"
#include "World/WorldScene.h"

class LayerE;
class Layer;
//class Character;

//typedef std::unique_ptr<Character> Character_ptr;

class WGameCore : public ska::Window, public ska::Singleton<WGameCore>, public ska::SceneHolder
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
	
	void initNewWorld();
	void transition(int type);
	void waitQuit(DialogMenu* window);

	float getSpeedInertie();
	ska::Animation& getChipsetAnimation();

	GUI& getGUI();
	Fight& getFight();
	Pokeball& getPokeball();
	AI& getAI();
	Inventory& getInventory();
	Settings& getSettings();
	PokemonManager& getPokemonManager();
	ska::SpriteAnimationManager& getSpriteAnimationManager();
    MouseCursor& getMouseCursor();
	TrainerCard& getTrainerCard();
	ShakerManager& getShakerManager();
	ska::World& getWorld();
	WorldScene& getWorldScene();
	
	const ska::InputActionContainer& getActions() const;
	const ska::InputRangeContainer& getRanges() const;
	const ska::InputToggleContainer& getToggles() const;

	ska::ParticleManager& getParticleManager();
	RainParticleManager& getRainParticleManager();

    void setOffsetChipset(int x, int y, int w, int h);
    void setSpeedInertie(float x);
    void setChipset(SDL_Surface* chipset);
	
	virtual void nextScene(std::unique_ptr<ska::Scene>& scene) override;
	virtual ska::ScenePtr& getScene() override;

    protected:
		

		ska::RawInputListener m_rawInputListener;
		ska::InputContextManager m_inputCManager;
		


		ska::Rectangle m_OfChip;
		bool m_ecritureLog;

		float m_speedInertie;
		ska::Animation m_chipsetAni;
		AI m_ai;
		vector<DialogMenu*> m_guiList;
		Pokeball m_pokeball;
		PokemonManager m_pkmnManager; //Une instance de gestion des pok�mon de la team
		TrainerCard m_trainerCard;
		ShakerManager m_shaker;
		ska::SpriteAnimationManager m_spriteAnimManager;
		bool m_scrolling;		
		Fight m_fight;
		MouseCursor m_mouseCursor;
		GUI m_gui;
		
		Settings m_settings;
		Inventory m_inv;
		WorldScene m_worldScene;

		ska::ParticleManager m_particleManager;
		RainParticleManager m_rainParticleManager;
		


};

#endif
