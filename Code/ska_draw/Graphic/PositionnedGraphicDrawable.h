#pragma once
#include "GraphicComponent.h"
#include "ECS/Basics/Physic/PositionComponent.h"
#include "Draw/Drawable.h"

namespace ska {
	class PositionnedGraphicDrawable : public Drawable {

	public:
		PositionnedGraphicDrawable(AnimatedTexture& gc, int posx, int posy, int priority, int priority2D) :
			m_graphic(gc),
			m_priority(priority),
			m_priority2D(priority2D)  {
			m_positionx = posx;
			m_positiony = posy;
		}

		void operator=(const PositionnedGraphicDrawable&) = delete;

		virtual void display() const override {
			m_graphic.render(m_positionx, m_positiony);
		}

		virtual int getPriority() const override {
			return m_priority;
		}

		virtual int getPriority2D() const override {
			return m_priority2D;
		}

		virtual bool isVisible() const override {
			return true;
		}
		virtual ~PositionnedGraphicDrawable(){}

	private:
		AnimatedTexture& m_graphic;
		int m_positionx;
		int m_positiony;
		int m_priority;
		int m_priority2D;
	};
}
