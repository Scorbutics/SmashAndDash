#pragma once

#include "HorizontalSlider.h"
#include "Label.h"

namespace ska {

	class LabeledHorizontalSlider : public HorizontalSlider {
	public:
		LabeledHorizontalSlider(Widget& parent, const std::string& styleName, Point<int> relativePos, const unsigned int pixelWidth);
		virtual void forceValue(float v) override;
		virtual ~LabeledHorizontalSlider() = default;
	private:
		int m_oldIntValue;
		Label& m_label;
	};

}
