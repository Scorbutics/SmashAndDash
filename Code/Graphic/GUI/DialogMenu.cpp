#include "DialogMenu.h"
#include "../../Utils/ChargementImages.h"
#include "Button_Quit.h"
#include "Scroll_Text.h"
#include "Button.h"
#include "Button_Bar.h"
#include "Inventory_Area.h"
#include "../../Utils/IDs.h"
#include "../../ska/Utils/RectangleUtils.h"
#include "../../ska/Utils/TimeUtils.h"
#include "Text_Area.h"
#include "Image_Area.h"

#define RECT_OFFSET 12
#define F_IN 1
#define F_OUT 2
#define SCROLL_SPEED 3


///////////////////////////////////////////////
//Le menu est géré de même façon qu'un layer. on a une image qui permet la construction des fenêtres du menu (on peut considérer ça comme un chipset à menu),
//et on blit certaines parties de l'image à tel ou tel endroit de sorte à créer une fenêtre.
///////////////////////////////////////////////


DialogMenu::DialogMenu(const std::string& text, const std::string& imageResource, const std::string& menuResource, const ska::Rectangle rect, const unsigned int fontSize, const bool scroll, const int timeout) :
m_timeout(timeout),
m_moving(false),
m_show(false),
m_isScrolling(false), 
m_alpha(false),
m_sensScroll(F_OUT), 
m_scroll(scroll), 
m_fontSize(fontSize),
m_t0(0), 
m_ligne(0),
m_menuTiles(menuResource) {
	m_rect = rect;
	m_rect.w = (m_rect.w / TAILLEBLOCFENETRE) * TAILLEBLOCFENETRE;
	m_rect.h = (m_rect.h / TAILLEBLOCFENETRE) * TAILLEBLOCFENETRE;

	m_scrollingRect = m_rect;
	m_scrollingRect.y += m_rect.h;

	if (!imageResource.empty()) {
		m_image.load(imageResource);
	}

	m_rectImage = ska::RectangleUtils::posToCenterPicture(m_rectImage, m_rect);
	m_rectImage.w = m_image.getWidth();
	m_rectImage.h = m_image.getHeight();

	modifyText(text);
}

DialogMenu::DialogMenu() : 
DialogMenu("", "", { 0 }) {

}

DialogMenu::DialogMenu(const std::string& text, const ska::Rectangle rect, const int timeout, const bool scroll) : 
DialogMenu(text, "", "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"menu.png", rect, 22, scroll, timeout) {

}

DialogMenu::DialogMenu(const std::string& text, const std::string& imageResource, const ska::Rectangle rect, const int timeout, const bool scroll) : 
DialogMenu(text, imageResource, "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"menu.png", rect, 22, scroll, timeout) {

}

DialogMenu::DialogMenu(const DialogMenu& dm) {
	operator=(dm);
}

int DialogMenu::getPriority() const {
	return GUI_DEFAULT_DISPLAY_PRIORITY;
}

