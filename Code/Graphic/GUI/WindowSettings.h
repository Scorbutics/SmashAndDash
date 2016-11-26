#pragma once
#include "../../ska/Graphic/GUI/MoveableWindow.h"

namespace ska {
	class Widget;
}

class WindowSettings : public ska::MoveableWindow<> {
public:
	WindowSettings(ska::Widget& parent, const ska::Point<int>& boxAbsolutePos);
    ~WindowSettings() = default;
};

