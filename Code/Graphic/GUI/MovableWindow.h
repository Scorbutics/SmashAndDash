#pragma once

#include "DialogMenu.h"
#include "../../ska/Graphic/Point.h"

namespace ska {
	class InputContextManager;
}

class MovableWindow : public DialogMenu
{
public:
	MovableWindow(const ska::InputContextManager& icm, const std::string& fichierMenu, ska::Rectangle posFond, int taillePolice);
    virtual void refresh();
    virtual ~MovableWindow();

private:
	ska::Point<float> m_mouseLastPos;
protected:
	const ska::InputContextManager& m_playerICM;
};

