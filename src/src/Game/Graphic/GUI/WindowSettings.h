#pragma once
#include "GUI/Components/UiBinder.h"
#include "GUI/Windows/MoveableWindow.h"


namespace ska {
	class Widget;
	class GUI;
	class LabeledHorizontalSlider;
	class CheckBox;
}

class Settings;

class WindowSettings : public ska::MoveableWindow<ska::KeyEventListener> {
public:
	WindowSettings(ska::WidgetPanel& parent, const ska::Point<int>& boxAbsolutePos);
	void bind(Settings& sets);
	// From 0 to 1 : of everything, of the music, of the sounds
	void setVolumes(float master, float music, float sound);
	void setWeather(bool b);
	void setFog(bool b);
    ~WindowSettings() = default;

private:
	ska::LabeledHorizontalSlider* m_masterController;
	ska::LabeledHorizontalSlider* m_musicController;
	ska::LabeledHorizontalSlider* m_soundController;
	ska::CheckBox* m_fogTController;
	ska::CheckBox* m_weatherController;
	ska::UiBinder<float> m_masterBinder;
	ska::UiBinder<float> m_musicBinder;
	ska::UiBinder<float> m_soundBinder;
	ska::UiBinder<bool> m_weatherBinder;
	ska::UiBinder<bool> m_fogBinder;
};

