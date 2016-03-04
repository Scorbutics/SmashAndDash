#pragma once
#include "Component.h"
namespace ska {
	class PositionComponent : public Component
	{
	public:
		PositionComponent();

		const int getX() const;
		const int getY() const;
		const int getZ() const;

		void setX(const int x);
		void setY(const int y);
		void setZ(const int z);

		virtual ~PositionComponent();
	private:
		int m_x;
		int m_y;
		int m_z;

	};
}
