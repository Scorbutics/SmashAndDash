#pragma once
#include <vector>
#include <bitset>
#include "InputContext.h"

namespace ska {


	class InputContextManager
	{
	public:
		InputContextManager(RawInputListener& ril);

		void refresh(); 
		const InputActionContainer& getActions() const;
		const InputRangeContainer& getRanges() const;
		const InputToggleContainer& getToggles() const;

		virtual ~InputContextManager();
	private:
		std::vector<InputContextPtr> m_contexts;
		InputRangeContainer m_ranges;
		InputActionContainer m_actions;
		InputToggleContainer m_toggles;
		RawInputListener& m_ril;
	};
}

