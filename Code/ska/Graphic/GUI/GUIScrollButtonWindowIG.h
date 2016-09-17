#include "TimeScrollableWindowIG.h"

namespace ska {
	class GUIScrollButtonWindowIG : public TimeScrollableWindowIG {
	public:
		GUIScrollButtonWindowIG(MouseObservable& guiObservable, const ska::Rectangle& box, bool drawStyle);
		GUIScrollButtonWindowIG(ska::Widget& parent, const ska::Rectangle& box, bool drawStyle);
		virtual bool scrollTo(const ska::Point<int>& targetPos, unsigned int speed);
		void scrollRewind();
		virtual ~GUIScrollButtonWindowIG();
	private:
		Point<int> m_realPos;

	};
}