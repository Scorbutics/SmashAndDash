#include "FPSLabel.h"

unsigned int FPSLabel::ACCU = 0;

FPSLabel::FPSLabel(ska::Widget& parent) : ska::WindowIG<ska::TimeEventListener>(parent, ska::Rectangle{ 0 }, "") {
	addHandler<ska::TimeEventListener>([this](ska::Widget* tthis, ska::TimeEvent& te) {
		ACCU += te.getEllapsedTime();
		m_fpsCalculator.calculate(ACCU);
	});
}

void FPSLabel::display() const {
	WindowIG<ska::TimeEventListener>::display();	
	m_fpsCalculator.getRenderable().display();
	ACCU = 0;
}


