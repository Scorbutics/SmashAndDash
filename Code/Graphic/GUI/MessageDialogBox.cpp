#include "MessageDialogBox.h"
#include "../../ska/Inputs/InputContextManager.h"
#include "../../ska/Graphic/Rectangle.h"
#include "../../Utils/IDs.h"

int MessageDialogBox::m_instanceExists = 0;

MessageDialogBox::MessageDialogBox(ska::TimeObservable& timeObs, ska::MouseObservable& gui, ska::KeyObservable& keyboardObs, ska::InputContextManager& icm, const std::string& text, const std::string& imageResource, const unsigned int screenH, const unsigned int screenW, const int timeout) :
m_dialog(timeObs, gui, keyboardObs, text, { 0, screenH - TAILLEBLOCFENETRE * 4, screenW / 2, TAILLEBLOCFENETRE * 4 }, timeout),
m_talker(timeObs, gui, keyboardObs, "", imageResource, { TAILLEBLOCFENETRE, screenH - TAILLEBLOCFENETRE * 9, 4 * TAILLEBLOCFENETRE, 5 * TAILLEBLOCFENETRE }, timeout),
m_playerICM(icm) {
	m_instanceExists++;
}

void MessageDialogBox::display() const {
	m_talker.display();
	m_dialog.display();
}

void MessageDialogBox::refresh() {
	const ska::InputActionContainer& in = m_playerICM.getActions();

	if ((m_talker.isVisible(true) || m_dialog.isVisible(true)) && (in[ska::InputAction::DoAction] || in[ska::InputAction::Quit])) {
		m_talker.hide(true);
		m_dialog.hide(true);
	}

	m_talker.refresh();
	m_dialog.refresh();
}

int MessageDialogBox::getPriority() const {
	return GUI_DEFAULT_DISPLAY_PRIORITY;
}

bool MessageDialogBox::isVisible() const {
	return m_dialog.isVisible() || m_talker.isVisible();
}

// const ska::Rectangle MessageDialogBox::getRect() const {
// 	return m_dialog.getRect();
// }

void MessageDialogBox::hide(bool x) {
	m_dialog.hide(x);
	m_talker.hide(x);
}

bool MessageDialogBox::instanceExists() {
	return m_instanceExists > 0;
}

MessageDialogBox::~MessageDialogBox() {
	m_instanceExists--;
}
