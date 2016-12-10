#pragma once
#include "../../ska/Graphic/GUI/MoveableWindow.h"
#include "../../ska/Graphic/GUI/Components/KeyEventListener.h"

namespace ska {
	class Widget;
	class GUI;
}

class WindowSettings : public ska::MoveableWindow<ska::KeyEventListener> {
public:
	WindowSettings(ska::GUI & gui, ska::Widget& parent, const ska::Point<int>& boxAbsolutePos);
    ~WindowSettings() = default;
};

