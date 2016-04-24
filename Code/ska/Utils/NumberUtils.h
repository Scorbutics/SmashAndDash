#pragma once
#include "../Exceptions/IllegalArgumentException.h"
#include "../Graphic/Point.h"

namespace ska{
	class NumberUtils
	{

	private:
		NumberUtils();

	public:
		~NumberUtils();

		static float random(float min, float max);
		static double random(double min, double max);
		static double random();
		static float exponential(float i);
		static int random(int min, int max);
		static double arctan(int x, int y);
		static double arctan(double slope);

		template <typename T>
		static T maximum(T a, T b) {
			return a > b ? a : b;
		}

		static double cosinus(double angle);
		static double sinus(double angle);

		static double absolute(double i);
		static unsigned int absolute(int i);

		static unsigned int squareroot(const unsigned int i);
		static unsigned int getMax10Pow(const int num);

		template <typename T>
		static Point<T> cartesian(const T radius, const double angle) {
			Point<T> result;
			result.x = radius * (float) cosinus(angle);
			result.y = radius * (float) sinus(angle);
			return result;
		}

		template <typename T>
		static PolarPoint<T> polar(const T x, const T y) {
			PolarPoint<T> result;
			if (y == 0) {
				throw ska::IllegalArgumentException("Error while converting cartesian coordinates to polar : slope cannot be infinite (x must be != 0)");
			}

			result.angle = arctan(x, y);
			result.radius = squareroot(x * x + y * y);
			return result;
		}

		/*	
			In a non-standard coordinate system (as we are, in computer graphics we have the y axis down and not up),
			Matrix rotation equation 
			|x'|   |cos(angle)	sin(angle)	| * |x|
			|y'| = |-sin(angle)	cos(angle)	|	|y|
			with (x'; y') the resulting coords, (x; y) the current ones and angle the angle of rotation
		*/
		template <typename T>
		static Point<T> rotate(const Point<T>& origin, double angle, const Point<T>& currentPoint) {
			/* First, we have to work with a (0;0) origin to make the rotation correctly */
			Point<T> diff = currentPoint - origin;
			
			/* Then we apply the multiplication with the rotation matrix with angle "angle" */
			Point<T> result = diff;
			result.x = diff.x * cosinus(angle) + diff.y * sinus(angle);
			result.y = -diff.x * sinus(angle) + diff.y * cosinus(angle);

			/* And we add the origin again */
			return result + origin;
		}
	};
}

