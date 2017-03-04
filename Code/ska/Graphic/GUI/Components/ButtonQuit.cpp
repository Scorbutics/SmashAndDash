#include "ButtonQuit.h"

ska::ButtonQuit::ButtonQuit(Widget& parent, Point<int> relativePos, const std::string& placeHolderStyleName) :
Button(parent, relativePos, placeHolderStyleName, nullptr, [&](Widget*, const ClickEvent& e){
	parent.show(false);
}) {
}
