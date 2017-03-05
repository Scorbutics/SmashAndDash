#pragma once
#include "CameraSystem.h"
#include "../Rectangle.h"

namespace ska {

	class CameraFixedSystem : public CameraSystem {
	public:
		CameraFixedSystem(EntityManager& entityManager, const unsigned int screenW, const unsigned int screenH, Point<int> point);
		CameraFixedSystem& operator=(const CameraFixedSystem&) = delete;
		~CameraFixedSystem();

		virtual void refresh() override;
	private:
		Rectangle m_fixed;
		
	};
}