#pragma once
#include "IDialogMenu.h"
#include "GUI/Windows/DynamicWindowIG.h"
#include "GUI/Utils/TimeObservable.h"
#include "GUI/Windows/TimeScrollableWindowIG.h"


namespace ska {
	class Widget;
	class MouseObservable;
}

class DialogMenu : public IDialogMenu,
    public ska::TimeScrollableWindowIG<ska::KeyEventListener> {
public:
	DialogMenu(ska::Widget& parent, const std::string& text, const std::string& name, const ska::Rectangle rect, const unsigned int fontSize, const bool scroll = true, const int timeout = -1);
	~DialogMenu() = default;

    const std::string& getName() const;

    void render(const ska::Renderer& renderer) const override;
	bool isVisible() const;
	virtual int getPriority() const override;

private:
	unsigned int m_fontSize;
	/* Defines a timeout to the displayed window (after that, it auto hides) */
	int m_timeout;
    const std::string m_name;
	bool m_scroll;

};

