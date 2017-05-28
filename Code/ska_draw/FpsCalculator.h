#pragma once
#include <array>
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
		double m_ticksum;
		std::array<int, MAXSAMPLES> m_ticklist;
		FpsRenderable m_renderable;
	};
}
