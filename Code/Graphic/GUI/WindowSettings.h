#pragma once
#include "../../ska/Graphic/GUI/MoveableWindow.h"

class WindowSettings : public ska::MoveableWindow {
public:
	WindowSettings(ska::MouseObservable& guiObservable, ska::KeyboardObservable& keyboardObs, const ska::Point<int>& boxAbsolutePos);
    ~WindowSettings() = default;
};

