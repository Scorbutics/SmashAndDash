#pragma once
#include "FpsRenderable.h"

namespace ska {
	class FpsCalculator {
	public:
		FpsCalculator();
		double calculate(long newtick);
		void setDisplayPriority(int p);
		const FpsRenderable& getRenderable() const;
		~FpsCalculator() = default;

	private:
		const static int MAXSAMPLES = 100;
		int m_tickindex;
		int m_ticksum;
		int m_ticklist[MAXSAMPLES];
		FpsRenderable m_renderable;
	};
}