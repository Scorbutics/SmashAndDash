#pragma once
#include "Rectangle.h"
#include "Graphic/Draw/Drawable.h"

class IDialogMenu : virtual public ska::Drawable {
public:
	IDialogMenu() = default;
	virtual ~IDialogMenu() = default;

	virtual void hide(bool x) = 0;
	//virtual const ska::Rectangle getBox() const = 0;
};
