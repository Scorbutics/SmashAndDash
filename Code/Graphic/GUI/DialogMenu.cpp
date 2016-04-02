#include <math.h>

#include "DialogMenu.h"
#include "../../Utils/ChargementImages.h"
#include "Button_Quit.h"
#include "Scroll_Text.h"
#include "Button.h"
#include "Button_Bar.h"
#include "Inventory_Area.h"
#include "../../Utils/IDs.h"
#include "../../ska/Utils/RectangleUtils.h"
#include "Text_Area.h"
#include "Image_Area.h"

#define RECT_OFFSET 12
#define F_IN 1
#define F_OUT 2
#define SCROLL_SPEED 6

using namespace std;

//Menu du style "statique"
//DWindow est le menu du style "dynamique"

///////////////////////////////////////////////
//Le menu est géré de même façon qu'un layer. on a une image qui permet la construction des fenêtres du menu (on peut considérer ça comme un chipset à menu),
//et on blit certaines parties de l'image à tel ou tel endroit de sorte à créer une fenêtre.
///////////////////////////////////////////////


DialogMenu::DialogMenu(string texte, string messImg, string fichierMenu, ska::Rectangle posFond, int taillePolice, bool scroll) {
	setMoving(false);
    m_show = false;
    m_alpha = false;
    m_alphaImg = false;
    m_sensScroll = F_IN;
    m_scroll = scroll;
    m_fontSize = taillePolice;
	m_rect = posFond;
	m_rect.w = (m_rect.w / TAILLEBLOCFENETRE) * TAILLEBLOCFENETRE;
	m_rect.h = (m_rect.h / TAILLEBLOCFENETRE) * TAILLEBLOCFENETRE;

    m_ligne = 0;

    m_posLFond.x = 0;
    m_posLFond.y = 0;
    m_posLFond.w = TAILLEBLOCFENETRE;
    m_posLFond.h = TAILLEBLOCFENETRE;

	m_posScrollFond.x = m_rect.w;
    m_posScrollFond.y = posFond.y;

	m_fond.load(fichierMenu, DEFAULT_T_RED, DEFAULT_T_GREEN, DEFAULT_T_BLUE);

	if (!messImg.empty()) {
		m_messImage.load(messImg, DEFAULT_T_RED, DEFAULT_T_GREEN, DEFAULT_T_BLUE);
	}

	m_rectMessImage.w = m_messImage.getWidth();
	m_rectMessImage.h = m_messImage.getHeight();
	m_rectMessImage.x = ska::RectangleUtils::posToCenterPicture(m_rectMessImage, m_rect).x;
	m_rectMessImage.y = ska::RectangleUtils::posToCenterPicture(m_rectMessImage, m_rect).y;

    m_couleur.r = 0;
    m_couleur.g = 0;
    m_couleur.b = 0;
	m_couleur.a = 255;

    m_posTexte.x = RECT_OFFSET + m_rect.x;
	m_posTexte.y = 2 * RECT_OFFSET + m_rect.y + m_rectMessImage.h;
    m_posTexte.h = m_rect.h;
    m_posTexte.w = m_fond.getWidth();

	modifyText(texte);
}

void DialogMenu::setMessImg(string img) {
	m_messImage.load(img, DEFAULT_T_RED, DEFAULT_T_GREEN, DEFAULT_T_BLUE);
}

int DialogMenu::getLines() {
    return m_size;
}

string DialogMenu::getText(unsigned int line) {
	if (line < m_texte.size()) {
		return m_texte[line];
	}
    return "";
}

void DialogMenu::pause() {

}

