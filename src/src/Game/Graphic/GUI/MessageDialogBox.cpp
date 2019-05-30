#include "Game/__internalConfig/LoggerConfig.h"
#include "MessageDialogBox.h"
#include "Core/Inputs/InputContextManager.h"
#include "Core/Rectangle.h"
#include "../../Utils/IDs.h"

int MessageDialogBox::m_instanceExists = 0;

MessageDialogBox::MessageDialogBox(ska::Widget& parent, ska::InputContextManager& icm, const std::string& text, const std::string&, const unsigned int screenH, const unsigned int screenW, const int timeout) :
m_dialog(parent, text, "msgDialog", { 0, static_cast<int>(screenH - 48 * 4), static_cast<int>(screenW / 2), 48 * 4 }, timeout),
m_playerICM(icm) {
	m_instanceExists++;
}

void MessageDialogBox::render(ska::Renderer& renderer) const{
	m_dialog.render(renderer);
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
