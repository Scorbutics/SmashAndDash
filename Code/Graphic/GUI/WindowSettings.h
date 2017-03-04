#pragma once
#include "../../ska/Graphic/GUI/Components/UiBinder.h"
#include "../../ska/Graphic/GUI/MoveableWindow.h"
#include "../../ska/Graphic/GUI/Components/KeyEventListener.h"

namespace ska {
	class Widget;
	class GUI;
	class LabeledHorizontalSlider;
	class CheckBox;
}

class Settings;

class WindowSettings : public ska::MoveableWindow<ska::KeyEventListener> {
public:
	WindowSettings(Widget& parent, const ska::Point<int>& boxAbsolutePos);
	void bind(Settings& sets);
	void setMusicVolume(float volPcts);
	void setWeather(bool b);
	void setFog(bool b);
    ~WindowSettings() = default;

private:
	ska::LabeledHorizontalSlider* m_volController;
	ska::CheckBox* m_fogTController;
	ska::CheckBox* m_weatherController;
	ska::UiBinder<float> m_volumeBinder;
	ska::UiBinder<bool> m_weatherBinder;
	ska::UiBinder<bool> m_fogBinder;
};

