#pragma once

namespace ska {
	class InputContextManager;

	struct InputMouseEvent {
		explicit InputMouseEvent(InputContextManager& icm_) :
			icm(icm_) {
		}

		void operator=(const InputMouseEvent&) = delete;
		virtual ~InputMouseEvent() = default;

		InputContextManager& icm;
	};
}
