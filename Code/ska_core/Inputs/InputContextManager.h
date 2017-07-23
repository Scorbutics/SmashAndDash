#pragma once
#include <vector>
#include <bitset>
#include <unordered_map>
#include "InputContext.h"
#include "EnumContextManager.h"
#include "../Data/Events/GameEventDispatcher.h"

namespace ska {

	class InputContextManager {
	public:
		InputContextManager(RawInputListener& ril, GameEventDispatcher& ged);
		InputContextManager(InputContextManager&& icm) = default;

		InputContextManager& operator=(const InputContextManager& icm) = delete;
		InputContextManager& operator=(InputContextManager&& icm) = default;
		void refresh(); 
		void addContext(EnumContextManager ecm, InputContextPtr&& icp);
		void disableContext(EnumContextManager ecm, bool disable);
		const InputActionContainer& getActions() const;
		const InputRangeContainer& getRanges() const;
		const InputToggleContainer& getToggles() const;
		const std::wstring& getTextInput() const;

		virtual ~InputContextManager();
	
	private:
		GameEventDispatcher& m_eventDispatcher;
		std::unordered_map<EnumContextManager, InputContextPtr> m_contexts;
		std::unordered_map<EnumContextManager, InputContextPtr> m_disabledContexts;
		InputRangeContainer m_ranges;
		InputActionContainer m_actions;
		InputToggleContainer m_toggles;
		std::wstring m_textInput;
		RawInputListener& m_ril;
		WindowInput m_windowInput;
	};
}

