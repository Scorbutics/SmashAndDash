#pragma once
#include "../../ska/Graphic/GUI/MoveableWindow.h"

class WindowSettings : public ska::MoveableWindow<> {
public:
	WindowSettings(ska::MouseObservable& guiObservable, ska::KeyObservable& keyboardObs, const ska::Point<int>& boxAbsolutePos);
    ~WindowSettings() = default;
};

