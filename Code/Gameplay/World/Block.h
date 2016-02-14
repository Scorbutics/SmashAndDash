#ifndef DEF_BLOCK
#define DEF_BLOCK

#include <SDL2/SDL.h>
#include <vector>
#include <memory>
#include "../../Graphic/Animation.h"


class WGameCore;

class Block
{
	public:
	Block(SDL_Rect corr, unsigned int idSprite, unsigned int properties, bool auto_animation, int col);
    Block(SDL_Rect corr, SDL_Rect posChipset, unsigned int properties, bool auto_animation, int col);
	unsigned int getProperties();
	int getCollision();
	unsigned int getID();
	void refresh(SDL_Rect pos, const SDL_Rect* rectAnim = NULL);
	~Block();
	void setSpriteFrame(unsigned int x);

	private:
	unsigned int m_properties, m_idSprite;
	SDL_Rect m_spritePosInChipset;
	bool m_auto_animation;
	int m_collision;
	Animation m_anim;

};

typedef std::unique_ptr<Block> Block_ptr;

#endif
