#pragma once
#include <unordered_map>
#include <memory>
#include <unordered_set>
#include "InputAction.h"
#include "RawInputListener.h"

namespace ska {
	class InputContext {
	public:
		InputContext(){}
		
		void buildCodeMap() {
			buildCodeMap(m_codesMapper);
		}

		void queryActions(RawInputListener& ril, std::unordered_set<InputAction>& actions) {
			KeyInput& keys = ril.getKeyInput();
			MouseInput& mouseKeys = ril.getMouseInput();
			for (auto& it = m_codesMapper.begin(); it != m_codesMapper.end(); it++) {
				if (keys.getKeyState(it->first) == 1) {
					actions.insert(it->second);
				}
			}
		}

		virtual ~InputContext(){};

	protected:
		virtual void buildCodeMap(std::unordered_map<int, InputAction>& codeMap) = 0;

	private:
		std::unordered_map<int, InputAction> m_codesMapper;
	};

	typedef std::unique_ptr<InputContext> InputContextPtr;
}