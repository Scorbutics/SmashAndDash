#ifndef DEF_PATH
#define DEF_PATH

#include <string>
#include "../../Graphic//Rectangle.h"

class World;

using namespace std;

class Path
{
    public:
    Path();

    bool isPredefined();
    bool isMotionless();
    bool isRandom();
	void findPath(ska::Rectangle from, ska::Rectangle to);

    int getPathDirection(unsigned int number);
    string getPathString();
    int getNextPathDirection();

    void setPathString(string path);

    private:
    string m_pathDirections;
    unsigned int m_pos, m_type;
};

#endif
