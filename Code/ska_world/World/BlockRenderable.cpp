#include "BlockRenderable.h"

ska::BlockRenderable::BlockRenderable(const unsigned int blockSize, Point<int> posChipset, bool auto_animation) : 
	m_anim(375, 4, true, 0, 0, blockSize, blockSize) {
	m_auto_animation = auto_animation;

	m_spritePosInChipset.x = posChipset.x*blockSize;
	m_spritePosInChipset.y = posChipset.y*blockSize;
	m_spritePosInChipset.w = blockSize;
	m_spritePosInChipset.h = blockSize;

}

void ska::BlockRenderable::setSpriteFrame(unsigned int x) {
	m_anim.setCurrentFrame(x);
	m_anim.stop(true);
}

void ska::BlockRenderable::refresh() {
	if (m_auto_animation) {
		m_anim.getRectOfCurrentFrame();
	}
}

ska::Rectangle ska::BlockRenderable::determineFrame(ska::Point<int>, const Rectangle* rectAnim) const {
	auto buf = m_spritePosInChipset;

	if (m_auto_animation) {
		Rectangle bufRectAnim;

		if (rectAnim != nullptr) {
			bufRectAnim = *rectAnim;
		} else  {
			bufRectAnim = m_anim.getOffsetAndFrameSize();
		}

		buf.x += bufRectAnim.x;
		buf.y += bufRectAnim.y;
	}

	return buf;
}
