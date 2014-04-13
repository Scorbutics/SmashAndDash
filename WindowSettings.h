#ifndef DEF_WINDOW_SETTINGS
#define DEF_WINDOW_SETTINGS

#include "MovableWindow.h"

class WindowSettings: public MovableWindow
{
    public:
    WindowSettings(std::string fichierMenu, SDL_Rect posFond, int taillePolice);
    void reset();
    void refresh();
    ~WindowSettings();

    private:
    int m_saveSettings, m_saveGame;
};

#endif
