#include <iostream>
#include <string>
#include <SDL2/SDL.h>
#include <fstream>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <math.h>
#include <memory>

#include "DialogMenu.h"
#include "../../Utils/ChargementImages.h"
#include "../../Gameplay/WGameCore.h"
#include "Button_Quit.h"
#include "Scroll_Text.h"
#include "Button.h"
#include "Button_Bar.h"
#include "Inventory_Area.h"

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


DialogMenu::DialogMenu(string texte, string messImg, string fichierMenu, SDL_Rect posFond, int taillePolice, bool scroll)
{
	this->setMoving(false);
    m_show = false;
    m_alpha = false;
    m_alphaImg = false;
    m_sensScroll = F_IN;
    m_scroll = scroll;
    m_fontSize = taillePolice;
    m_posFond = posFond;
    m_posFond.w = (m_posFond.w/TAILLEBLOCFENETRE) * TAILLEBLOCFENETRE;
    m_posFond.h = (m_posFond.h/TAILLEBLOCFENETRE) * TAILLEBLOCFENETRE;

    m_ligne = 0;

    m_posLFond.x = 0;
    m_posLFond.y = 0;
    m_posLFond.w = TAILLEBLOCFENETRE;
    m_posLFond.h = TAILLEBLOCFENETRE;

    m_posScrollFond.x = m_posFond.w;
    m_posScrollFond.y = posFond.y;

    m_fond.load(fichierMenu, T_RED, T_GREEN, T_BLUE);

	if(messImg != "")
		m_messImage.load(messImg, T_RED, T_GREEN, T_BLUE);

	m_posMessImage.w = m_messImage.getWidth();
	m_posMessImage.h = m_messImage.getHeight();
    m_posMessImage.x = PosToCenterPicture(&m_posMessImage, &m_posFond).x ;
    m_posMessImage.y = PosToCenterPicture(&m_posMessImage, &m_posFond).y;

    m_couleur.r = 0;
    m_couleur.g = 0;
    m_couleur.b = 0;
	m_couleur.a = 255;

    m_posTexte.x = RECT_OFFSET + m_posFond.x;
    m_posTexte.y = 2*RECT_OFFSET + m_posFond.y + m_posMessImage.h;
    m_posTexte.h = m_posFond.h;
    m_posTexte.w = m_fond.getWidth();

	modifyText(texte);

    
}

void DialogMenu::setMessImg(string img)
{
	m_messImage.load(img, T_RED, T_GREEN, T_BLUE);
}

int DialogMenu::getLines()
{
    return m_size;
}

string DialogMenu::getText(unsigned int line)
{
    if(line < m_texte.size())
        return m_texte[line];
    return "";
}

void DialogMenu::pause()
{
    int continuer = 1;
	WGameCore& wScreen = WGameCore::getInstance();
	KeyInput* in = wScreen.getInputListener().getKeyInput();

	in->resetAll();
    while(continuer == 1)
    {
        wScreen.getInputListener().updateEvents();
        if(in->getKeyState(SDL_SCANCODE_SPACE) || in->getKeyState(SDL_SCANCODE_RETURN) || in->getKeyState(SDL_SCANCODE_ESCAPE))
            continuer = 0;

		SDL_Delay(30);
    }

    in->resetAll();
}

