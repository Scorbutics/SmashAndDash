#pragma once
#include "CameraSystem.h"
#include "Rectangle.h"

namespace ska {
	class CameraFollowSystem : public CameraSystem {
	public:
		CameraFollowSystem(EntityManager& entityManager, const unsigned int screenW, const unsigned int screenH);
		~CameraFollowSystem();

		virtual void refresh(unsigned int ellapsedTime) override;
	private:
		Rectangle m_follow;
	};
}
