#ifndef DEF_WINDOW_BAG
#define DEF_WINDOW_BAG

#include "MovableWindow.h"

class WGameCore;
class WindowBag: public MovableWindow
{
    public:
		WindowBag(std::string fichierMenu, ska::Rectangle posFond, int taillePolice);

    int* getBoolUseObject();
    int* getBoolTrashObject();
    int* getBoolGiveObject();
	ska::Point<int> getCurObjectPos();

    void reset();
    void refresh();
    ~WindowBag();

    private:
    int m_boolUseObject, m_boolGiveObject, m_boolTrashObject;
	ska::Point<int> m_curObjectPos;
};

//typedef std::unique_ptr<WindowBag> WindowBagPtr;

#endif
