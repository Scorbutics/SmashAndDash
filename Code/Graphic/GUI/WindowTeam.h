#ifndef DEF_WINDOW_TEAM
#define DEF_WINDOW_TEAM

#include "MovableWindow.h"
#include "SlotPokemonArea.h"

class WGameCore;
class WindowTeam: public MovableWindow
{
    public:
    WindowTeam(string fichierMenu, SDL_Rect posFond, int taillePolice);
    void reset(std::string action);
    void refresh();
    ~WindowTeam();

    private:
    std::vector<std::unique_ptr<SlotPokemon_Area>> m_slotPkmn;
    std::vector<int> m_boolUseObjectSelectPkmn;
    int m_indexCursor;
};

#endif
