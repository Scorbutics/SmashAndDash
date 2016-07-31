#ifndef DEF_MOVABLE_WINDOW
#define DEF_MOVABLE_WINDOW


#include "DialogMenu.h"
#include "../../ska/Graphic/Point.h"

class WGameCore;

class MovableWindow : public DialogMenu
{
public:
	MovableWindow(const std::string& fichierMenu, ska::Rectangle posFond, int taillePolice);
    virtual void refresh();
    virtual std::string getKey(){return "";};
    virtual void forceValue(unsigned int){};
    virtual ~MovableWindow();

private:
	ska::Point<float> m_mouseLastPos;
};

#endif