void DialogMenu::display() const {
	
	if (!isVisible()) {
		return;
	}

	ska::Rectangle backgroundTileClip = { 0, 0, TAILLEBLOCFENETRE, TAILLEBLOCFENETRE };
	ska::Point<int> backgroundTilePos;
	ska::Point<int> textPos;

	/* Draw the dialog using menu tiles */
	for (int i = 0; i < m_rect.w; i += TAILLEBLOCFENETRE) {
		for (int j = 0; j < m_rect.h; j += TAILLEBLOCFENETRE) {
			backgroundTilePos.x = i + m_scrollingRect.x;
			backgroundTilePos.y = j + m_scrollingRect.y;

			if (i == 0 && j == 0) {
				backgroundTileClip.x = 0;
				backgroundTileClip.y = 0;
            } else if (i == m_rect.w - TAILLEBLOCFENETRE && j == m_rect.h - TAILLEBLOCFENETRE) {
                backgroundTileClip.x = 2*TAILLEBLOCFENETRE;
                backgroundTileClip.y = 2*TAILLEBLOCFENETRE;
            } else if (i == 0 && j == m_rect.h - TAILLEBLOCFENETRE) {
                backgroundTileClip.x = 0;
                backgroundTileClip.y = 2*TAILLEBLOCFENETRE;
            } else if (i == m_rect.w - TAILLEBLOCFENETRE && j == 0) {
                backgroundTileClip.x = 2*TAILLEBLOCFENETRE;
                backgroundTileClip.y = 0;
            } else if (j == 0) {
                backgroundTileClip.x = TAILLEBLOCFENETRE;
                backgroundTileClip.y = 0;
            } else if (j == m_rect.h - TAILLEBLOCFENETRE) {
                backgroundTileClip.x = TAILLEBLOCFENETRE;
                backgroundTileClip.y = 2*TAILLEBLOCFENETRE;
            } else if (i == 0) {
                backgroundTileClip.x = 0;
                backgroundTileClip.y = TAILLEBLOCFENETRE;
            } else if (i == m_rect.w - TAILLEBLOCFENETRE) {
                backgroundTileClip.x = 2*TAILLEBLOCFENETRE;
                backgroundTileClip.y = TAILLEBLOCFENETRE;
            } else {
                backgroundTileClip.x = TAILLEBLOCFENETRE;
                backgroundTileClip.y = TAILLEBLOCFENETRE;
            }

			m_menuTiles.render(backgroundTilePos.x, backgroundTilePos.y, &backgroundTileClip);
        }
    }

	/* Draw the associated image */
	m_image.render(m_rectImage.x, m_rectImage.y);
	
	/* Draw the text */
	if (!m_textImage.empty()) {
		textPos.x = RECT_OFFSET + m_scrollingRect.x;
		textPos.y = RECT_OFFSET + m_scrollingRect.y + m_fontSize / 2;

		for (unsigned int i = 0; i <= m_ligne; i++) {
			ska::Rectangle textClip = { 0, 0, 0, m_textImage[0].getHeight() };

			if (i == m_ligne) {
				if (m_scrollTextLengthPerLine[m_ligne] < m_textImage[m_ligne].getWidth()) {
					textClip.w += (int)m_scrollTextLengthPerLine[m_ligne];
				} else {
					textClip.w = m_textImage[m_ligne].getWidth();
				}
				m_textImage[i].render(textPos.x, textPos.y, &textClip);
			} else {
				m_textImage[i].render(textPos.x, textPos.y);
			}

			textPos.y += m_fontSize;
		}
	}
	


	for (const auto& area : m_areaList){
		area->display();
	}

	if (m_closeButton != nullptr) {
		m_closeButton->display();
	}
}

void DialogMenu::operator=(const DialogMenu& dm) {
	/* Do not copy Window Areas */
	m_clickHandler = dm.m_clickHandler;
	m_alpha = dm.m_alpha;
	m_color = dm.m_color;
	m_fontSize = dm.m_fontSize;
	m_image = dm.m_image;
	m_isScrolling = dm.m_isScrolling;
	m_ligne = dm.m_ligne;
	m_menuTiles = dm.m_menuTiles;
	m_moving = dm.m_moving;
	m_rect = dm.m_rect;
	m_rectImage = dm.m_rectImage;
	m_scroll = dm.m_scroll;
	m_scrollingRect = dm.m_scrollingRect;
	m_scrollTextLengthPerLine = dm.m_scrollTextLengthPerLine;
	m_sensScroll = dm.m_sensScroll;
	m_show = dm.m_show;
	m_size = dm.m_size;
	m_t0 = dm.m_t0;
	m_text = dm.m_text;
	m_textImage = dm.m_textImage;
	m_timeout = dm.m_timeout;
}

void DialogMenu::click(const ska::Point<int>& clickPos) {
	if (m_clickHandler != nullptr) {
		m_clickHandler();
	}
}

const std::string& DialogMenu::getName() const {
	return m_name;
}

