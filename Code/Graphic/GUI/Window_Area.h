#ifndef DEF_WINDOW_AREA
#define DEF_WINDOW_AREA

#include <iostream>
#include <string>
#include <SDL2/SDL.h>

class DialogMenu;
class WGameCore;

class Window_Area
{
    public:
    Window_Area(DialogMenu* parent);
    bool canBeClicked();
    bool isVisible();
    void hide(bool x);
    SDL_Rect* getRelativePos();
    void setRelativePos(SDL_Rect pos);
    DialogMenu* getParent();
	virtual void display() = 0;
    virtual std::string getKey() = 0;
    virtual void forceValue(unsigned int index) = 0;
    virtual ~Window_Area();
    SDL_Rect getAbsolutePos();
    bool isA(int type);

    protected:
    DialogMenu* m_parent;
    SDL_Rect m_relativePos;
    bool m_canBeClicked, m_active;
    int m_type;

};

#endif
