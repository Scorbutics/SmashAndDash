#ifndef DEF_COMBAT
#define DEF_COMBAT

#include <string>
#include <memory>
#include <SDL2/SDL.h>
#include "Player.h"
#include "LayerE.h"
#include "Texture.h"

class World;
class DialogMenu;

class Fight
{
    public:
    Fight();
	void updateAreaStates();
	bool isInFightArea(Player* hero);
    bool isFighting();
	void start(Character* opponent);
	void showDialog(unsigned int duration);
    
    void refresh();
    void end(bool win);
    ~Fight();
    void setAreasFromLayerEvent();
    void setFightCount(int x);
    void setFight(bool x);

    int getFightCount();
    Character* getOpponent();
    Player* getPokemon();
    Player* getTrainer();
    DialogMenu* getDialog();
    const SDL_Rect* getArea(int i);

    void addArea(int x, int y, int w, int h);
    void addArea(SDL_Rect area);
    void deleteAllAreas();
	void display();
	
    private:
    void displayDialog();
	bool m_isFighting, m_dialogActive;
    unsigned int m_t0, m_duration, m_mobAggroRange;
    int m_fightCount;
    Character *m_opponent; 
	Player* m_pkmn, *m_trainer;
    std::vector<SDL_Rect> m_areaList;
	SDL_Rect m_opponentID;
    Animation m_animGrass;
    Texture m_grassSprite;
    std::unique_ptr<DialogMenu> m_dial;

};

#endif