void DialogMenu::display() {
	

	for (int i = m_rect.x; i < (m_rect.x + m_rect.w); i += TAILLEBLOCFENETRE)
    {
		for (int j = m_rect.y; j < (m_rect.y + m_rect.h); j += TAILLEBLOCFENETRE)
        {

			m_positionFond.x = i - m_posScrollFond.x + m_rect.x; //m_positionFond est un buffer (Rectangle déclaré dans Window)
            m_positionFond.y = j;

			if (i == m_rect.x && j == m_rect.y) {
                m_posLFond.x = 0;
                m_posLFond.y = 0;
            } else if (i == (m_rect.x + m_rect.w) - TAILLEBLOCFENETRE && j == (m_rect.y + m_rect.h) - TAILLEBLOCFENETRE) {
                m_posLFond.x = 2*TAILLEBLOCFENETRE;
                m_posLFond.y = 2*TAILLEBLOCFENETRE;
            } else if (i == m_rect.x && j == (m_rect.y + m_rect.h) - TAILLEBLOCFENETRE) {
                m_posLFond.x = 0;
                m_posLFond.y = 2*TAILLEBLOCFENETRE;
            } else if (i == (m_rect.x + m_rect.w) - TAILLEBLOCFENETRE && j == m_rect.y) {
                m_posLFond.x = 2*TAILLEBLOCFENETRE;
                m_posLFond.y = 0;
            } else if (j == m_rect.y) {
                m_posLFond.x = TAILLEBLOCFENETRE;
                m_posLFond.y = 0;
            } else if (j == (m_rect.y + m_rect.h) - TAILLEBLOCFENETRE) {
                m_posLFond.x = TAILLEBLOCFENETRE;
                m_posLFond.y = 2*TAILLEBLOCFENETRE;
            } else if (i == m_rect.x) {
                m_posLFond.x = 0;
                m_posLFond.y = TAILLEBLOCFENETRE;
            } else if (i == (m_rect.x + m_rect.w) - TAILLEBLOCFENETRE) {
                m_posLFond.x = 2*TAILLEBLOCFENETRE;
                m_posLFond.y = TAILLEBLOCFENETRE;
            } else {
                m_posLFond.x = TAILLEBLOCFENETRE;
                m_posLFond.y = TAILLEBLOCFENETRE;
            }

			m_fond.render(m_positionFond.x, m_positionFond.y, &m_posLFond);
        }
    }

	m_posTexte.x = RECT_OFFSET + m_rect.x - m_posScrollFond.x + m_rect.x;
	m_posTexte.y = RECT_OFFSET + m_rect.y - m_posScrollFond.y + m_rect.y + m_fontSize / 2;
	m_rectMessImage.x = ska::RectangleUtils::posToCenterPicture(m_rectMessImage, m_rect).x - m_posScrollFond.x + m_rect.x;

	m_messImage.render(m_rectMessImage.x, m_rectMessImage.y);

	if (!m_stexte.empty() /*&& !m_x.empty()*/) {
		for (unsigned int i = 0; i <= m_ligne; i++) {
			ska::Rectangle buf;

			buf.x = 0;
			buf.y = 0;
			buf.w = 0;
			buf.h = m_stexte[0].getHeight();

			if (i == m_ligne) {
				if (m_x[m_ligne] < m_stexte[m_ligne].getWidth()) {
					m_x[m_ligne] += 9.5;
					buf.w += (int)m_x[m_ligne];
				} else {
					buf.w = m_stexte[m_ligne].getWidth();
					m_x[m_ligne] = (float)buf.w;
					if (m_ligne + 1 < m_texte.size()) {
						m_ligne++;
					}
				}
				m_stexte[i].render(m_posTexte.x, m_posTexte.y, &buf);
			} else {
				m_stexte[i].render(m_posTexte.x, m_posTexte.y);
			}

			m_posTexte.y += m_fontSize;
		}
	}
	m_posTexte.y = m_rect.h / 30 + m_rectMessImage.y + m_rectMessImage.h;

    if(m_sensScroll == F_IN) {
		//Scroll in
		if (m_posScrollFond.x - SCROLL_SPEED*TAILLEBLOCFENETRE / 2 >= m_rect.x && m_scroll == true) {
            m_posScrollFond.x -= SCROLL_SPEED*TAILLEBLOCFENETRE/2;
            m_show = true;
        } else {
			m_posScrollFond.x = m_rect.x;
		}
    } else {
		//Scroll out
		if (m_scroll == true)  {
			m_posScrollFond.x += SCROLL_SPEED*TAILLEBLOCFENETRE / 2;
		} else {
            m_posScrollFond.x = -1;
            m_show = false;
        }

    }

	for (unsigned int i = 0; i < m_areaList.size(); i++){
		m_areaList[i]->display();
	}

}

void DialogMenu::refresh() {
	for(unsigned int i = 0; i < m_areaList.size(); i++) {
		DynamicWindowArea* dynArea = dynamic_cast<DynamicWindowArea*>(&(*m_areaList[i]));
		if(dynArea != NULL)  {
			// &(*m_areaList[i]) was safely casted to DynamicWindowArea*
			dynArea->refresh();
		}
	}
}

