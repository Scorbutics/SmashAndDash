#pragma once
#include <unordered_map>
#include <memory>
#include "InputAction.h"
#include "InputRange.h"

#include "RawInputListener.h"

namespace ska {
	class InputContext {
	public:
		InputContext(){}
		
		void buildCodeMap() {
			buildCodeMap(m_codesMapper);
		}

		virtual void queryActions(RawInputListener& ril, InputActionContainer& actions) = 0;
		virtual void queryRanges(RawInputListener& ril, InputRangeContainer& actions) = 0;

		virtual ~InputContext(){};

	protected:
		virtual void buildCodeMap(std::unordered_map<int, InputAction>& codeMap) = 0;
		std::unordered_map<int, InputAction> m_codesMapper;
	
	};

	typedef std::unique_ptr<InputContext> InputContextPtr;
}