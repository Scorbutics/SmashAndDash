#include "WindowSettings.h"
#include "../../ska/Graphic/GUI/GUI.h"
#include "../../ska/Graphic/GUI/Components/Input.h"
#include "../../ska/Graphic/GUI/Components/CheckBox.h"
#include "../../ska/Graphic/GUI/Components/RadioButtonGroup.h"
#include "../../ska/Graphic/GUI/Components/RadioButton.h"
#include "../../ska/Graphic/GUI/Components/ListBox.h"
#include "../../ska/Graphic/GUI/Components/HorizontalSlider.h"

WindowSettings::WindowSettings(ska::GUI& gui, ska::Widget& parent, const ska::Point<int>& absolutePos) :
ska::MoveableWindow<ska::KeyEventListener>(parent, ska::Rectangle{ absolutePos.x, absolutePos.y, 7 * TAILLEBLOCFENETRE, 7 * TAILLEBLOCFENETRE }, ska::Button::MENU_DEFAULT_THEME_PATH + "menu") {


	auto input = std::unique_ptr<ska::Input>(new ska::Input(*this, " ", 12, ska::Point<int>(16, 32)));
	addWidget(input);

	auto checkBox = std::unique_ptr<ska::CheckBox>(new ska::CheckBox(*this, ska::Point<int>(16, 64), ska::Button::MENU_DEFAULT_THEME_PATH + "checkbox", nullptr));
	addWidget(checkBox);

	std::shared_ptr<ska::RadioButtonGroup> group = std::make_shared<ska::RadioButtonGroup>();
	auto radioButton1 = std::unique_ptr<ska::RadioButton>(new ska::RadioButton(*this, group, ska::Point<int>(16, 96), ska::Button::MENU_DEFAULT_THEME_PATH + "radiobutton"));
	auto radioButton2 = std::unique_ptr<ska::RadioButton>(new ska::RadioButton(*this, group, ska::Point<int>(16, 128), ska::Button::MENU_DEFAULT_THEME_PATH + "radiobutton"));
	auto radioButton3 = std::unique_ptr<ska::RadioButton>(new ska::RadioButton(*this, group, ska::Point<int>(48, 96), ska::Button::MENU_DEFAULT_THEME_PATH + "radiobutton"));
	addWidget(radioButton1);
	addWidget(radioButton2);
	addWidget(radioButton3);

	auto listbox = std::unique_ptr<ska::ListBox<int>>(new ska::ListBox<int>(gui, *this, ska::Point<int>(16, 160), ska::Button::MENU_DEFAULT_THEME_PATH + "listbox", ska::Button::MENU_DEFAULT_THEME_PATH + "listbox-clean", nullptr));
	std::vector<int> vals;
	vals.push_back(0);
	vals.push_back(7);
	vals.push_back(25);
	vals.push_back(82);
	listbox->load(std::move(vals));
	addWidget(listbox);

	auto checkBox2 = std::unique_ptr<ska::CheckBox>(new ska::CheckBox(*this, ska::Point<int>(16, 192), ska::Button::MENU_DEFAULT_THEME_PATH + "checkbox", nullptr));
	addWidget(checkBox2);

	auto hSlider = std::unique_ptr<ska::HorizontalSlider>(new ska::HorizontalSlider(*this, ska::Button::MENU_DEFAULT_THEME_PATH + "slider", ska::Point<int>(48, 192), 100));
	addWidget(hSlider);

	resort();
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
//     this->addTextArea("Météo :", 20, buf);
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
