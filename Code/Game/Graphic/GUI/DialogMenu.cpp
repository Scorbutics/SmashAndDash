#include "DialogMenu.h"
#include "GUI/Components/Concrete/Button.h"
#include "GUI/Components/Concrete/ScrollingMultiLineLabel.h"
#include "../../Utils/IDs.h"
#include "Utils/TimeUtils.h"

#define RECT_OFFSET 12
#define F_IN 1
#define F_OUT 2
#define SCROLL_SPEED 3

///////////////////////////////////////////////
//Le menu est géré de même façon qu'un layer. on a une image qui permet la construction des fenêtres du menu (on peut considérer ça comme un chipset à menu),
//et on blit certaines parties de l'image à tel ou tel endroit de sorte à créer une fenêtre.
///////////////////////////////////////////////
DialogMenu::DialogMenu(ska::TimeObservable& timeObs, ska::MouseObservable& mouseObs, ska::KeyObservable& keyObs, const std::string& text, const std::string&, const std::string&, const ska::Rectangle rect, const unsigned int fontSize, const bool scroll, const int timeout) :
TimeScrollableWindowIG<ska::KeyEventListener>(&timeObs, &mouseObs, &keyObs, rect, ska::Button::MENU_DEFAULT_THEME_PATH + "menu"),
m_fontSize(fontSize),
m_ligne(0),
m_sensScroll(F_OUT),
m_timeout(timeout),
m_t0(0),
m_show(false),
m_scroll(scroll),
m_alpha(false),
m_moving(false),
m_isScrolling(false) {
	m_scrollingRect = getBox();
	m_scrollingRect.y += m_scrollingRect.h;

	hide(false);

	addWidget(std::make_unique<ska::ScrollingMultiLineLabel>(*this, text, 11, 1.F, ska::Point<int>(16, 16)));

	/*if (!imageResource.empty()) {
		m_image.load(imageResource);
	}

	m_rectImage = ska::RectangleUtils::posToCenterPicture(m_rectImage, m_scrollingRect);
	m_rectImage.w = m_image.getWidth();
	m_rectImage.h = m_image.getHeight();*/

	//modifyText(text);
}

DialogMenu::DialogMenu(ska::TimeObservable& timeObs, ska::MouseObservable& gui, ska::KeyObservable& keyboardObs) :
DialogMenu(timeObs, gui, keyboardObs, "", "", { 0, 0, 0, 0 }) {

}

DialogMenu::DialogMenu(ska::TimeObservable& timeObs, ska::MouseObservable& gui, ska::KeyObservable& keyboardObs, const std::string& text, const ska::Rectangle rect, const int timeout, const bool scroll) :
DialogMenu(timeObs, gui, keyboardObs, text, "", ska::Button::MENU_DEFAULT_THEME_PATH + "menu.png", rect, 22, scroll, timeout) {

}

DialogMenu::DialogMenu(ska::TimeObservable& timeObs, ska::MouseObservable& gui, ska::KeyObservable& keyboardObs, const std::string& text, const std::string& imageResource, const ska::Rectangle rect, const int timeout, const bool scroll) :
DialogMenu(timeObs, gui, keyboardObs, text, imageResource, ska::Button::MENU_DEFAULT_THEME_PATH + "menu.png", rect, 22, scroll, timeout) {

}

DialogMenu::DialogMenu(ska::TimeObservable& timeObs, ska::MouseObservable& mouseObs, ska::KeyObservable& keyObs, const DialogMenu& dm) :
TimeScrollableWindowIG<ska::KeyEventListener>(&timeObs, &mouseObs, &keyObs, dm.getBox(), ska::Button::MENU_DEFAULT_THEME_PATH + "menu") {
	operator=(dm);
}

int DialogMenu::getPriority() const {
	return GUI_DEFAULT_DISPLAY_PRIORITY;
}

