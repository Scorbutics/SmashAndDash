#pragma once
#include "../ska/ECS/Component.h"
#include "GUI/DialogMenu.h"

//TODO only descriptive class, the message will be displayed externally by sending an event to the GUI
class DialogComponent : public ska::Component {
public:
	DialogComponent() {}
// 	DialogComponent(ska::GUI& gui) : dialog(gui), m_gui(gui) {}
//
// 	void operator=(const DialogComponent& dm) {
// 		dialog = dm.dialog;
// 	}
//
// 	DialogComponent(const DialogComponent& dm) : m_gui(dm.m_gui), dialog(dm.m_gui) {
// 		operator=(dm);
// 	}

	/*DialogComponent(const DialogMenu& dm) : m_gui(dm.m_gui), dialog(dm.m_gui)  {
		dialog = dm;
	}*/
	virtual ~DialogComponent() {}

	//DialogMenu dialog;
//private:
	//ska::GUI& m_gui;
};
