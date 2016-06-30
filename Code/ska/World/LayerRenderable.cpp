#include <iostream>
#include <string>
#include <fstream>

#include "../Exceptions/IndexOutOfBoundsException.h"
#include "../Utils/StringUtils.h"

#include "World.h"
#include "LayerRenderable.h"

ska::LayerRenderable::LayerRenderable(ska::World& w) : m_world(w) {
	m_block.reserve(20);
}

void ska::LayerRenderable::update() {
	ska::Rectangle absoluteCurrentPos;
	const ska::Rectangle* cameraPos = m_world.getView();

	if (cameraPos == NULL) {
		return;
	}

	/* TODO external view ? (avoid camera reference)*/
	const unsigned int absORelX = NumberUtils::absolute(cameraPos->x);
	const unsigned int absORelY = NumberUtils::absolute(cameraPos->y);
	const unsigned int cameraPositionStartBlockX = absORelX / m_world.getBlockSize();
	const unsigned int cameraPositionStartBlockY = absORelY / m_world.getBlockSize();
	const unsigned int cameraPositionEndBlockX = (absORelX + cameraPos->w) / m_world.getBlockSize();
	const unsigned int cameraPositionEndBlockY = (absORelY + cameraPos->h) / m_world.getBlockSize();

	const unsigned int layerPixelsX = m_world.getPixelWidth();
	const unsigned int layerPixelsY = m_world.getPixelHeight();

	for (unsigned int i = cameraPositionStartBlockX; i <= cameraPositionEndBlockX; i++) {
		for (unsigned int j = cameraPositionStartBlockY; j <= cameraPositionEndBlockY; j++) {
			unsigned int currentXBlock = i*m_world.getBlockSize();
			unsigned int currentYBlock = j*m_world.getBlockSize();
			absoluteCurrentPos.x = currentXBlock - absORelX;
			absoluteCurrentPos.y = currentYBlock - absORelY;

			if (currentXBlock < layerPixelsX && currentYBlock < layerPixelsY) {
				BlockRenderable* b = m_block[i][j];
				if (b != nullptr) {
					m_world.getChipset().update(*b);
				}
			}
			
		}
	}
}

void ska::LayerRenderable::display() const {

	ska::Rectangle absoluteCurrentPos;
	const ska::Rectangle* cameraPos = m_world.getView();

	if (cameraPos == NULL) {
		return;
	}

	/* TODO external view ? (avoid camera reference)*/
	const unsigned int absORelX = NumberUtils::absolute(cameraPos->x);
	const unsigned int absORelY = NumberUtils::absolute(cameraPos->y);
	const unsigned int cameraPositionStartBlockX = absORelX / m_world.getBlockSize();
	const unsigned int cameraPositionStartBlockY = absORelY / m_world.getBlockSize();
	const unsigned int cameraPositionEndBlockX = (absORelX + cameraPos->w) / m_world.getBlockSize();
	const unsigned int cameraPositionEndBlockY = (absORelY + cameraPos->h) / m_world.getBlockSize();

	const unsigned int layerPixelsX = m_world.getPixelWidth();
	const unsigned int layerPixelsY = m_world.getPixelHeight();

	for (unsigned int i = cameraPositionStartBlockX; i <= cameraPositionEndBlockX; i++) {
		for (unsigned int j = cameraPositionStartBlockY; j <= cameraPositionEndBlockY; j++) {

			unsigned int currentXBlock = i*m_world.getBlockSize();
			unsigned int currentYBlock = j*m_world.getBlockSize();
			absoluteCurrentPos.x = currentXBlock - absORelX;
			absoluteCurrentPos.y = currentYBlock - absORelY;

			if (currentXBlock < layerPixelsX && currentYBlock < layerPixelsY) {
				BlockRenderable* b = m_block[i][j];
				if (b != nullptr) {
					/* TODO passer la propriété BLOCK_PROP_WIND_SENSITIVITY en script de chipset */
					m_world.getChipset().render(absoluteCurrentPos, *b);
				}
			}
		}
	}
}

bool ska::LayerRenderable::isVisible() const {
	return !m_block.empty();
}

void ska::LayerRenderable::clear() {
	m_block.clear();
}

ska::BlockRenderable* ska::LayerRenderable::getBlock(const unsigned int i, const unsigned int j) {
	if (i < m_block.size() && j < m_block[i].size()) {
		return m_block[i][j];
	}
	else {
		throw ska::IndexOutOfBoundsException("block at coordinates (" + ska::StringUtils::intToStr(i) + "; " + ska::StringUtils::intToStr(j) + ") cannot be accessed");
	}

}

void ska::LayerRenderable::reset(std::vector<std::vector<BlockRenderable*>>& block) {
	m_block = std::move(block);
}