void DialogMenu::display()
{
	WGameCore& wScreen = WGameCore::getInstance();

    for(int i = m_posFond.x; i < (m_posFond.x+m_posFond.w); i+=TAILLEBLOCFENETRE)
    {
        for(int j = m_posFond.y; j < (m_posFond.y+m_posFond.h); j+=TAILLEBLOCFENETRE)
        {

            m_positionFond.x = i - m_posScrollFond.x + m_posFond.x; //m_positionFond est un buffer (SDL_Rect déclaré dans Window)
            m_positionFond.y = j;

            if(i == m_posFond.x && j == m_posFond.y)
            {
                m_posLFond.x = 0;
                m_posLFond.y = 0;
            }
            else if(i == (m_posFond.x+m_posFond.w)-TAILLEBLOCFENETRE && j == (m_posFond.y+m_posFond.h)-TAILLEBLOCFENETRE)
            {
                m_posLFond.x = 2*TAILLEBLOCFENETRE;
                m_posLFond.y = 2*TAILLEBLOCFENETRE;
            }
            else if(i == m_posFond.x && j == (m_posFond.y+m_posFond.h)-TAILLEBLOCFENETRE)
            {
                m_posLFond.x = 0;
                m_posLFond.y = 2*TAILLEBLOCFENETRE;
            }
            else if(i == (m_posFond.x+m_posFond.w)-TAILLEBLOCFENETRE && j == m_posFond.y)
            {
                m_posLFond.x = 2*TAILLEBLOCFENETRE;
                m_posLFond.y = 0;
            }
            else if(j == m_posFond.y)
            {
                m_posLFond.x = TAILLEBLOCFENETRE;
                m_posLFond.y = 0;
            }
            else if(j == (m_posFond.y+m_posFond.h)-TAILLEBLOCFENETRE)
            {
                m_posLFond.x = TAILLEBLOCFENETRE;
                m_posLFond.y = 2*TAILLEBLOCFENETRE;
            }
            else if(i == m_posFond.x)
            {
                m_posLFond.x = 0;
                m_posLFond.y = TAILLEBLOCFENETRE;
            }
            else if(i == (m_posFond.x+m_posFond.w)-TAILLEBLOCFENETRE)
            {
                m_posLFond.x = 2*TAILLEBLOCFENETRE;
                m_posLFond.y = TAILLEBLOCFENETRE;
            }
            else
            {
                m_posLFond.x = TAILLEBLOCFENETRE;
                m_posLFond.y = TAILLEBLOCFENETRE;
            }

			m_fond.render(m_positionFond.x, m_positionFond.y, &m_posLFond);
        }
    }

    m_posTexte.x = RECT_OFFSET + m_posFond.x - m_posScrollFond.x + m_posFond.x;
	m_posTexte.y = RECT_OFFSET + m_posFond.y - m_posScrollFond.y + m_posFond.y + m_fontSize / 2;
    m_posMessImage.x = PosToCenterPicture(&m_posMessImage, &m_posFond).x - m_posScrollFond.x + m_posFond.x;

	m_messImage.render(m_posMessImage.x, m_posMessImage.y);

    if(!m_stexte.empty() /*&& !m_x.empty()*/)
        for(unsigned int i = 0; i <= m_ligne; i++)
        {
            SDL_Rect buf;

            buf.x = 0;
            buf.y = 0;
            buf.w = 0;
            buf.h = m_stexte[0].getHeight();

            if(i == m_ligne)
            {
                if(m_x[m_ligne] < m_stexte[m_ligne].getWidth())
                {
                    m_x[m_ligne] += 9.5;
                    buf.w += (int)m_x[m_ligne];
                }
                else
                {

                    buf.w = m_stexte[m_ligne].getWidth();
                    m_x[m_ligne] = (float)buf.w;
                    if(m_ligne + 1 < m_texte.size())
                        m_ligne++;
                }
				
				m_stexte[i].render(m_posTexte.x, m_posTexte.y, &buf);

            }
            else
				m_stexte[i].render(m_posTexte.x, m_posTexte.y);
            

			m_posTexte.y += m_fontSize;

        }

    m_posTexte.y = m_posFond.h/30+ m_posMessImage.y + m_posMessImage.h;

    if(m_sensScroll == F_IN)
    {
        if(m_posScrollFond.x - SCROLL_SPEED*TAILLEBLOCFENETRE/2 >= m_posFond.x && m_scroll == true) //Scroll in
        {
            m_posScrollFond.x -= SCROLL_SPEED*TAILLEBLOCFENETRE/2;
            m_show = true;
        }
        else
            m_posScrollFond.x = m_posFond.x;
    }
    else
    {
        if(m_posScrollFond.x + SCROLL_SPEED*TAILLEBLOCFENETRE/2 <= (int)wScreen.getWidth() && m_scroll == true) //Scroll out
            m_posScrollFond.x += SCROLL_SPEED*TAILLEBLOCFENETRE/2;
        else
        {
            m_posScrollFond.x = wScreen.getWidth();
            m_show = false;
        }

    }

    for(unsigned int i = 0; i < m_areaList.size(); i++)
        m_areaList[i]->display();

}

