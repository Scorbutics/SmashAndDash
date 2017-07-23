#pragma once

namespace ska {

    enum StateEventType {
        FIRST_STATE_LOADED,
		STATE_CHANGED
    };

	struct StateEvent {
		StateEvent(const StateEventType& t) : 
			type(t) {
		}

		void operator=(const StateEvent&) = delete;
		virtual ~StateEvent() = default;

        const StateEventType type;

	};
}
