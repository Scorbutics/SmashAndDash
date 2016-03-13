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

		virtual ~InputContextManager();
	private:
		std::vector<InputContextPtr> m_contexts;
		InputRangeContainer m_ranges;
		InputActionContainer m_actions;
		RawInputListener& m_ril;
	};
}

