#include <iostream>
#include <utility>
#include "SpriteAnimationManager.h"
#include "SpriteAnimation.h"

ska::SpriteAnimationManager::SpriteAnimationManager()
{
}

ska::SpriteAnimation* ska::SpriteAnimationManager::play(int type, int id, Point<int> pos, unsigned int cycles, int alpha, unsigned int framesNumber, unsigned int delay)
{
	const size_t size = m_sprites.size();
	size_t i;
	for(i = 0; i < size && m_sprites[i] != nullptr; i++);
	
	if(i != size)
	{
		m_sprites[i] = std::move(SpriteAnimationPtr(new SpriteAnimation(type, id, pos, framesNumber, delay, alpha)));
		m_cycles[i] = cycles;
	}
	else
	{
		m_sprites.push_back(SpriteAnimationPtr(new SpriteAnimation(type, id, pos, framesNumber, delay, alpha)));
		m_cycles.push_back(cycles);
	}

	return &(*m_sprites[i]);
}

void ska::SpriteAnimationManager::refresh() {
	for (unsigned int i = 0; i < m_sprites.size(); i++) {
		if (m_sprites[i] != nullptr) {
			m_sprites[i]->refresh();
			if (m_sprites[i]->getAnimation()->getCycles() == m_cycles[i]) {
				m_sprites[i] = nullptr;
				m_cycles[i] = 0;
			}
		}
	}
}

void ska::SpriteAnimationManager::display() const {
	for (unsigned int i = 0; i < m_sprites.size(); i++) {
		if (m_sprites[i] != nullptr) {
			m_sprites[i]->display();
		}
	}
}

bool ska::SpriteAnimationManager::isVisible() const {
	return !m_sprites.empty();
}

void ska::SpriteAnimationManager::reset() {
	m_sprites.clear();
	m_cycles.clear();
}

ska::SpriteAnimationManager::~SpriteAnimationManager() {
}
