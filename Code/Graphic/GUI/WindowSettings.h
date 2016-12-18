#pragma once
#include "../../ska/Graphic/GUI/Components/UiBinder.h"
#include "../../ska/Graphic/GUI/MoveableWindow.h"
#include "../../ska/Graphic/GUI/Components/KeyEventListener.h"

namespace ska {
	class Widget;
	class GUI;
	class LabeledHorizontalSlider;
}

class Settings;

class WindowSettings : public ska::MoveableWindow<ska::KeyEventListener> {
public:
	WindowSettings(ska::Widget& parent, const ska::Point<int>& boxAbsolutePos);
	void bind(Settings& sets);
    ~WindowSettings() = default;

private:
	ska::LabeledHorizontalSlider* m_volController;
	ska::UiBinder<float> m_volumeBinder;
};

