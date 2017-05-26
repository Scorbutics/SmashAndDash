#pragma once
#include "../../Component.h"
#include "../../../Data/Events/GUIEvent.h"

namespace ska {
	class BalloonDialog;

    class DialogComponent : public Component {
    public:
        DialogComponent();
        virtual ~DialogComponent() = default;

	    BalloonDialog* handle;
		std::string name;
    };
}
