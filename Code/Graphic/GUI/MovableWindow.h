#ifndef DEF_MOVABLE_WINDOW
#define DEF_MOVABLE_WINDOW


#include "DialogMenu.h"
class WGameCore;

class MovableWindow : public DialogMenu
{
    public:
    MovableWindow(std::string fichierMenu, SDL_Rect posFond, int taillePolice);
    virtual void refresh();
    virtual std::string getKey(){return "";};
    virtual void forceValue(unsigned int){};
    virtual ~MovableWindow();

    private:
    SDL_Rect m_mouseLastPos;
};

#endif
