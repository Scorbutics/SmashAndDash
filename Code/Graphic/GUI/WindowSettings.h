#ifndef DEF_WINDOW_SETTINGS
#define DEF_WINDOW_SETTINGS

#include "MovableWindow.h"

class WindowSettings: public MovableWindow
{
    public:
		WindowSettings(std::string fichierMenu, ska::Rectangle posFond, int taillePolice);
    void reset();
    void refresh();
    ~WindowSettings();

    private:
    int m_saveSettings, m_saveGame;
};

//typedef std::unique_ptr<WindowSettings> WindowSettingsPtr;

#endif
