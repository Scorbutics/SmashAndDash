#ifndef DEF_PATH
#define DEF_PATH

#include <string>
#include "../../Graphic/ska/Rectangle.h"
#include "Node.h"

class World;

namespace ska {
	class Path
	{
	public:
		Path();

		bool isPredefined();
		bool isMotionless();
		bool isRandom();
		void findPath(ska::Rectangle from, ska::Rectangle to, const unsigned int blockSize);

		void createGraphe(std::vector<std::vector<Node>> *graphe);

		int getPathDirection(unsigned int number);
		std::string getPathString();
		int getNextPathDirection();

		void setPathString(std::string path);

	private:
		std::string m_pathDirections;
		unsigned int m_pos, m_type;
	};
}
#endif