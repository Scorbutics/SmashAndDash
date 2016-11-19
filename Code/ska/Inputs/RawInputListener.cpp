#include <iostream>
#include "RawInputListener.h"
#include "../Utils/SkaConstants.h"
#include "../Exceptions/TerminateProcessException.h"
#include "../Utils/StringUtils.h"

ska::RawInputListener::RawInputListener()
{
}



ska::KeyInput& ska::RawInputListener::getKeyInput() {
    return m_keyIn;
}

ska::MouseInput& ska::RawInputListener::getMouseInput() {
	return m_mouseIn;
}

const std::wstring& ska::RawInputListener::getTextInput() const {
	return m_textInput;
}

void ska::RawInputListener::update() {
    SDL_Event event;
	m_mouseIn.setMouseLastPos(m_mouseIn.getMousePos());

	m_textInput = L"";
	m_keyIn.resetTriggers();
	m_mouseIn.resetTriggers();

	while(SDL_PollEvent(&event)) {
		switch (event.type) {
            case SDL_KEYDOWN:
                m_keyIn.setKeyState(event.key.keysym.scancode, 1);
                break;
            case SDL_KEYUP:
				m_keyIn.setKeyState(event.key.keysym.scancode, 0);
                break;
            case SDL_MOUSEMOTION:
				m_mouseIn.setMousePos(event);
                break;
            case SDL_MOUSEBUTTONDOWN:
				m_mouseIn.setMouseState(event.button.button, 1);
                break;
            case SDL_MOUSEBUTTONUP:
				m_mouseIn.setMouseState(event.button.button, 0);
                break;
			case SDL_TEXTEDITING:
				break;
			case SDL_TEXTINPUT:
				m_textInput += ska::StringUtils::toUTF8(event.edit.text);
				break;
            case SDL_QUIT:
				throw TerminateProcessException("Program quitted");
                break;
            default:
                break;
		}


		if(event.type == SDL_WINDOWEVENT) {
			switch(event.window.event) {

				case SDL_WINDOWEVENT_RESIZED:
					/*wScreen.resize(event.window.data1, event.window.data2);
					wScreen.flip();*/
						break;

				case SDL_WINDOWEVENT_EXPOSED:
						break;
				case SDL_WINDOWEVENT_SIZE_CHANGED:
					//wScreen.flip();
					//int width, height;
					/*
					if(event.resize.w > -abs(wScreen.getORel().x) + w->getNbrBlocX()*TAILLEBLOC)
						width = -abs(wScreen.getORel().x) + w->getNbrBlocX()*TAILLEBLOC;
					else
						width = event.resize.w;

					if(event.resize.h > -abs(wScreen.getORel().y) + w->getNbrBlocY()*TAILLEBLOC)
						height = -abs(wScreen.getORel().y) + w->getNbrBlocY()*TAILLEBLOC;
					else
						height = event.resize.h;*/

					//*screen = SDL_SetVideoMode(width, height, 32, SDL_HWSURFACE | SDL_RESIZABLE);
					/*
					wm->getGUI()->getDialog()->setPos(0, (*screen)->h - TAILLEBLOCFENETRE*4);
					wm->getGUI()->getImgDialog()->setPos(0, (*screen)->h - TAILLEBLOCFENETRE*9);
					wm->getGUI()->getAttackPokemonWindow()->setPos(0, (*screen)->h - TAILLEBLOCFENETRE*2);
					wm->getGUI()->getToolbar()->setPos((*screen)->w-7*TAILLEBLOCFENETRE, 0);*/

					break;

				default:
					break;
			}
		}



	}

}

ska::RawInputListener::~RawInputListener(){}





int GetDirectionFromChar(char directionChar)
{
    int buf = 0;
    if(directionChar == 'b')
        buf = 0;
    else if (directionChar == 'd')
        buf = 1;
    else if (directionChar == 'h')
        buf = 2;
    else if(directionChar == 'g')
        buf = 3;
    return buf;

}

/*ska::Rectangle MovementManagerSouris(ska::MouseInput *in, Character_ptr& hero)
{
	ska::Rectangle mousePos = in->getMousePos(), direction;

    if(in->getMouseState(SDL_BUTTON_LEFT) == 1)
    {
        if(mousePos.x - (hero->getPos().x + hero->getOffsetAndFrameSize().w/2) < 0)
            direction.x = -1;
        else
            direction.x = 1;

        if(mousePos.y - (hero->getPos().y + hero->getOffsetAndFrameSize().h/2) < 0)
            direction.y = -1;
        else
            direction.y = 1;
    }
    else
    {
        direction.x = 0;
        direction.y = 0;
    }

    return direction;

}*/

char GetCharFromDirection(int dir)
{
    char c;
    switch(dir)
    {
        case 0:
        c = 'b';
        break;

        case 1:
        c = 'd';
        break;

        case 2:
        c = 'h';
        break;

        case 3:
        default:
        c = 'g';
        break;
    }
    return c;
}