void DialogMenu::display() const {
	if(!isVisible()) {
		return;
	}


	TimeScrollableWindowIG<ska::KeyEventListener>::display();

	/* Draw the associated image */
	//m_image.render(m_rectImage.x, m_rectImage.y);

	/* Draw the text */
	if (!m_stext.empty()) {
		ska::Point<int> textPos;
		textPos.x = RECT_OFFSET + m_scrollingRect.x;
		textPos.y = RECT_OFFSET + m_scrollingRect.y + m_fontSize / 2;

		for (unsigned int i = 0; i <= m_ligne; i++) {
			ska::Rectangle textClip = { 0, 0, 0, static_cast<int>(m_stext[0].getHeight()) };

			if (i == m_ligne) {
				if (m_scrollTextLengthPerLine[m_ligne] < m_stext[m_ligne].getWidth()) {
					textClip.w += static_cast<int>(m_scrollTextLengthPerLine[m_ligne]);
				} else {
					textClip.w = m_stext[m_ligne].getWidth();
				}
				m_stext[i].render(textPos.x, textPos.y, &textClip);
			} else {
				m_stext[i].render(textPos.x, textPos.y);
			}

			textPos.y += m_fontSize;
		}
	}


}

void DialogMenu::operator=(const DialogMenu& dm) {
	/* Do not copy Window Areas and guiObservable */
	//m_clickHandler = dm.m_clickHandler;
	m_alpha = dm.m_alpha;
	//m_color = dm.m_color;
	m_fontSize = dm.m_fontSize;
	//m_image = dm.m_image;
	m_isScrolling = dm.m_isScrolling;
	m_ligne = dm.m_ligne;
	//m_menuTiles = dm.m_menuTiles;
	m_moving = dm.m_moving;
	//m_rect = dm.m_rect;
	//m_rectImage = dm.m_rectImage;
	m_scroll = dm.m_scroll;
	m_scrollingRect = dm.m_scrollingRect;
	m_scrollTextLengthPerLine = dm.m_scrollTextLengthPerLine;
	m_sensScroll = dm.m_sensScroll;
	m_show = dm.m_show;
	m_size = dm.m_size;
	m_t0 = dm.m_t0;
	m_text = dm.m_text;
	//m_textImage = dm.m_textImage;
	m_timeout = dm.m_timeout;

}

/*
void DialogMenu::click(const ska::Point<int>& clickPos) {
	if (m_clickHandler != nullptr) {
		m_clickHandler();
	}
}*/

// const std::string& DialogMenu::getName() const {
// 	return m_name;
// }

bool DialogMenu::refresh() {
	if (isVisible(true)) {
		if (m_timeout != -1 && (ska::TimeUtils::getTicks() - m_t0) >= static_cast<unsigned int>(m_timeout)) {
			hide(true);
		}
	}

	/* Re-center image */
	//m_rectImage = ska::RectangleUtils::posToCenterPicture(m_rectImage, m_scrollingRect);

	/* Text scrolling */
	if (!m_stext.empty()) {
		ska::Rectangle textClip = { 0, 0, 0, static_cast<int>(m_stext[0].getHeight()) };

		if (m_scrollTextLengthPerLine[m_ligne] < m_stext[m_ligne].getWidth()) {
			m_scrollTextLengthPerLine[m_ligne] += 9.5;
			//textClip.w += static_cast<int>(m_scrollTextLengthPerLine[m_ligne]);
		} else {
			textClip.w = m_stext[m_ligne].getWidth();
			m_scrollTextLengthPerLine[m_ligne] = static_cast<float>(textClip.w);
			if (m_ligne + 1 < m_text.size()) {
				m_ligne++;
			}
		}
	}

	const auto& box = getBox();

	/* Window scrolling */
	if (m_scroll) {
		if (m_sensScroll == F_IN) {
			//Scroll in
			if (m_scrollingRect.y - SCROLL_SPEED*TAILLEBLOCFENETRE / 2 >= box.y) {
				m_scrollingRect.y -= SCROLL_SPEED*TAILLEBLOCFENETRE / 2;
			}
			else {
				m_scrollingRect.y = box.y;

				m_show = true;
				m_isScrolling = false;
			}
		} else {
			//Scroll out
			if ((m_scrollingRect.y + SCROLL_SPEED * TAILLEBLOCFENETRE / 2) <= box.y + box.h)  {
				m_scrollingRect.y += SCROLL_SPEED*TAILLEBLOCFENETRE / 2;
			}
			else {
				m_scrollingRect.y = box.y + box.h;
				//hide(true);
				m_show = false;
				m_isScrolling = false;
			}

		}
	}

	//TODO determiner les evenements a dispatcher aux widgets contenus

// 	for(auto& area : m_widgets) {
// 		/* TODO : ... AHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH */
// 		DynamicWindowArea* dynArea = dynamic_cast<DynamicWindowArea*>(area.get());
// 		if(dynArea != NULL)  {
// 			dynArea->refresh();
// 		}
// 	}

// 	if (m_closeButton != nullptr) {
// 		m_closeButton->refresh();
// 	}
	return true;
}

