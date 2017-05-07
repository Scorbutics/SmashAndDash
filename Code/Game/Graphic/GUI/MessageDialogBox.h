#pragma once
#include <string>
#include "DialogMenu.h"


namespace ska {
	class InputContextManager;
	class MouseObservable;
}

class MessageDialogBox :
    public IDialogMenu {

public:
	MessageDialogBox(ska::Widget& parent, ska::InputContextManager& icm, const std::string& text, const std::string& messImg, const unsigned int screenH, const unsigned int screenW, const int timeout = -1);
	void operator=(const MessageDialogBox&) = delete;

	void display() const override;
	bool isVisible() const override;
	int getPriority() const override;

	static bool instanceExists();

	~MessageDialogBox();
private:
	static int m_instanceExists;
	DialogMenu m_dialog;

	//TODO Image tête PNJ
	//DialogMenu m_talker;

	const ska::InputContextManager& m_playerICM;
};

