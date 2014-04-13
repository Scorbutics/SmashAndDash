#include "Button.h"
#include "WGameCore.h"

using namespace std;

Button::Button(DialogMenu* parent, SDL_Rect relativePos, std::string styleName, std::string styleNamePressed, int* variable, std::vector<int> values, std::vector<std::string> displayedText, int fontSize, std::string key, bool bstatic) : DynamicWindowArea(parent)
{
    m_static = bstatic;
    m_index = 0;
    m_buttonStyle.load(styleName, T_RED, T_GREEN, T_BLUE);
    m_buttonStylePressed.load(styleNamePressed, T_RED, T_GREEN, T_BLUE);
    m_fontSize = fontSize;
    m_displayedText = displayedText;

    m_font = TTF_OpenFont(FILE_FONT, m_fontSize);
    if(m_font == NULL)
        cerr << "Erreur lors de l'ouverture de la police : " << TTF_GetError() << endl;


    m_relativePos = relativePos;
    m_relativePos.w = m_buttonStyle.getWidth();
    m_relativePos.h = m_buttonStyle.getHeight();
    m_couleur.r = m_couleur.g = m_couleur.b = 0;
    m_variable = variable;
    m_values = values;
    m_key = key;
    m_text = m_displayedText[m_index%(m_values.size())];
	m_stext.loadFromText(m_font, m_text, m_couleur);
}

void Button::setVariable(int* variable)
{
    m_variable = variable;
}

void Button::forceValue(unsigned int index)
{
    if(index < m_values.size())
     *m_variable = m_values[index];
    m_text = m_displayedText[m_index%(m_values.size())];

    m_stext.loadFromText(m_font, m_text, m_couleur);
}


void Button::display()
{

    if(!m_parent->isVisible())
        return;
    else
        m_active = true;

	WGameCore& wScreen = WGameCore::getInstance();
    MouseInput *in = wScreen.getInputListener().getMouseInput();
    SDL_Rect buf = m_relativePos, textPos = m_relativePos, mousePos = in->getMousePos();
    buf.x += (m_parent->getPos())->x;
    buf.y += (m_parent->getPos())->y;
    textPos.w = m_stext.getWidth();
    textPos.h = m_stext.getHeight();

    textPos = PosToCenterPicture(&textPos ,&buf);

    if(!m_static && IsPositionInBox(&mousePos, &buf) && in->getMouseState(SDL_BUTTON_LEFT)) //Si on presse le bouton
    {
		m_buttonStylePressed.render(buf.x, buf.y);
        textPos.x += 2; //pour faire un effet "bouton enfoncé", on décale le texte vers le bas droite
        textPos.y += 2;
    }
    else
        m_buttonStyle.render(buf.x, buf.y);

	m_stext.render(textPos.x, textPos.y);
}

void Button::refresh()
{
	WGameCore& wScreen = WGameCore::getInstance();
	MouseInput *in = wScreen.getInputListener().getMouseInput();
	SDL_Rect buf = m_relativePos, textPos = m_relativePos, mousePos = in->getMousePos();
	buf.x += (m_parent->getPos())->x;
	buf.y += (m_parent->getPos())->y;
	textPos.w = m_stext.getWidth();
	textPos.h = m_stext.getHeight();

	textPos = PosToCenterPicture(&textPos ,&buf);

	if(!m_static && IsPositionInBox(&mousePos, &buf) && in->mouseClick(SDL_BUTTON_LEFT) && m_variable != NULL) //Si on a cliqué sur le bouton (différent de l'état "bouton pressé" !)
	{
		//m_parent->pause();
		m_index++;
		*m_variable = m_values[m_index%(m_values.size())];
		m_text = m_displayedText[m_index%(m_values.size())];
		m_stext.loadFromText(m_font, m_text, m_couleur);
		in->setMouseLastState(SDL_BUTTON_LEFT, 0);
		in->setMouseState(SDL_BUTTON_LEFT, 0);
	}

	if (m_variable != NULL && *m_variable != m_values[m_index % (m_values.size())])
	{
		this->forceIndexFromCurrentValue();
		m_text =  m_displayedText[m_index%(m_values.size())];
		m_stext.loadFromText(m_font, m_text, m_couleur);
	}
}

std::string Button::getKey()
{
    return m_key;
}

void Button::forceIndexFromCurrentValue()
{
    for(unsigned int i = 0; i < m_values.size(); i++)
        if(m_values[i] == *m_variable)
            m_index = i;
}

Button::~Button()
{
    TTF_CloseFont(m_font);
}
