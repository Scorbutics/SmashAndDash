#include "FPSLabel.h"

unsigned int FPSLabel::ACCU = 0;

FPSLabel::FPSLabel(ska::Widget& parent) : ska::WindowIG<ska::TimeEventListener>(parent, ska::Rectangle{ 0 }, "") {
	addHandler<ska::TimeEventListener>([this](ska::Widget*, ska::TimeEvent& te) {
		ACCU += te.getEllapsedTime();
		m_fpsCalculator.calculate(ACCU);
	});
}

void FPSLabel::render(const ska::Renderer& renderer) const {
	WindowIG<ska::TimeEventListener>::render(renderer);
	m_fpsCalculator.getRenderable().render(renderer);
	ACCU = 0;
}