void DialogMenu::setActionClic(string action) {
    m_actionClic = action;
}

string DialogMenu::getActionClic() {
    return m_actionClic;
}

int DialogMenu::getX(unsigned int ligne) {    
	return (ligne < m_size) ? (int)m_x[ligne] : -1;
}

void DialogMenu::deleteAll() {
	for (unsigned int i = 0; i < m_areaList.size(); i++) {
		m_areaList.pop_back();
	}
    m_areaList.clear();
}

int DialogMenu::getWLine(unsigned int ligne) {     
	 return (ligne < m_size) ? m_stexte[ligne].getWidth() : -1;
}

Inventory_Area* DialogMenu::getInventoryArea(unsigned int index) {
    unsigned int i;
    index++;
	for (i = 0; i < m_areaList.size() && index != 0; i++) {
		if ((m_areaList[i])->isA(BUTTON_INVENTORY_AREA)) {
			index--;
		}
	}
    
	return (index == 0) ? &(**(reinterpret_cast<unique_ptr<Inventory_Area>*> (&(m_areaList[i - 1])))) : NULL;    
}

void DialogMenu::modifyColor(SDL_Color col) {
    m_couleur = col;
	for (unsigned int i = 0; i < m_size; i++) {
		m_stexte[i].loadFromText(m_fontSize, m_texte[i], m_couleur);
	}
    
}


void DialogMenu::modifyText(string texte) {
    m_stexte.clear();
    m_texte.clear();
	m_x.clear();
	

    {
        unsigned int j = 0;
        for(unsigned int i = 0; i < texte.size(); i++) {
			
            if(i == 0 || texte[i] == '¤') {
                m_texte.push_back(string());
                m_stexte.push_back(ska::Texture());
                m_x.push_back(0);
                j++;
            }

			if (j - 1 < m_texte.size() && texte[i] != '¤') {
				m_texte[j - 1] += texte[i];
			}
			
        }
        m_size = j;

    }


	for (unsigned int j = 0; j < m_size; j++) {
		m_stexte[j].loadFromText(m_fontSize, m_texte[j], m_couleur);
		if ((m_stexte[j].getWidth() / TAILLEBLOCFENETRE + 1) * TAILLEBLOCFENETRE > m_rect.w) {
			m_rect.w = (m_stexte[j].getWidth() / TAILLEBLOCFENETRE + 1) * TAILLEBLOCFENETRE;
		}
	}
        
    

}

void DialogMenu::resize(int w, int h) {
	m_rect.w = (w / TAILLEBLOCFENETRE) * TAILLEBLOCFENETRE;
	m_rect.h = (h / TAILLEBLOCFENETRE) * TAILLEBLOCFENETRE;
}

bool DialogMenu::isVisible() {
    return m_show;
}

void DialogMenu::hide(bool x) {    
    m_sensScroll =  x? F_OUT : F_IN;
    
	if (!m_scroll) {
		m_show = !x;
		for (unsigned int j = 0; j < m_x.size(); j++) {
			m_x[j] = 0;
		}
	}

    m_ligne = 0;
}

const ska::Rectangle& DialogMenu::getRect() {
	return m_rect;
}

const ska::Point<int>& DialogMenu::getPos() {
	return m_rect;
}

const unsigned int DialogMenu::getWidth() {
	return m_rect.w;
}

const unsigned int DialogMenu::getHeight() {
	return m_rect.h;
}

const ska::Rectangle& DialogMenu::getPosImg() {
    return m_rectMessImage;
}

bool DialogMenu::getAlpha() {
    return m_alpha;
}

void DialogMenu::setPosImg(int x, int y) {
	m_rectMessImage.x = x;
	m_rectMessImage.y = y;
}

void DialogMenu::setPos(int x, int y) {
    //m_moving = true;
    m_rect.x = x;
	m_rect.y = y;
    //m_posScrollFond.x = x;
    m_posScrollFond.y = y;
	m_rectMessImage = ska::RectangleUtils::posToCenterPicture(m_rectMessImage, m_rect);
}

void DialogMenu::setAlpha(bool x) {
	m_fond.setAlpha(x ? 128:255);
    m_alpha = x;
}

bool DialogMenu::getAlphaImg() {
    return m_alphaImg;
}

