#pragma once
#include "CameraSystem.h"
#include "../Point.h"

namespace ska {

	class CameraFixedSystem : public CameraSystem {
	public:
		CameraFixedSystem(EntityManager& entityManager, const unsigned int screenW, const unsigned int screenH, Point<int> point);
		~CameraFixedSystem();

		virtual void refresh() override;
	private:
		Point<int> m_fixed;
	};
}