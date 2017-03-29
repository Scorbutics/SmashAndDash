#pragma once
#include "../../Component.h"
//#include "GUI/DialogMenu.h"

namespace ska {
//TODO only descriptive class, the message will be displayed externally by sending an event to the GUI
    class DialogComponent : public Component {
    public:
        DialogComponent();
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
        virtual ~DialogComponent() = default;

        //DialogMenu dialog;
    //private:
        //ska::GUI& m_gui;
    };
}
