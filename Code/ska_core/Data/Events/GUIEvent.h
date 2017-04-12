#pragma once

#include "ECS/ECSDefines.h"

namespace ska {

    enum GUIEventType {
        REMOVE_WINDOW
    };


	struct GUIEvent {
		GUIEvent(const GUIEventType& t) : type(t) {
		}

		void operator=(const GUIEvent&) = delete;
		virtual ~GUIEvent() = default;

		std::string windowName;
        const GUIEventType type;

	};
}