void DialogMenu::refresh() {
	if (isVisible(true)) {
		if (m_timeout != -1 && (ska::TimeUtils::getTicks() - m_t0) >= (unsigned int)m_timeout) {
			hide(true);
		}
	}

	/* Re-center image */
	m_rectImage = ska::RectangleUtils::posToCenterPicture(m_rectImage, m_scrollingRect);

	/* Text scrolling */
	if (!m_textImage.empty()) {		
		ska::Rectangle textClip = { 0, 0, 0, m_textImage[0].getHeight() };
	
		if (m_scrollTextLengthPerLine[m_ligne] < m_textImage[m_ligne].getWidth()) {
			m_scrollTextLengthPerLine[m_ligne] += 9.5;
			textClip.w += (int)m_scrollTextLengthPerLine[m_ligne];
		} else {
			textClip.w = m_textImage[m_ligne].getWidth();
			m_scrollTextLengthPerLine[m_ligne] = (float)textClip.w;
			if (m_ligne + 1 < m_text.size()) {
				m_ligne++;
			}
		}
	}

	/* Window scrolling */
	if (m_scroll) {
		if (m_sensScroll == F_IN) {
			//Scroll in
			if (m_scrollingRect.y - SCROLL_SPEED*TAILLEBLOCFENETRE / 2 >= m_rect.y) {
				m_scrollingRect.y -= SCROLL_SPEED*TAILLEBLOCFENETRE / 2;
			}
			else {
				m_scrollingRect.y = m_rect.y;

				m_show = true;
				m_isScrolling = false;
			}
		}
		else {
			//Scroll out
			if ((m_scrollingRect.y + SCROLL_SPEED * TAILLEBLOCFENETRE / 2) <= m_rect.y + m_rect.h)  {
				m_scrollingRect.y += SCROLL_SPEED*TAILLEBLOCFENETRE / 2;
			}
			else {
				m_scrollingRect.y = m_rect.y + m_rect.h;
				//hide(true);
				m_show = false;
				m_isScrolling = false;
			}

		}
	}

	for(auto& area : m_areaList) {
		/* TODO : ... AHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH */
		DynamicWindowArea* dynArea = dynamic_cast<DynamicWindowArea*>(area.get());
		if(dynArea != NULL)  {
			dynArea->refresh();
		}
	}

	if (m_closeButton != nullptr) {
		m_closeButton->refresh();
	}
}

void DialogMenu::setClickHandler(std::function<void(void)> const& action) {
    m_clickHandler = action;
}

void DialogMenu::deleteAll() {
    m_areaList.clear();
}

Inventory_Area* DialogMenu::getInventoryArea(unsigned int index) {
    /*unsigned int i;
    index++;
	for (i = 0; i < m_areaList.size() && index != 0; i++) {
		/* TODO : ... AHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH */
	/*	if ((m_areaList[i])->isA(BUTTON_INVENTORY_AREA)) {
			index--;
		}
	}
    
	return (index == 0) ? &(**(reinterpret_cast<std::unique_ptr<Inventory_Area>*> (&(m_areaList[i - 1])))) : NULL;    */
	return nullptr;
}

void DialogMenu::modifyColor(ska::Color col) {
    m_color = col;
	for (unsigned int i = 0; i < m_size; i++) {
		m_textImage[i].loadFromText(m_fontSize, m_text[i], m_color);
	}
    
}

void DialogMenu::name(const std::string& name) {
	m_name = name;
}

void DialogMenu::modifyText(const std::string& texte) {
    m_textImage.clear();
	m_text.clear();
	m_scrollTextLengthPerLine.clear();
	

    {
        unsigned int j = 0;
        for(unsigned int i = 0; i < texte.size(); i++) {
			
            if(i == 0 || texte[i] == '¤') {
                m_text.push_back("");
                m_textImage.push_back(ska::Texture());
				m_scrollTextLengthPerLine.push_back(0);
                j++;
            }

			if (j - 1 < m_text.size() && texte[i] != '¤') {
				m_text[j - 1] += texte[i];
			}
			
        }
        m_size = j;

    }


	for (unsigned int j = 0; j < m_size; j++) {
		m_textImage[j].loadFromText(m_fontSize, m_text[j], m_color);
		if ((m_textImage[j].getWidth() / TAILLEBLOCFENETRE + 1) * TAILLEBLOCFENETRE > m_rect.w) {
			m_rect.w = (m_textImage[j].getWidth() / TAILLEBLOCFENETRE + 1) * TAILLEBLOCFENETRE;
		}
	}
        
    

}

void DialogMenu::resize(int w, int h) {
	m_rect.w = (w / TAILLEBLOCFENETRE) * TAILLEBLOCFENETRE;
	m_rect.h = (h / TAILLEBLOCFENETRE) * TAILLEBLOCFENETRE;
}

bool DialogMenu::isVisible(bool noScrolling) const {
	return m_show || m_scroll && (!noScrolling && m_isScrolling);
}

bool DialogMenu::isVisible() const {
	return isVisible(false);
}

