#include <iostream>
#include "SpriteAnimationManager.h"
#include "SpriteAnimation.h"

using namespace std;

SpriteAnimationManager::SpriteAnimationManager()
{
}

SpriteAnimation* SpriteAnimationManager::play(int type, int id, SDL_Rect pos, unsigned int cycles, int alpha, unsigned int framesNumber, unsigned int delay)
{
	const size_t size = m_sprites.size();
	size_t i;
	for(i = 0; i < size && m_sprites[i] != NULL; i++);
	
	if(i != size)
	{
		m_sprites[i] = unique_ptr<SpriteAnimation>(new SpriteAnimation(type, id, pos, framesNumber, delay, alpha));
		m_cycles[i] = cycles;
	}
	else
	{
		m_sprites.push_back(unique_ptr<SpriteAnimation>(new SpriteAnimation(type, id, pos, framesNumber, delay, alpha)));
		m_cycles.push_back(cycles);
	}

	return &(*m_sprites[i]);
}

void SpriteAnimationManager::display()
{
	for(unsigned int i = 0; i < m_sprites.size(); i++)
	{
		if(m_sprites[i] != NULL)
		{
			m_sprites[i]->refresh();
			if(m_sprites[i]->getAnimation()->getCycles() == m_cycles[i])
			{
				m_sprites[i] = NULL;
				m_cycles[i] = 0;
			}
		}
	}
}

void SpriteAnimationManager::reset()
{
	m_sprites.clear();
	m_cycles.clear();
}

SpriteAnimationManager::~SpriteAnimationManager()
{
}