// void DialogMenu::setClickHandler(std::function<void(void)> const& action) {
//     m_clickHandler = action;
// }

// void DialogMenu::clear() {
// 	m_widgets.clear();
// }

// Inventory_Area* DialogMenu::getInventoryArea(unsigned int index) {
//     /*unsigned int i;
//     index++;
// 	for (i = 0; i < m_areaList.size() && index != 0; i++) {
// 		/* TODO : ... AHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH */
// 	/*	if ((m_areaList[i])->isA(BUTTON_INVENTORY_AREA)) {
// 			index--;
// 		}
// 	}
//
// 	return (index == 0) ? &(**(reinterpret_cast<std::unique_ptr<Inventory_Area>*> (&(m_areaList[i - 1])))) : NULL;    */
// 	return nullptr;
// }

void DialogMenu::modifyColor(ska::Color col) {
    m_color = col;
	for (unsigned int i = 0; i < m_size; i++) {
		m_stext[i].loadFromText(m_fontSize, m_text[i], m_color);
	}

}

// void DialogMenu::name(const std::string& name) {
// 	m_name = name;
// }

void DialogMenu::modifyText(const std::string& texte) {
    m_stext.clear();
	m_text.clear();
	m_scrollTextLengthPerLine.clear();


    {
        unsigned int j = 0;
        for(unsigned int i = 0; i < texte.size(); i++) {

            if(i == 0 || texte[i] == '¤') {
                m_text.push_back("");
                m_stext.push_back(ska::Texture());
				m_scrollTextLengthPerLine.push_back(0);
                j++;
            }

			if (j - 1 < m_text.size() && texte[i] != '¤') {
				m_text[j - 1] += texte[i];
			}

        }
        m_size = j;

    }

	const auto& box = getBox();

	for (unsigned int j = 0; j < m_size; j++) {
		m_stext[j].loadFromText(m_fontSize, m_text[j], m_color);
		if ((m_stext[j].getWidth() / TAILLEBLOCFENETRE + 1) * TAILLEBLOCFENETRE > static_cast<unsigned int>(box.w)) {
			setWidth((m_stext[j].getWidth() / TAILLEBLOCFENETRE + 1) * TAILLEBLOCFENETRE);
		}
	}



}

void DialogMenu::resize(int w, int h) {
	setWidth((w / TAILLEBLOCFENETRE) * TAILLEBLOCFENETRE);
	setHeight((h / TAILLEBLOCFENETRE) * TAILLEBLOCFENETRE);
}

bool DialogMenu::isVisible(bool noScrolling) const {
	return m_show || (m_scroll && (!noScrolling && m_isScrolling));
}

bool DialogMenu::isVisible() const {
	return isVisible(false);
}

void DialogMenu::hide(bool hide) {

	if (!m_scroll) {
		m_show = !hide;
		fill(m_scrollTextLengthPerLine.begin(), m_scrollTextLengthPerLine.end(), 0.0F);
	} else {
		m_isScrolling = true;
	}

	m_sensScroll = hide ? F_OUT : F_IN;

	if (!hide) {
		m_t0 = ska::TimeUtils::getTicks();
	}

    m_ligne = 0;
}

