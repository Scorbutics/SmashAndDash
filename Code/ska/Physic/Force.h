#ifndef DEF_FORCE
#define DEF_FORCE

#include "../Graphic/Rectangle.h"

namespace ska {
	class Force
	{
	public:
		Force();
		Force(ska::Rectangle pos, float power, float angle);

		ska::Rectangle getPos();
		float getPower() const;
		float getAngle() const;

		void setPos(ska::Rectangle pos);
		void setPower(float power);
		void setAngle(float angle);

		~Force();

	protected:
		ska::Rectangle m_pos;
		float m_angle, m_power;

	};
}
#endif
