#ifndef DEF_ANIMATION
#define DEF_ANIMATION

#include <SDL2/SDL.h>

//si bool isVertical = true : les frames de l'animation sont alignées verticalement dans le fichier image. Si false, horizontalement
class Animation
{
    public:
    Animation(unsigned int delay, int frameNumber, bool isVertical, SDL_Rect offsetFrame);
	Animation(unsigned int delay, int frameNumber, bool isVertical, int x = 0, int y = 0, unsigned int width = 0, unsigned int height = 0);
    void resetCount();
    void stop(bool x);
    void nextFrame();
	void resetCycles();

    SDL_Rect getOffsetAndFrameSize();
    SDL_Rect getRectOfCurrentFrame();
    unsigned int getCount();
	unsigned int getDelay();
	unsigned int getNumberFrames();
	unsigned int getCycles();
	int getPosition();
	int getMaxPosition();

    void setOffsetAndFrameSize(SDL_Rect s);
    void setCount(unsigned int x);
    void setDelay(unsigned int x);
    void setNumberFrames(unsigned int x);
	SDL_Rect getOffsetBase();

private:
    SDL_Rect m_offsetBase, m_offset;
    int m_frameWidth, m_frameHeight, m_position, m_sensAni, m_frameNumber;
    unsigned int m_count, m_countMAX, m_cycles;
    bool m_isVertical, m_active;
};

#endif
