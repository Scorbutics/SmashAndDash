#ifndef DEF_GUI
#define DEF_GUI

#include <vector>
#include <SDL2/SDL.h>
#include <memory>
#include "WindowSettings.h"
#include "WindowTeam.h"
#include "WindowBag.h"
#include "ToolBar.h"
#include "WindowShop.h"
#include "../../ska/Utils/Observer.h"
#include "../../ska/Utils/Observable.h"
#include "../../ska/Graphic/Draw/DrawableFixedPriority.h"

using namespace std;

class WGameCore;

class GUI : public ska::Observer, public DrawableFixedPriority
{
    public:
    GUI();

	virtual void update(ska::Observable* obs, ska::EventArg* e);

    void dialogRefresh();
    void refresh();
	int isPositionOnButton(ska::Rectangle *pos);
    bool isMouseOnAWindow();
    bool isVisible() const override;
    void initButtons();
    void hide(bool x);
    bool isMovingAWindow();

	void resetInfoPNJWindow(Character* pnj);
	void resetInfoPokemonWindow(Character* pokemon);
	void resetAttackPokemonWindow(Character* pokemon);
    void resetAttackOpponentWindow(Character* op);

    void resetMovableWindowPokemonTeam(std::string action);
    void resetMovableWindowPokemonBag();
    void resetMovableWindowSettings();

    DialogMenu* getDialog();
    DialogMenu* getImgDialog();
    DialogMenu* getMovableWindow();
    DialogMenu*getInfoPNJWindow();
    DialogMenu* getInfoPokemonWindow();
    DialogMenu* getAttackPokemonWindow();
    DialogMenu* getAttackOpponentWindow();
    DialogMenu* getFacesetPkmn();
    DialogMenu* getClickMenu();
    DialogMenu* getFacesetOpponent();
    WindowSettings* getWindowSettings();
    WindowTeam* getWindowTeam();
    WindowBag* getWindowBag();
    ToolBar* getToolbar();
    WindowShop* getWindowShop();
    int getRefreshPNJWindowCount();
    DialogMenu* getButton(unsigned int id);
    size_t getButtonListSize();

    void setRefreshPNJWindowCount(int x);
    void setPNJ(Character_ptr *pnj);
    void setClickMenu();
	void dialogDisplay();
	void display() override;
private:
    vector<unique_ptr<DialogMenu>> m_buttonList;
    unique_ptr<DialogMenu> m_dial, m_imgDial, m_movableWindow, m_pnjInfoWindow, m_pokeInfoWindow, m_facesetPkmn, m_facesetOpponent, m_attackPokemon, m_attackOpponent, m_clickMenu;
    vector<int> m_buttonScroll;
	ska::Rectangle m_lastMousePos, m_curObjectPos;
    int m_side, m_lastMouseState, m_refreshCount;
    bool m_hide, m_isMovingWindow;
    Character *m_pnj;
    std::unique_ptr<WindowSettings> m_wSettings;
    std::unique_ptr<WindowTeam> m_wTeam;
    std::unique_ptr<WindowBag> m_wBag;
    std::unique_ptr<ToolBar> m_toolBar;
    std::unique_ptr<WindowShop> m_wShop;

};

#endif
