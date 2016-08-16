#pragma once
#include <vector>
#include <memory>
#include "MouseCursor.h"
#include "../../ska/Utils/Observer.h"
#include "../../ska/Utils/Observable.h"
#include "../../ska/Graphic/Draw/DrawableFixedPriority.h"
#include "../../ska/Graphic/Point.h"
#include "WindowSettings.h"
#include "WindowTeam.h"
#include "WindowBag.h"
#include "ToolBar.h"
#include "WindowShop.h"

class DialogMenu;
class IDialogMenu;

using WindowBagPtr = std::unique_ptr<WindowBag>;
using IDialogMenuPtr = std::unique_ptr<IDialogMenu>;
using WindowSettingsPtr = std::unique_ptr<WindowSettings>;
using WindowTeamPtr = std::unique_ptr<WindowTeam>;
using WindowBagPtr = std::unique_ptr<WindowBag>;
using ToolBarPtr = std::unique_ptr<ToolBar>;
using WindowShopPtr = std::unique_ptr<WindowShop>;

using DialogMenuPtr = std::unique_ptr<DialogMenu>;

namespace ska {
	class Window;
}

class GUI :  public ska::DrawableFixedPriority {

public:
	GUI(const ska::Window& w, const ska::InputContextManager& playerICM);

	//virtual void update(ska::Observable<const int>* obs, const ska::EventArg& e, const int& i) override;

    void dialogRefresh();
	int addDialog(IDialogMenuPtr& d);
	bool existDialog(const unsigned int index) const;
    void refresh();
	int isPositionOnButton(const ska::Point<float>& pos) const;
    bool isMouseOnAWindow();
    bool isVisible() const override;
	void initButtons(const ska::Window& w);
    void hide(bool x);
    bool isMovingAWindow();

	/*void resetInfoPNJWindow(Character* pnj);
	void resetInfoPokemonWindow(Character* pokemon);
	void resetAttackPokemonWindow(Character* pokemon);
    void resetAttackOpponentWindow(Character* op);*/

    void resetMovableWindowPokemonTeam(std::string action);
    void resetMovableWindowPokemonBag();
    void resetMovableWindowSettings();

    int getRefreshPNJWindowCount();
    DialogMenuPtr& getButton(unsigned int id);
    size_t getButtonListSize();

    void setRefreshPNJWindowCount(int x);
    void setClickMenu();
	void dialogDisplay() const;
	void display() const override;

	void update();

private:
    std::vector<DialogMenuPtr> m_buttonList;
	std::vector<IDialogMenuPtr> m_extraWindows;
	DialogMenuPtr m_movableWindow, m_pnjInfoWindow, m_pokeInfoWindow, m_facesetPkmn, m_facesetOpponent, m_attackOpponent, m_clickMenu;
	std::vector<int> m_buttonScroll;
	ska::Point<int> m_lastMousePos, m_curObjectPos;
    int m_side, m_lastMouseState, m_refreshCount;
    bool m_hide, m_isMovingWindow;
	const ska::Window& m_window;
    std::unique_ptr<WindowSettings> m_wSettings;
    std::unique_ptr<WindowTeam> m_wTeam;
    std::unique_ptr<WindowBag> m_wBag;
    std::unique_ptr<ToolBar> m_toolBar;
    std::unique_ptr<WindowShop> m_wShop;

	const ska::InputContextManager& m_playerICM;
	MouseCursor m_mouseCursor;

};
