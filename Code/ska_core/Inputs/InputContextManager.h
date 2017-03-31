#pragma once
#include <vector>
#include <bitset>
#include <unordered_map>
#include "InputContext.h"
#include "EnumContextManager.h"

namespace ska {

	class InputContextManager {
	public:
		explicit InputContextManager(RawInputListener& ril);
		InputContextManager(const InputContextManager& icm);

		void operator=(const InputContextManager& icm);
		static InputContextManager instantiateEmpty(InputContextManager& icm);
		void refresh(); 
		void addContext(EnumContextManager ecm, InputContextPtr&& icp);
		void disableContext(EnumContextManager ecm, bool disable);
		const InputActionContainer& getActions() const;
		const InputRangeContainer& getRanges() const;
		const InputToggleContainer& getToggles() const;
		const std::wstring& getTextInput() const;

		virtual ~InputContextManager();
	private:
		std::unordered_map<EnumContextManager, InputContextPtr> m_contexts;
		std::unordered_map<EnumContextManager, InputContextPtr> m_disabledContexts;
		InputRangeContainer m_ranges;
		InputActionContainer m_actions;
		InputToggleContainer m_toggles;
		std::wstring m_textInput;
		RawInputListener& m_ril;
	};
}

