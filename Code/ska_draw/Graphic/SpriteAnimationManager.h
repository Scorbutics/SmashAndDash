#ifndef DEF_SPRITEANIMATIONMANAGER
#define DEF_SPRITEANIMATIONMANAGER

#include <memory>
#include <vector>
#include "./Draw/DrawableFixedPriority.h"
#include "Point.h"


namespace ska {
	class SpriteAnimation;
	class SpriteAnimationManager : public DrawableFixedPriority
	{
	public:
		SpriteAnimationManager();
		void reset();
		SpriteAnimation* play(int type, int id, Point<int> pos, unsigned int cycles, int alpha = -1, unsigned int framesNumber = 4, unsigned int delay = 300);
		~SpriteAnimationManager();
		bool isVisible() const override;
		void display() const override;
		void refresh();

	private:
		std::vector<unsigned int> m_cycles;
		std::vector<std::unique_ptr<SpriteAnimation>> m_sprites;
	};
}
#endif