void DialogMenu::refresh()
{
	for(unsigned int i = 0; i < m_areaList.size(); i++)
	{
		DynamicWindowArea* dynArea = dynamic_cast<DynamicWindowArea*>(&(*m_areaList[i]));
		if(dynArea != NULL) 
		{
			// &(*m_areaList[i]) was safely casted to DynamicWindowArea*
			dynArea->refresh();
		}
	}
}

void DialogMenu::setActionClic(string action)
{
    m_actionClic = action;
}

string DialogMenu::getActionClic()
{
    return m_actionClic;
}

int DialogMenu::getX(unsigned int ligne)
{
    if(ligne < m_size)
        return (int)m_x[ligne];
    else
        return -1;
}

void DialogMenu::deleteAll()
{
    for(unsigned int i = 0; i < m_areaList.size(); i++)
        m_areaList.pop_back();
    m_areaList.clear();
}

int DialogMenu::getWLine(unsigned int ligne)
{
     if(ligne < m_size)
        return m_stexte[ligne].getWidth();
     else
        return -1;
}

Inventory_Area* DialogMenu::getInventoryArea(unsigned int index)
{
    unsigned int i;
    index++;
    for(i = 0; i < m_areaList.size() && index != 0; i++)
        if((m_areaList[i])->isA(BUTTON_INVENTORY_AREA))
            index--;

    if(index == 0)
        return &(**(reinterpret_cast<unique_ptr<Inventory_Area>*> (&(m_areaList[i-1]))));
    else
        return NULL;
}

void DialogMenu::modifyColor(SDL_Color col)
{
    m_couleur = col;
    for(unsigned int i = 0; i < m_size; i++)
		m_stexte[i].loadFromText(m_fontSize, m_texte[i], m_couleur);
    
}


void DialogMenu::modifyText(string texte)
{
    m_stexte.clear();
    m_texte.clear();
	m_x.clear();
	

    {
        int j = 0;
        for(unsigned int i = 0; i < texte.size(); i++)
        {
			

            if(i == 0 || texte[i] == '¤')
            {
                m_texte.push_back(string());
                m_stexte.push_back(Texture());
                m_x.push_back(0);
                j++;
            }

			if (j-1 < m_texte.size() && texte[i] != '¤')
				m_texte[j-1] += texte[i];
			

        }
        m_size = j;


    }


	for (unsigned int j = 0; j < m_size; j++)
	{
		m_stexte[j].loadFromText(m_fontSize, m_texte[j], m_couleur);
		if ((m_stexte[j].getWidth() / TAILLEBLOCFENETRE + 1) * TAILLEBLOCFENETRE > m_posFond.w)
			m_posFond.w = (m_stexte[j].getWidth() / TAILLEBLOCFENETRE + 1) * TAILLEBLOCFENETRE;
	}
        
    

}

void DialogMenu::resize(int w, int h)
{
    m_posFond.w = (w/TAILLEBLOCFENETRE) * TAILLEBLOCFENETRE;
    m_posFond.h = (h/TAILLEBLOCFENETRE) * TAILLEBLOCFENETRE;
}

bool DialogMenu::isVisible()
{
    return m_show;
}

void DialogMenu::hide(bool x)
{
    if(x)
        m_sensScroll = F_OUT;
    else
        m_sensScroll = F_IN;

	if (m_scroll == false)
	{
		m_show = !x;
		for (unsigned int j = 0; j < m_x.size(); j++)
			m_x[j] = 0;
	}

    m_ligne = 0;
}

const SDL_Rect* DialogMenu::getPos()
{
    return &m_posFond;
}

const SDL_Rect* DialogMenu::getPosImg()
{
    return &m_posMessImage;
}

bool DialogMenu::getAlpha()
{
    return m_alpha;
}

void DialogMenu::setPosImg(int x, int y)
{
    m_posMessImage.x = x;
    m_posMessImage.y = y;
}

void DialogMenu::setPos(int x, int y)
{
    //m_moving = true;
    m_posFond.x = x;
    m_posFond.y = y;
    //m_posScrollFond.x = x;
    m_posScrollFond.y = y;
    m_posMessImage = PosToCenterPicture(&m_posMessImage, &m_posFond);
}

void DialogMenu::setAlpha(bool x)
{
	m_fond.setAlpha(x ? 128:255);
    m_alpha = x;
}

bool DialogMenu::getAlphaImg()
{
    return m_alphaImg;
}

