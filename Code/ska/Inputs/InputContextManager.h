#pragma once
#include <vector>
#include <bitset>
#include "InputContext.h"

namespace ska {


	class InputContextManager
	{
	public:
		InputContextManager(RawInputListener& ril);
		InputContextManager(const InputContextManager& icm);

		void operator=(const InputContextManager& icm);
		static InputContextManager instantiateEmpty(InputContextManager& icm);
		void refresh(); 
		void addContext(InputContextPtr& icp);
		const InputActionContainer& getActions() const;
		const InputRangeContainer& getRanges() const;
		const InputToggleContainer& getToggles() const;
		const std::wstring& getTextInput() const;

		virtual ~InputContextManager();
	private:
		std::vector<InputContextPtr> m_contexts;
		InputRangeContainer m_ranges;
		InputActionContainer m_actions;
		InputToggleContainer m_toggles;
		std::wstring m_textInput;
		RawInputListener& m_ril;
	};
}

