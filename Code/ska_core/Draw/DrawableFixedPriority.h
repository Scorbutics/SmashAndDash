#pragma once
#include "Drawable.h"
#include "../Core/Priorized.h"

namespace ska {
	class DrawableFixedPriority :
        virtual public Drawable,
        public Priorized {
	public:
		DrawableFixedPriority() = default;

		virtual int getPriority() const override {
            return Priorized::getPriority();
		}

		virtual ~DrawableFixedPriority() = default;

	};
}