void DialogMenu::addButtonClose(string imgName, string secondImgName, ska::Rectangle relativePos) {
	for (unsigned int i = 0; i < m_areaList.size(); i++) {
		if ((m_areaList[i])->isA(BUTTON_CLOSE)) {
			return;
		}
	}

    m_areaList.push_back(unique_ptr<Button_Quit>(new Button_Quit(this, imgName, secondImgName, relativePos)));
}

void DialogMenu::addTextArea(string text, int fontSize, ska::Rectangle relativePos) {
    m_areaList.push_back(unique_ptr<Text_Area>(new Text_Area(this, text, fontSize, relativePos)));
}

void DialogMenu::addImageArea(string name, bool alpha, ska::Rectangle relativePos, ska::Rectangle* rectSrc) {
    m_areaList.push_back(unique_ptr<Image_Area>(new Image_Area(this, relativePos, rectSrc, name, alpha)));
}

void DialogMenu::addImageArea(ska::Texture* tex, bool alpha, ska::Rectangle relativePos, ska::Rectangle* rectSrc) {
	m_areaList.push_back(unique_ptr<Image_Area>(new Image_Area(this, relativePos, rectSrc, tex, alpha)));
}

void DialogMenu::addScrollText(string buttonAspect, int height, int width, vector<string> text, int fontSize, ska::Rectangle relativePos) {
    m_areaList.push_back(unique_ptr<Scroll_Text>(new Scroll_Text(this, buttonAspect, height, width, text, fontSize, relativePos)));
}

void DialogMenu::addButton(ska::Rectangle relativePos, std::string styleName, std::string styleNamePressed, int* variable, vector<int> value, vector<string> displayedText, int fontSize, string key) {
    m_areaList.push_back(unique_ptr<Button>(new Button(this, relativePos, styleName, styleNamePressed, variable, value, displayedText, fontSize, key, false)));
}

void DialogMenu::addButtonBar(ska::Rectangle relativePos, std::string styleName, int* variable, std::vector<int> values, vector<string> displayedText, int fontSize, std::string key) {
    m_areaList.push_back(unique_ptr<Button_Bar>(new Button_Bar(this, relativePos, styleName, variable, values, displayedText, fontSize, key)));
}

Window_Area* DialogMenu::getButton(string key) {
    const size_t areaListSize = m_areaList.size();
	for (size_t i = 0; i < areaListSize; i++) {
		if (m_areaList[i]->getKey() == key) {
			return &(*(m_areaList[i]));
		}
	}
    cerr << "Erreur (classe DialogMenu) : Dépassement mémoire dans la liste des boutons associés à une fenêtre" << endl;
    return &(*(m_areaList[0]));
}

void DialogMenu::addInventory(Inventory& inv, ska::Rectangle relativePos) {
	ska::Rectangle buf = relativePos;
	buf.h = m_rect.h - relativePos.x;
	buf.w = m_rect.w - relativePos.y;
    m_areaList.push_back(unique_ptr<Inventory_Area>(new Inventory_Area(this, &inv, buf)));
}

Window_Area* DialogMenu::getCloseButton() {
    unsigned int i;
	for (i = 0; i < m_areaList.size(); i++) {
		if ((m_areaList[i])->isA(BUTTON_CLOSE)) {
			break;
		}
	}
    
	return (i < m_areaList.size()) ? &(*(m_areaList[i])) : NULL;
}

void DialogMenu::setAlphaImg(bool x) {
    m_alphaImg = x;
	m_messImage.setAlpha(x ? 128:255);
}

DialogMenu::~DialogMenu() {
    m_stexte.clear();
    m_texte.clear();
    //TTF_CloseFont(m_police);
}



bool DialogMenu::isMoving() {
    return m_moving;
}

void DialogMenu::setMoving(bool x) {
    m_moving = x;
}

void DialogMenu::setPos(ska::Point<int> pos) {
    //m_moving = true;
	m_rect.x = pos.x;
	m_rect.y = pos.y;
    m_posScrollFond.x = pos.x;
    m_posScrollFond.y = pos.y;
}

void DialogMenu::move(ska::Point<int> delta) {
    m_moving = true;
	m_rect.x += delta.x;
	m_rect.y += delta.y;
	m_posScrollFond.x = m_rect.x;
	m_posScrollFond.y = m_rect.y;
}
