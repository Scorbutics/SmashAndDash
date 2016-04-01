#pragma once
#include "CameraSystem.h"
#include "../Point.h"

namespace ska {
	class CameraFollowSystem : public CameraSystem {
	public:
		CameraFollowSystem(EntityManager& entityManager, const unsigned int screenW, const unsigned int screenH);
		~CameraFollowSystem();

		virtual void refresh() override;
	private:
		Point<int> m_follow;
	};
}