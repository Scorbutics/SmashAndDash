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

WindowSettings::WindowSettings(ska::Widget& parent, const ska::Point<int>& absolutePos) :
ska::MoveableWindow<ska::KeyEventListener>(parent, ska::Rectangle{ absolutePos.x, absolutePos.y, 8 * TAILLEBLOCFENETRE, 7 * TAILLEBLOCFENETRE }, ska::Button::MENU_DEFAULT_THEME_PATH + "menu") {	

	auto input = std::unique_ptr<ska::Input>(new ska::Input(*this, " ", 12, ska::Point<int>(16, 32)));
	input->addHandler<ska::ValueChangedEventListener<std::wstring>>([](ska::Widget* tthis, ska::ValueChangedEvent<std::wstring>& e) {
		std::wclog << "Typed :" << e.getValue() << std::endl;
	});
	addWidget(input);

	auto checkBox = std::unique_ptr<ska::CheckBox>(new ska::CheckBox(*this, ska::Point<int>(16, 64), ska::Button::MENU_DEFAULT_THEME_PATH + "checkbox", nullptr));
	addWidget(checkBox);

	auto question = std::unique_ptr<ska::Label>(new ska::Label(*this, "Comment aimes-tu le chocolat ?", 14, ska::Point<int>(16, 96)));
	addWidget(question);

	auto radioList = std::unique_ptr<ska::RadioButtonList<std::string>>(new ska::RadioButtonList<std::string>(*this, ska::Point<int>(176, 96), ska::Button::MENU_DEFAULT_THEME_PATH + "radiobutton"));
	std::vector<std::string> radioVals;
	radioVals.push_back("Blanc");
	radioVals.push_back("Lait");
	radioVals.push_back("Noir");
	radioList->load(std::move(radioVals));
	radioList->addHandler<ska::ValueChangedEventListener<std::string*>>([](ska::Widget* tthis, ska::ValueChangedEvent<std::string*>& e) {
		std::clog << "Radio value : " << *e.getValue() << std::endl;
	});
	addWidget(radioList);

	auto listbox = std::unique_ptr<ska::ListBox<int>>(new ska::ListBox<int>(*this, ska::Point<int>(16, 160), ska::Button::MENU_DEFAULT_THEME_PATH + "listbox", ska::Button::MENU_DEFAULT_THEME_PATH + "listbox-clean", nullptr));
	std::vector<int> vals;
	vals.push_back(0);
	vals.push_back(7);
	vals.push_back(25);
	vals.push_back(82);
	listbox->load(std::move(vals));
	listbox->addHandler<ska::ValueChangedEventListener<int*>>([](ska::Widget* tthis, ska::ValueChangedEvent<int*>& e) {
		int a;
		std::clog << "Listbox choice" << std::endl;
	});
	addWidget(listbox);

	auto checkBox2 = std::unique_ptr<ska::CheckBox>(new ska::CheckBox(*this, ska::Point<int>(16, 192), ska::Button::MENU_DEFAULT_THEME_PATH + "checkbox", nullptr));
	checkBox2->addHandler<ska::ValueChangedEventListener<bool>>([](ska::Widget* tthis, ska::ValueChangedEvent<bool>& e) {
		int a;
		std::clog << "Checkbox" << std::endl;
	});
	addWidget(checkBox2);

	auto hSlider = std::unique_ptr<ska::LabeledHorizontalSlider>(new ska::LabeledHorizontalSlider(*this, ska::Button::MENU_DEFAULT_THEME_PATH + "slider", ska::Point<int>(48, 192), 160));
	m_volController = addWidget(hSlider);

	resort();
}

void WindowSettings::bind(Settings& sets) {
	m_volumeBinder.bind(*m_volController, std::bind(&Settings::setSoundVolume, &sets, std::placeholders::_1));
}

