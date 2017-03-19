#pragma once
#include "../../ska/Graphic/Rectangle.h"
#include "../../ska/Graphic/Draw/Drawable.h"

class IDialogMenu : virtual public ska::Drawable {
public:
	IDialogMenu() = default;
	virtual ~IDialogMenu() = default;

	virtual void hide(bool x) = 0;
	virtual bool refresh() = 0;
	//virtual const ska::Rectangle getBox() const = 0;
};