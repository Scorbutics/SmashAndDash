#ifndef DEF_FORCE
#define DEF_FORCE

#include "../Graphic/Rectangle.h"

namespace ska {
	class Force
	{
	public:
		Force();
		Force(Rectangle pos, float power, float angle);

		Rectangle getPos();
		float getPower() const;
		float getAngle() const;

		void setPos(Rectangle pos);
		void setPower(float power);
		void setAngle(float angle);

		~Force();

	protected:
		Rectangle m_pos;
		float m_angle, m_power;

	};
}
#endif
