#pragma once

namespace ska {
	class InputContextManager;

	struct InputKeyEvent {
		explicit InputKeyEvent(const InputContextManager& icm_) :
			icm(icm_) {
		}

		void operator=(const InputKeyEvent&) = delete;
		virtual ~InputKeyEvent() = default;

		const InputContextManager& icm;
	};
}
