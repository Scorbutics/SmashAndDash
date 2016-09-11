#include "ButtonQuit.h"

ska::ButtonQuit::ButtonQuit(Widget& parent, ska::Point<int> relativePos, const std::string& placeHolderStyleName) :
Button(parent, relativePos, placeHolderStyleName, [&](const ska::ClickEvent& e){
	parent.show(false);
	return false;
}) {
}
