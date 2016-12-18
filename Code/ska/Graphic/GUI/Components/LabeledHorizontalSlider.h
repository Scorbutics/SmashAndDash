#pragma once

#include "HorizontalSlider.h"
#include "Label.h"

namespace ska {

	class LabeledHorizontalSlider : public HorizontalSlider {
	public:
		LabeledHorizontalSlider(Widget& parent, const std::string& styleName, Point<int> relativePos, const unsigned int pixelWidth);
		virtual ~LabeledHorizontalSlider() = default;
	private:
		Label* m_label;
	};

}