void DialogMenu::hide(bool hide) {   

	if (!m_scroll) {
		m_show = !hide;
		std::fill(m_scrollTextLengthPerLine.begin(), m_scrollTextLengthPerLine.end(), 0.0F);
	} else {
		m_isScrolling = true;
	}

	m_sensScroll = hide ? F_OUT : F_IN;

	if (!hide) {
		m_t0 = ska::TimeUtils::getTicks();
	}

    m_ligne = 0;
}

const ska::Rectangle DialogMenu::getRect() const {
	return m_rect;
}

void DialogMenu::setAlpha(bool x) {
	m_menuTiles.setAlpha(x ? 128:255);
    m_alpha = x;
}

void DialogMenu::setButtonClose(std::unique_ptr<Button_Quit>& button) {
	m_closeButton = std::move(button);
}

void DialogMenu::addTextArea(const std::string& text, int fontSize, ska::Point<int> relativePos) {
	m_areaList.push_back(std::unique_ptr<Text_Area>(new Text_Area(*this, text, fontSize, relativePos)));
}

void DialogMenu::addImageArea(const std::string& name, bool alpha, ska::Point<int> relativePos, ska::Rectangle* rectSrc) {
	m_areaList.push_back(std::unique_ptr<Image_Area>(new Image_Area(*this, relativePos, rectSrc, name, alpha)));
}

void DialogMenu::addImageArea(ska::Texture* tex, bool alpha, ska::Point<int> relativePos, ska::Rectangle* rectSrc) {
	m_areaList.push_back(std::unique_ptr<Image_Area>(new Image_Area(*this, relativePos, rectSrc, tex, alpha)));
}

void DialogMenu::addScrollText(const std::string& buttonAspect, int height, int width, const std::vector<std::string>& text, int fontSize, ska::Rectangle relativePos) {
	//m_areaList.push_back(std::unique_ptr<Scroll_Text>(new Scroll_Text(m_playerICM, *this, buttonAspect, height, width, text, fontSize, relativePos)));
}

void DialogMenu::addButton(ska::Rectangle relativePos, const std::string& styleName, const std::string& styleNamePressed, int* variable, const std::vector<int>& value, const std::vector<std::string>& displayedText, int fontSize, const std::string& key) {
	//m_areaList.push_back(std::unique_ptr<Button>(new Button(*this, relativePos, styleName, styleNamePressed, variable, value, displayedText, fontSize, key, false)));
}

void DialogMenu::addButtonBar(ska::Rectangle relativePos, const std::string& styleName, int* variable, const std::vector<int>& values, const std::vector<std::string>& displayedText, int fontSize, const std::string& key) {
	//m_areaList.push_back(std::unique_ptr<Button_Bar>(new Button_Bar(*this, relativePos, styleName, variable, values, displayedText, fontSize, key)));
}

Window_Area* DialogMenu::getButton(const std::string& key) {
	/* Si c'est pour faire ça, autant faire une unordered_map ....... */
	/*for (auto& area : m_areaList) {
		if (area->getKey() == key) {
			return area.get();
		}
	}*/
	std::cerr << "Erreur (classe DialogMenu) : Dépassement mémoire dans la liste des boutons associés à une fenêtre" << std::endl;
    return m_areaList[0].get();
}

void DialogMenu::addInventory(Inventory& inv, ska::Rectangle relativePos) {
	ska::Rectangle buf = relativePos;
	buf.h = m_rect.h - relativePos.x;
	buf.w = m_rect.w - relativePos.y;
	m_areaList.push_back(std::unique_ptr<Inventory_Area>(new Inventory_Area(*this, &inv, buf)));
}

Window_Area* DialogMenu::getCloseButton() {
	return m_closeButton.get();
}

DialogMenu::~DialogMenu() {
    m_textImage.clear();
    m_text.clear();
}

bool DialogMenu::isMoving() {
    return m_moving;
}

void DialogMenu::setPos(ska::Point<int> pos) {
    //m_moving = true;
	m_rect.x = pos.x;
	m_rect.y = pos.y;
    m_scrollingRect.x = pos.x;
	m_scrollingRect.y = pos.y;
}

void DialogMenu::addDynamicArea(DynamicWindowAreaPtr&& area) {
	m_areaList.push_back(std::move(area));
}

void DialogMenu::move(ska::Point<int> delta) {
    m_moving = true;
	m_rect.x += delta.x;
	m_rect.y += delta.y;
	m_scrollingRect.x = m_rect.x;
	m_scrollingRect.y = m_rect.y;
}
