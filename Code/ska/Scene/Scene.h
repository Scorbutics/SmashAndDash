#pragma once
#include "HasGraphic.h"
#include "HasLogic.h"
#include "../Inputs/InputContextManager.h"
#include "../Inputs/RawInputListener.h"
#include "../Inputs/System/InputSystem.h"

namespace ska {
	class Scene : public HasGraphic, public HasLogic
	{
	public:
		Scene(RawInputListener& ril) : m_inputCManager(ril) {}
		
		const InputContextManager& getInputContextManager() const {
			return m_inputCManager;
		}

		virtual ~Scene() {}
	protected:
		InputContextManager m_inputCManager;
	};
}

