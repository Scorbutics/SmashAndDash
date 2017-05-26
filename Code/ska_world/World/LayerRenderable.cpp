#include "Exceptions/IndexOutOfBoundsException.h"
#include "Utils/StringUtils.h"

#include "World.h"
#include "LayerRenderable.h"

ska::LayerRenderable::LayerRenderable(World& w) : 
	m_world(w) {
	m_block.reserve(20);
}

void ska::LayerRenderable::update() {
	const auto* cameraPos = m_world.getView();

	if (cameraPos == nullptr) {
		return;
	}

	/* TODO external view ? (avoid camera reference)*/
	const auto absORelX = NumberUtils::absolute(cameraPos->x);
	const auto absORelY = NumberUtils::absolute(cameraPos->y);
	const auto cameraPositionStartBlockX = absORelX / m_world.getBlockSize();
	const auto cameraPositionStartBlockY = absORelY / m_world.getBlockSize();
	const auto cameraPositionEndBlockX = (absORelX + cameraPos->w) / m_world.getBlockSize();
	const auto cameraPositionEndBlockY = (absORelY + cameraPos->h) / m_world.getBlockSize();

	const auto layerPixelsX = m_world.getPixelWidth();
	const auto layerPixelsY = m_world.getPixelHeight();

	for (auto i = cameraPositionStartBlockX; i <= cameraPositionEndBlockX; i++) {
		for (auto j = cameraPositionStartBlockY; j <= cameraPositionEndBlockY; j++) {
			auto currentXBlock = i * m_world.getBlockSize();
			auto currentYBlock = j * m_world.getBlockSize();

			if (currentXBlock < layerPixelsX && currentYBlock < layerPixelsY) {
				auto b = m_block[i][j];
				if (b != nullptr) {
					m_world.getChipset().update(*b);
				}
			}

		}
	}
}

void ska::LayerRenderable::display() const {
	const auto* cameraPos = m_world.getView();

	if (cameraPos == nullptr) {
		return;
	}

	/* TODO external view ? (avoid camera reference)*/
	const auto absORelX = NumberUtils::absolute(cameraPos->x);
	const auto absORelY = NumberUtils::absolute(cameraPos->y);
	const auto cameraPositionStartBlockX = absORelX / m_world.getBlockSize();
	const auto cameraPositionStartBlockY = absORelY / m_world.getBlockSize();
	const auto cameraPositionEndBlockX = (absORelX + cameraPos->w) / m_world.getBlockSize();
	const auto cameraPositionEndBlockY = (absORelY + cameraPos->h) / m_world.getBlockSize();

	const auto layerPixelsX = m_world.getPixelWidth();
	const auto layerPixelsY = m_world.getPixelHeight();

	for (auto i = cameraPositionStartBlockX; i <= cameraPositionEndBlockX; i++) {
		for (auto j = cameraPositionStartBlockY; j <= cameraPositionEndBlockY; j++) {
			auto currentXBlock = i * m_world.getBlockSize();
			auto currentYBlock = j * m_world.getBlockSize();
			if (currentXBlock < layerPixelsX && currentYBlock < layerPixelsY) {
				auto b = m_block[i][j];
				if (b != nullptr) {
					const ska::Point<int> absoluteCurrentPos(currentXBlock - absORelX, currentYBlock - absORelY);
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
	
	throw IndexOutOfBoundsException("block at coordinates (" + StringUtils::intToStr(i) + "; " + StringUtils::intToStr(j) + ") cannot be accessed");
}

void ska::LayerRenderable::reset(std::vector<std::vector<BlockRenderable*>>& block) {
	m_block = move(block);
}

