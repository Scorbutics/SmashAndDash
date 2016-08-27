#pragma once
#include "MovableWindow.h"

class WGameCore;
class WindowTeam: public MovableWindow
{
public:
	WindowTeam(const ska::InputContextManager& icm, std::string fichierMenu, ska::Rectangle posFond, int taillePolice);
    void reset(std::string action);
    void refresh();
    ~WindowTeam();

private:
    //std::vector<std::unique_ptr<SlotPokemon_Area>> m_slotPkmn;
    std::vector<int> m_boolUseObjectSelectPkmn;
    int m_indexCursor;
};



