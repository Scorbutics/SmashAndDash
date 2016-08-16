#pragma once

#include "MovableWindow.h"


class WindowBag: public MovableWindow
{
public:
	WindowBag(const ska::InputContextManager& icm, std::string fichierMenu, ska::Rectangle posFond, int taillePolice);

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

