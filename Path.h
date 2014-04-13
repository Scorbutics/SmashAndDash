#ifndef DEF_PATH
#define DEF_PATH

#include <string>
#include <SDL2/SDL.h>

class World;

using namespace std;

class Path
{
    public:
    Path();

    bool isPredefined();
    bool isMotionless();
    bool isRandom();
    void findPath(SDL_Rect from, SDL_Rect to);

    int getPathDirection(unsigned int number);
    string getPathString();
    int getNextPathDirection();

    void setPathString(string path);

    private:
    string m_pathDirections;
    unsigned int m_pos, m_type;
};

#endif
