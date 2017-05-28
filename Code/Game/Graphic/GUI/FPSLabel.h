#pragma once
#include "GUI/Windows/WindowIG.h"
#include "GUI/Events/TimeEventListener.h"
#include "FpsCalculator.h"

class FPSLabel : public ska::WindowIG<ska::TimeEventListener> {
public:
	explicit FPSLabel(ska::Widget& parent);
	virtual void display() const override;
	virtual ~FPSLabel() = default;

private:
	ska::FpsCalculator m_fpsCalculator;
	static unsigned int ACCU;
};
