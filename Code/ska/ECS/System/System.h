#pragma once
#include <iostream>
#include "../ECSDefines.h"
#include "../Component/Component.h"
#include "../EntityManager.h"

namespace ska {

	class System {
	public :
		System(EntityManager& entityManager) : m_entityManager(entityManager) { }
		
		void refresh(EntityIdContainer& entityId) {
			for (EntityId id : entityId) {
				refresh(id);
			}
		}

		~System(){}

	private:
		

	protected:
		virtual void refresh(EntityId& entity) = 0;
		EntityManager& m_entityManager;
				
	};
}