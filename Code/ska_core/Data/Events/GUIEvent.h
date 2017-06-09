#pragma once

#include "../../Point.h"

namespace ska {

	class BalloonDialog;

    enum GUIEventType {
        REMOVE_WINDOW,
		ADD_BALLOON,
		REFRESH_BALLOON
    };


	struct GUIEvent {
		GUIEvent(const GUIEventType& t) : balloonHandle(nullptr), delay(0), type(t) {
		}

		void operator=(const GUIEvent&) = delete;
		virtual ~GUIEvent() = default;

		BalloonDialog* balloonHandle;
		ska::Point<int> balloonPosition;
		int delay;
		std::string text;
		std::string windowName;
        const GUIEventType type;

	};
}
