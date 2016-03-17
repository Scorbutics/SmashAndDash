#pragma once
#include "GraphicComponent.h"
#include "../Physic/PositionComponent.h"
#include "Draw/Drawable.h"

namespace ska {
	class PositionnedGraphicDrawable : public Drawable {
	
	public:
		PositionnedGraphicDrawable(GraphicComponent& gc, int posx, int posy, int priority) : m_graphic(gc), m_priority(priority) { m_positionx = posx; m_positiony = posy; }
		virtual void display() override {
			m_graphic.sprite.render(m_positionx, m_positiony);
		}
		
		virtual int getPriority() const override {
			return m_priority;
		}

		virtual bool isVisible() const override {
			return true;
		}
		virtual ~PositionnedGraphicDrawable(){}

	private:
		GraphicComponent& m_graphic;
		int m_positionx;
		int m_positiony;
		int m_priority;
	};
}