#include "Block.h"

ska::Block::Block(int corrFileWidth, Point<int> posChipset, unsigned int properties, int col) {
	m_properties = properties;
    m_collision = col;
	m_id = posChipset.x + posChipset.y * corrFileWidth;
}

unsigned int ska::Block::getProperties() {
	return m_properties;
}

int ska::Block::getCollision() const {
    return m_collision;
}

unsigned int ska::Block::getID() const {
	return m_id;
}
