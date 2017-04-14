#include "MessageDialogBox.h"
#include "Inputs/InputContextManager.h"
#include "Rectangle.h"
#include "../../Utils/IDs.h"

int MessageDialogBox::m_instanceExists = 0;

MessageDialogBox::MessageDialogBox(ska::Widget& parent, ska::InputContextManager& icm, const std::string& text, const std::string& imageResource, const unsigned int screenH, const unsigned int screenW, const int timeout) :
m_dialog(parent, text, "msgDialog", { 0, static_cast<int>(screenH - TAILLEBLOCFENETRE * 4), static_cast<int>(screenW / 2), TAILLEBLOCFENETRE * 4 }, timeout),
m_playerICM(icm) {
	m_instanceExists++;
}

void MessageDialogBox::display() const {
	m_dialog.display();
}

int MessageDialogBox::getPriority() const {
	return GUI_DEFAULT_DISPLAY_PRIORITY;
}

bool MessageDialogBox::isVisible() const {
	return m_dialog.isVisible();
}

bool MessageDialogBox::instanceExists() {
	return m_instanceExists > 0;
}

MessageDialogBox::~MessageDialogBox() {
	m_instanceExists--;
}