// const ska::Rectangle DialogMenu::getRect() const {
// 	return m_rect;
// }

void DialogMenu::setAlpha(bool) {
	/*m_menuTiles.setAlpha(x ? 128:255);
    m_alpha = x;*/
}

// void DialogMenu::setButtonClose(std::unique_ptr<ska::ButtonQuit>& button) {
// 	m_closeButton = std::move(button);
// }

// void DialogMenu::addTextArea(const std::string& text, int fontSize, ska::Point<int> relativePos) {
// 	m_widgets.push_back(std::unique_ptr<ska::Label>(new ska::Label(*this, text, fontSize, relativePos)));
// }
//
// void DialogMenu::addImageArea(const std::string& name, bool alpha, ska::Point<int> relativePos, ska::Rectangle* rectSrc) {
// 	m_widgets.push_back(std::unique_ptr<ska::Image>(new ska::Image(*this, name, relativePos, alpha)));
// }
//
// void DialogMenu::addScrollText(const std::string& buttonAspect, int height, int width, const std::vector<std::string>& text, int fontSize, ska::Rectangle relativePos) {
// 	//m_areaList.push_back(std::unique_ptr<Scroll_Text>(new Scroll_Text(m_playerICM, *this, buttonAspect, height, width, text, fontSize, relativePos)));
// }
//
// void DialogMenu::addButton(ska::Rectangle relativePos, const std::string& styleName, const std::string& styleNamePressed, int* variable, const std::vector<int>& value, const std::vector<std::string>& displayedText, int fontSize, const std::string& key) {
// 	//m_areaList.push_back(std::unique_ptr<Button>(new Button(*this, relativePos, styleName, styleNamePressed, variable, value, displayedText, fontSize, key, false)));
// }
//
// void DialogMenu::addButtonBar(ska::Rectangle relativePos, const std::string& styleName, int* variable, const std::vector<int>& values, const std::vector<std::string>& displayedText, int fontSize, const std::string& key) {
// 	//m_areaList.push_back(std::unique_ptr<Button_Bar>(new Button_Bar(*this, relativePos, styleName, variable, values, displayedText, fontSize, key)));
// }
//
// ska::Widget* DialogMenu::getButton(const std::string& key) {
// 	/* Si c'est pour faire ça, autant faire une unordered_map ....... */
// 	/*for (auto& area : m_areaList) {
// 		if (area->getKey() == key) {
// 			return area.get();
// 		}
// 	}*/
// 	std::cerr << "Erreur (classe DialogMenu) : Dépassement mémoire dans la liste des boutons associés à une fenêtre" << std::endl;
// 	return m_widgets[0].get();
// }

// void DialogMenu::addInventory(Inventory& inv, ska::Rectangle relativePos) {
// 	ska::Rectangle buf = relativePos;
// 	buf.h = m_rect.h - relativePos.x;
// 	buf.w = m_rect.w - relativePos.y;
// 	//m_widgets.push_back(std::unique_ptr<Inventory_Area>(new Inventory_Area(*this, &inv, buf)));
// }
//
// ska::ButtonQuit* DialogMenu::getCloseButton()
// {
// 	return m_closeButton.get();
// }

DialogMenu::~DialogMenu() {
    m_stext.clear();
    m_text.clear();
}

bool DialogMenu::isMoving() {
    return m_moving;
}

void DialogMenu::move(const ska::Point<int>& pos) {
	Widget::move(pos);
    m_scrollingRect.x = pos.x;
	m_scrollingRect.y = pos.y;
}

// void DialogMenu::addDynamicArea(std::unique_ptr<ska::Widget>&& area) {
// 	m_widgets.push_back(std::move(area));
// }

// void DialogMenu::move(ska::Point<int> delta) {
//     m_moving = true;
// 	m_rect.x += delta.x;
// 	m_rect.y += delta.y;
// 	m_scrollingRect.x = m_rect.x;
// 	m_scrollingRect.y = m_rect.y;
// }