void DialogMenu::addButtonClose(string imgName, string secondImgName, SDL_Rect relativePos)
{
    for(unsigned int i = 0; i < m_areaList.size(); i++)
        if((m_areaList[i])->isA(BUTTON_CLOSE))
            return;

    m_areaList.push_back(unique_ptr<Button_Quit>(new Button_Quit(this, imgName, secondImgName, relativePos)));
}

void DialogMenu::addTextArea(string text, int fontSize, SDL_Rect relativePos)
{
    m_areaList.push_back(unique_ptr<Text_Area>(new Text_Area(this, text, fontSize, relativePos)));
}

void DialogMenu::addImageArea(string name, bool alpha, SDL_Rect relativePos, SDL_Rect* rectSrc)
{
    m_areaList.push_back(unique_ptr<Image_Area>(new Image_Area(this, relativePos, rectSrc, name, alpha)));
}

void DialogMenu::addImageArea(Texture* tex, bool alpha, SDL_Rect relativePos, SDL_Rect* rectSrc)
{
	m_areaList.push_back(unique_ptr<Image_Area>(new Image_Area(this, relativePos, rectSrc, tex, alpha)));
}

void DialogMenu::addScrollText(string buttonAspect, int height, int width, vector<string> text, int fontSize, SDL_Rect relativePos)
{
    m_areaList.push_back(unique_ptr<Scroll_Text>(new Scroll_Text(this, buttonAspect, height, width, text, fontSize, relativePos)));
}

void DialogMenu::addButton(SDL_Rect relativePos, std::string styleName, std::string styleNamePressed, int* variable, vector<int> value, vector<string> displayedText, int fontSize, string key)
{
    m_areaList.push_back(unique_ptr<Button>(new Button(this, relativePos, styleName, styleNamePressed, variable, value, displayedText, fontSize, key, false)));
}

void DialogMenu::addButtonBar(SDL_Rect relativePos, std::string styleName, int* variable, std::vector<int> values, vector<string> displayedText, int fontSize, std::string key)
{
    m_areaList.push_back(unique_ptr<Button_Bar>(new Button_Bar(this, relativePos, styleName, variable, values, displayedText, fontSize, key)));
}

Window_Area* DialogMenu::getButton(string key)
{
    const size_t areaListSize = m_areaList.size();
    for(size_t i = 0; i < areaListSize; i++)
        if(m_areaList[i]->getKey() == key)
            return &(*(m_areaList[i]));
    cerr << "Erreur (classe DialogMenu) : Dépassement mémoire dans la liste des boutons associés à une fenêtre" << endl;
    return &(*(m_areaList[0]));
}

void DialogMenu::addInventory(Inventory& inv, SDL_Rect relativePos)
{
    SDL_Rect buf = relativePos;
    buf.h = m_posFond.h - relativePos.x;
    buf.w = m_posFond.w - relativePos.y;
    m_areaList.push_back(unique_ptr<Inventory_Area>(new Inventory_Area(this, &inv, buf)));
}

Window_Area* DialogMenu::getCloseButton()
{
    unsigned int i;
    for(i = 0; i < m_areaList.size(); i++)
        if((m_areaList[i])->isA(BUTTON_CLOSE))
            break;

    if(i < m_areaList.size())
        return &(*(m_areaList[i]));
    else
        return NULL;
}

void DialogMenu::setAlphaImg(bool x)
{
    m_alphaImg = x;
	m_messImage.setAlpha(x ? 128:255);
}

DialogMenu::~DialogMenu()
{

    m_stexte.clear();
    m_texte.clear();
    //TTF_CloseFont(m_police);
}



bool DialogMenu::isMoving()
{
    return m_moving;
}

void DialogMenu::setMoving(bool x)
{
    m_moving = x;
}

void DialogMenu::setPos(SDL_Rect pos)
{
    //m_moving = true;
    m_posFond.x = pos.x;
    m_posFond.y = pos.y;
    m_posScrollFond.x = pos.x;
    m_posScrollFond.y = pos.y;
}

void DialogMenu::move(SDL_Rect delta)
{
    m_moving = true;
    m_posFond.x += delta.x;
    m_posFond.y += delta.y;
    m_posScrollFond.x = m_posFond.x;
    m_posScrollFond.y = m_posFond.y;
}
