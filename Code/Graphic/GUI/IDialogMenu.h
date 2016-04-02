#pragma once
#include "../../ska/Graphic/Rectangle.h"

class IDialogMenu {
public:
	IDialogMenu() = default;
	virtual ~IDialogMenu() = default;

	virtual void hide(bool x) = 0;
	virtual bool isVisible() const = 0;
	virtual void display() = 0;
	virtual void refresh() = 0;
	virtual const ska::Rectangle getRect() const = 0;
};