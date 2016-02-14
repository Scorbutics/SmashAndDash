#ifndef DEF_WINDOW_BAG
#define DEF_WINDOW_BAG

#include "MovableWindow.h"

class WGameCore;
class WindowBag: public MovableWindow
{
    public:
    WindowBag(std::string fichierMenu, SDL_Rect posFond, int taillePolice);

    int* getBoolUseObject();
    int* getBoolTrashObject();
    int* getBoolGiveObject();
    SDL_Rect getCurObjectPos();

    void reset();
    void refresh();
    ~WindowBag();

    private:
    int m_boolUseObject, m_boolGiveObject, m_boolTrashObject;
    SDL_Rect m_curObjectPos;
};

#endif
