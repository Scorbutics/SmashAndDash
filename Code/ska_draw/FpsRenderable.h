#pragma once

#include "Draw/DrawableFixedPriority.h"
#include "Graphic/Texture.h"

namespace ska {
	class FpsRenderable :
	    public DrawableFixedPriority {
	public:
		FpsRenderable();
		~FpsRenderable() = default;

		void setVisible(bool v);
		void update(const double value);

		bool isVisible() const override;
		void display() const override;

	private:
		static const int DELAY = 200;
		Texture m_fps;
		unsigned int m_t0;
		bool m_toggleVisible;

	};
}
