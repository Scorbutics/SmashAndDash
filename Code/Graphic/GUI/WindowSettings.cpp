#include <iostream>

#include "WindowSettings.h"
#include "../../Gameplay/Data/Settings.h"

#include "../../ska/Graphic/GUI/GUI.h"
#include "../../ska/Graphic/GUI/Components/Input.h"
#include "../../ska/Graphic/GUI/Components/CheckBox.h"
#include "../../ska/Graphic/GUI/Components/RadioButtonGroup.h"
#include "../../ska/Graphic/GUI/Components/RadioButton.h"
#include "../../ska/Graphic/GUI/Components/RadioButtonList.h"
#include "../../ska/Graphic/GUI/Components/ListBox.h"
#include "../../ska/Graphic/GUI/Components/LabeledHorizontalSlider.h"

WindowSettings::WindowSettings(Widget& parent, const ska::Point<int>& absolutePos) :
MoveableWindow<ska::KeyEventListener>(parent, ska::Rectangle{ absolutePos.x, absolutePos.y, 8 * TAILLEBLOCFENETRE, 10 * TAILLEBLOCFENETRE }, ska::Button::MENU_DEFAULT_THEME_PATH + "menu") {	

// 	auto input = std::unique_ptr<ska::Input>(new ska::Input(*this, " ", 12, ska::Point<int>(16, 32)));
// 	input->addHandler<ska::ValueChangedEventListener<std::wstring>>([](ska::Widget* tthis, ska::ValueChangedEvent<std::wstring>& e) {
// 		std::wclog << "Typed :" << e.getValue() << std::endl;
// 	});
// 	addWidget(input);
	
	auto title = std::unique_ptr<ska::Label>(new ska::Label(*this, "Paramètres", 11, ska::Point<int>(95, 0)));
	title->setPriority(std::numeric_limits<int>::max());
	title->setFontColor(255, 255, 255, 255);
	addWidget(title);

	auto weatherText = std::unique_ptr<ska::Label>(new ska::Label(*this, "Météo", 11, ska::Point<int>(40, 32)));
	addWidget(weatherText);

	auto weather = std::unique_ptr<ska::CheckBox>(new ska::CheckBox(*this, ska::Point<int>(16, 32), ska::Button::MENU_DEFAULT_THEME_PATH + "checkbox", nullptr));
	m_weatherController = addWidget(weather);

	auto fogTransparencyText = std::unique_ptr<ska::Label>(new ska::Label(*this, "Brouillard transparent", 11, ska::Point<int>(40, 48)));
	addWidget(fogTransparencyText);

	auto fogTransparency = std::unique_ptr<ska::CheckBox>(new ska::CheckBox(*this, ska::Point<int>(16, 48), ska::Button::MENU_DEFAULT_THEME_PATH + "checkbox", nullptr));
	m_fogTController = addWidget(fogTransparency);

// 	auto radioList = std::unique_ptr<ska::RadioButtonList<std::string>>(new ska::RadioButtonList<std::string>(*this, ska::Point<int>(176, 96), ska::Button::MENU_DEFAULT_THEME_PATH + "radiobutton"));
// 	std::vector<std::string> radioVals;
// 	radioVals.push_back("Blanc");
// 	radioVals.push_back("Lait");
// 	radioVals.push_back("Noir");
// 	radioList->load(std::move(radioVals));
// 	radioList->addHandler<ska::ValueChangedEventListener<std::string*>>([](ska::Widget* tthis, ska::ValueChangedEvent<std::string*>& e) {
// 		std::clog << "Radio value : " << *e.getValue() << std::endl;
// 	});
// 	addWidget(radioList);

// 	auto listbox = std::unique_ptr<ska::ListBox<int>>(new ska::ListBox<int>(*this, ska::Point<int>(16, 160), ska::Button::MENU_DEFAULT_THEME_PATH + "listbox", ska::Button::MENU_DEFAULT_THEME_PATH + "listbox-clean", nullptr));
// 	std::vector<int> vals;
// 	vals.push_back(0);
// 	vals.push_back(7);
// 	vals.push_back(25);
// 	vals.push_back(82);
// 	listbox->load(std::move(vals));
// 	listbox->addHandler<ska::ValueChangedEventListener<int*>>([](ska::Widget* tthis, ska::ValueChangedEvent<int*>& e) {
// 		int a;
// 		std::clog << "Listbox choice" << std::endl;
// 	});
// 	addWidget(listbox);



	auto volLabel = std::unique_ptr<ska::Label>(new ska::Label(*this, "Volume", 11, ska::Point<int>(16, 80)));
	addWidget(volLabel);
	auto hSlider = std::unique_ptr<ska::LabeledHorizontalSlider>(new ska::LabeledHorizontalSlider(*this, ska::Button::MENU_DEFAULT_THEME_PATH + "slider", ska::Point<int>(16, 96), 160));
	m_volController = addWidget(hSlider);

	resort();
}

void WindowSettings::bind(Settings& sets) {
	m_volumeBinder.bind(*m_volController, std::bind(&Settings::setSoundVolume, &sets, std::placeholders::_1));
	m_fogBinder.bind(*m_fogTController, std::bind(&Settings::setFog, &sets, std::placeholders::_1));
	m_weatherBinder.bind(*m_weatherController, std::bind(&Settings::setWeather, &sets, std::placeholders::_1));
}

void WindowSettings::setMusicVolume(float volPcts) {
	m_volController->forceValue(volPcts);
}

void WindowSettings::setWeather(bool b) {
	m_weatherController->forceValue(b);
}

void WindowSettings::setFog(bool b) {
	m_fogTController->forceValue(b);
}