// #include "WindowSettings.h"
// #include "ToolBar.h"
// #include "WindowShop.h"
// #include "WindowTeam.h"
// #include "WindowBag.h"
// #include "../../Utils/IDs.h"
// #include "../../ska/Graphic/GUI/Components/ButtonQuit.h"
//
// WindowSettings::WindowSettings(const ska::InputContextManager& icm, std::string fichierMenu, ska::Rectangle posFond, int taillePolice) :
// MovableWindow(icm, fichierMenu, posFond, taillePolice)
// {
//     m_saveSettings = 0;
// 	m_saveGame = 0;
// }
//
// void WindowSettings::reset()
// {
//     this->clear();
//
// 	ska::Rectangle buf;
//     buf.x = 4*TAILLEBLOCFENETRE;
//     buf.y = TAILLEBLOCFENETRE*3/4;
// 	const auto& box = getBox();
// 	this->resize(box.w, 10 * TAILLEBLOCFENETRE);
//     buf.x = 9*TAILLEBLOCFENETRE;
// 	//addWidget(std::unique_ptr<ska::ButtonQuit>(new ska::ButtonQuit(*this, buf, "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"close_button.png")));
// 	//this->resize(box.w, 10 * TAILLEBLOCFENETRE);
// 	std::vector<int> vbool, vSndVol;
//     vbool.push_back(0);
//     vbool.push_back(1);
//     vSndVol.push_back(0);
//     vSndVol.push_back(25);
//     vSndVol.push_back(50);
//     vSndVol.push_back(75);
//     vSndVol.push_back(100);
// 	std::vector<std::string> vOnOff, vSndVolS, vSave, vSaveGame;
//     vOnOff.push_back("Off");
//     vOnOff.push_back("On");
//     vSndVolS.push_back("0%");
//     vSndVolS.push_back("25%");
//     vSndVolS.push_back("50%");
//     vSndVolS.push_back("75%");
//     vSndVolS.push_back("100%");
// 	vSaveGame.push_back("Save game");
// 	vSaveGame.push_back("Game saved");
//     vSave.push_back("Save settings");
//     vSave.push_back("Saved !");
//
//     buf.x = 4*TAILLEBLOCFENETRE;
//     buf.y = TAILLEBLOCFENETRE*3/4;
//     //this->addTextArea("Options", 20, buf);
//     buf.x = TAILLEBLOCFENETRE;
//     buf.y = 2*TAILLEBLOCFENETRE;
//     //this->addTextArea("Brouillard :", 20, buf);
//     buf.x += (int)(6.5*TAILLEBLOCFENETRE);
// 	/*
//     this->addButton(buf, "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"button.png", "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"buttonpressed.png", settings.getFogActive(), vbool, vOnOff, 20, "fog_settings");
//     buf.y += TAILLEBLOCFENETRE;
// 	buf.x -= (int)(6.5*TAILLEBLOCFENETRE);
//     this->addTextArea("M�t�o :", 20, buf);
// 	buf.x += (int)(6.5*TAILLEBLOCFENETRE);
//     this->addButton(buf, "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"button.png", "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"buttonpressed.png", settings.getWeatherActive(), vbool, vOnOff, 20, "weather_settings");
//     buf.y += TAILLEBLOCFENETRE;
// 	buf.x -= (int)(6.5*TAILLEBLOCFENETRE);
//     this->addTextArea("Particules:", 20, buf);
// 	buf.x += (int)(6.5*TAILLEBLOCFENETRE);
//     this->addButton(buf, "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"button.png", "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"buttonpressed.png", settings.getParticles(), vbool, vOnOff, 20, "particles_settings");
//     buf.y += TAILLEBLOCFENETRE;
// 	buf.x -= (int)(6.5*TAILLEBLOCFENETRE);
//     this->addTextArea("Transparence de l'interface:", 20, buf);
// 	buf.x += (int)(6.5*TAILLEBLOCFENETRE);
//     this->addButton(buf, "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"button.png", "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"buttonpressed.png", settings.getGuiTransparency(), vbool, vOnOff, 20, "gui_settings");
//     buf.y += TAILLEBLOCFENETRE;
// 	buf.x -= (int)(6.5*TAILLEBLOCFENETRE);
//     this->addTextArea("Volume musique :", 20, buf);
// 	buf.x += (int)(6.5*TAILLEBLOCFENETRE);
//     buf.w = 3*TAILLEBLOCFENETRE;
//     this->addButtonBar(buf, "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"scroll_scrollbar_horizontal.png", settings.getSoundVolume(), vSndVol, vSndVolS, 20, "sound_vol_settings");
//     buf.y = 8*TAILLEBLOCFENETRE;
// 	buf.x -= (int)(6.5*TAILLEBLOCFENETRE);
// 	this->addButton(buf, "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"button.png", "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"buttonpressed.png", &m_saveGame, vbool, vSaveGame, 20, "save_game");
// 	buf.x += (int)(2.5*TAILLEBLOCFENETRE);
//     this->addTextArea("Save settings :", 20, buf);
//     buf.x += 4*TAILLEBLOCFENETRE;
//     this->addButton(buf, "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"button.png", "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"buttonpressed.png", &m_saveSettings, vbool, vSave, 20, "save_settings");
// 	*/
// }
//
// void WindowSettings::refresh()
// {
//     MovableWindow::refresh();
//
//
// 	//Settings& settings = wScreen.getSettings();
//
//     if(m_saveSettings != 0)
//     {
//         /*settings.save();
//         for(unsigned int i = 0; i < gui.getButtonListSize(); i++)
//         {
//             //gui.getButton(i)->setAlpha((*settings.getGuiTransparency() != 0));
//             //gui.getButton(i)->setAlphaImg((*settings.getGuiTransparency() != 0));
//         }
//         this->setAlpha((*settings.getGuiTransparency() != 0));*/
//         /*gui.getAttackPokemonWindow()->setAlpha((*settings.getGuiTransparency() != 0));
//         gui.getAttackOpponentWindow()->setAlpha((*settings.getGuiTransparency() != 0));
//         gui.getInfoPokemonWindow()->setAlpha((*settings.getGuiTransparency() != 0));
//         gui.getInfoPNJWindow()->setAlpha((*settings.getGuiTransparency() != 0));
//         //gui.getDialog()->setAlpha((*settings.getGuiTransparency() != 0));
//         gui.getToolbar()->setAlpha((*settings.getGuiTransparency() != 0));
//         gui.getWindowTeam()->setAlpha((*settings.getGuiTransparency() != 0));
//         gui.getWindowShop()->getShopBar()->setAlpha((*settings.getGuiTransparency() != 0));
//         gui.getWindowBag()->setAlpha((*settings.getGuiTransparency() != 0));
//         //gui.getImgDialog()->setAlpha((*settings.getGuiTransparency() != 0));
//         gui.getFacesetPkmn()->setAlpha((*settings.getGuiTransparency() != 0));
//         gui.getFacesetOpponent()->setAlpha((*settings.getGuiTransparency() != 0));*/
//         //getButton("save_settings")->forceValue(0);
//     }
//
// 	if (m_saveGame != 0)
// 	{
// 		//wScreen.getSavegameManager().saveGame("save1");
// 		//getButton("save_game")->forceValue(0);
// 	}
// }
//
// WindowSettings::~WindowSettings()
// {
// }
//
