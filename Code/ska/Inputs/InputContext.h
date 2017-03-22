#pragma once
#include <unordered_map>
#include <memory>
#include "InputAction.h"
#include "InputRange.h"
#include "InputToggle.h"

#include "RawInputListener.h"

namespace ska {
	class InputContext {
	public:
		InputContext(){}
		
		void buildCodeMap() {
			buildCodeMap(m_actionsMapper, m_togglesMapper);
		}

		virtual void queryActions(RawInputListener& ril, InputActionContainer& actions) = 0;
		virtual void queryRanges(RawInputListener& ril, InputRangeContainer& ranges) = 0;
		virtual void queryToggles(RawInputListener& ril, InputToggleContainer& toggles) = 0;
		virtual const std::wstring& queryText(RawInputListener& ril) = 0;

		virtual ~InputContext(){};

	protected:
		virtual void buildCodeMap(std::unordered_map<int, InputAction>& actions, std::unordered_map<int, InputToggle>& toggles) = 0;
		std::unordered_map<int, InputAction> m_actionsMapper;
		std::unordered_map<int, InputToggle> m_togglesMapper;
	
	};

	typedef std::unique_ptr<InputContext> InputContextPtr;
}
