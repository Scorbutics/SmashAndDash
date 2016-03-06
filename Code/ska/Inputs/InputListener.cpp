#include "InputListener.h"
#include "../Utils\SkaConstants.h"


//#include "../Gameplay\WGameCore.h"


ska::InputListener::InputListener()
{
}



ska::KeyInput* ska::InputListener::getKeyInput()
{
    return &m_keyIn;
}

ska::MouseInput* ska::InputListener::getMouseInput()
{
	return &m_mouseIn;
}

bool ska::InputListener::refresh(bool stuck)
{
	//WGameCore& wScreen = WGameCore::getInstance();

    this->updateEvents();
	if (stuck && m_keyIn.getQuit())
	{
		exit(0);
	}

    return (!stuck ? this->update() : true);
}


bool ska::InputListener::update()
{
	//WGameCore& wScreen = WGameCore::getInstance();
    bool dMenu = true;
	/*unsigned int power = wScreen.getHero()->getSpeedLimit() * 5;
	ska::Rectangle mousePos = m_mouseIn.getMousePos(), posHeroCenter = wScreen.getHero()->getCenterPos();
    posHeroCenter.x += wScreen.getORel().x;
    posHeroCenter.y += wScreen.getORel().y;*/

	if (m_keyIn.getQuit())
	{
		/*wScreen.setContinue(false);
		wScreen.quitFlip();*/
	}

	if (m_keyIn.getKeyState(SDL_SCANCODE_LSHIFT))
	{
		m_keyEvent.setScancode(SDL_SCANCODE_LSHIFT);
		m_keyEvent.setState(1);

		notifyObservers(m_keyEvent, 0);
	}
	else
	{
		m_keyEvent.setScancode(SDL_SCANCODE_LSHIFT);
		m_keyEvent.setState(0);

		notifyObservers(m_keyEvent, 0);
	}

	
	if (m_keyIn.getKeyState(SDL_SCANCODE_RETURN))
	{
		m_keyEvent.setScancode(SDL_SCANCODE_RETURN);
		m_keyEvent.setState(1);

		notifyObservers(m_keyEvent, 0);
	}


	
	if (m_keyIn.getKeyState(SDL_SCANCODE_SPACE))
	{
		m_keyEvent.setScancode(SDL_SCANCODE_SPACE);
		m_keyEvent.setState(1);

		notifyObservers(m_keyEvent, 0);
	}



	if (m_keyIn.getKeyState(SDL_SCANCODE_1))
	{
		m_keyEvent.setScancode(SDL_SCANCODE_1);
		m_keyEvent.setState(1);

		notifyObservers(m_keyEvent, 0);
	}
	else if (m_keyIn.getKeyState(SDL_SCANCODE_2))
	{
		m_keyEvent.setScancode(SDL_SCANCODE_2);
		m_keyEvent.setState(1);

		notifyObservers(m_keyEvent, 0);
	}
	else if (m_keyIn.getKeyState(SDL_SCANCODE_3))
	{
		m_keyEvent.setScancode(SDL_SCANCODE_3);
		m_keyEvent.setState(1);

		notifyObservers(m_keyEvent, 0);
	}
	else if (m_keyIn.getKeyState(SDL_SCANCODE_4))
	{
		m_keyEvent.setScancode(SDL_SCANCODE_4);
		m_keyEvent.setState(1);

		notifyObservers(m_keyEvent, 0);
	}



	if (m_keyIn.getKeyState(SDL_SCANCODE_S) && m_keyIn.getKeyState(SDL_SCANCODE_D))
    {
		m_keyEvent.setState(1);
		m_keyEvent.setScancode(SDL_SCANCODE_S);
		m_keyEvent.setSecondScancode(SDL_SCANCODE_D);
		notifyObservers(m_keyEvent, 0);
       
    }
	else if (m_keyIn.getKeyState(SDL_SCANCODE_W) && m_keyIn.getKeyState(SDL_SCANCODE_D))
    {
		m_keyEvent.setState(1);
		m_keyEvent.setScancode(SDL_SCANCODE_W);
		m_keyEvent.setSecondScancode(SDL_SCANCODE_D);
		notifyObservers(m_keyEvent, 0);

    }
	else if (m_keyIn.getKeyState(SDL_SCANCODE_W) && m_keyIn.getKeyState(SDL_SCANCODE_A))
    {
		m_keyEvent.setState(1);
		m_keyEvent.setScancode(SDL_SCANCODE_W);
		m_keyEvent.setSecondScancode(SDL_SCANCODE_A);
		notifyObservers(m_keyEvent, 0);

    }
	else if (m_keyIn.getKeyState(SDL_SCANCODE_S) && m_keyIn.getKeyState(SDL_SCANCODE_A))
    {
		m_keyEvent.setState(1);
		m_keyEvent.setScancode(SDL_SCANCODE_S);
		m_keyEvent.setSecondScancode(SDL_SCANCODE_A);
		notifyObservers(m_keyEvent, 0);
    }
	else if (m_keyIn.getKeyState(SDL_SCANCODE_W))
    {
		m_keyEvent.setState(1);
		m_keyEvent.setScancode(SDL_SCANCODE_W);
		m_keyEvent.setSecondScancode(-1);
		notifyObservers(m_keyEvent, 0);
    }
	else if (m_keyIn.getKeyState(SDL_SCANCODE_S))
    {
		m_keyEvent.setState(1);
		m_keyEvent.setScancode(SDL_SCANCODE_S);
		m_keyEvent.setSecondScancode(-1);
		notifyObservers(m_keyEvent, 0);
    }
	else if (m_keyIn.getKeyState(SDL_SCANCODE_D))
    {
		m_keyEvent.setState(1);
		m_keyEvent.setScancode(SDL_SCANCODE_D);
		m_keyEvent.setSecondScancode(-1);
		notifyObservers(m_keyEvent, 0);
    }
	else if (m_keyIn.getKeyState(SDL_SCANCODE_A))
    {
		m_keyEvent.setState(1);
		m_keyEvent.setScancode(SDL_SCANCODE_A);
		m_keyEvent.setSecondScancode(-1);
		notifyObservers(m_keyEvent, 0);
    }
	else if (m_mouseIn.mouseClick(SDL_BUTTON_RIGHT))   //Enclenchement du Pathfinding
    {
		m_mouseEvent.setX(m_mouseIn.getMousePos().x);
		m_mouseEvent.setY(m_mouseIn.getMousePos().y);
		m_mouseEvent.setButton(SDL_BUTTON_RIGHT);
		m_mouseEvent.setClicked(true);

		//Si c'est un clic hors des fenêtres, on notifie le héro
		/*if (!wScreen.getGUI().isMouseOnAWindow())
			notifyObserver(wScreen.getHero(), &m_mouseEvent);
		else //sinon il notifie la GUI
			notifyObserver(&wScreen.getGUI(), &m_mouseEvent);*/
		
		
    }
	else if (m_mouseIn.getMouseState(SDL_BUTTON_LEFT)) /*&& mouseTranslation.x != 0 */
    {
		ska::Rectangle mousePos = m_mouseIn.getMousePos();
		m_mouseEvent.setX(m_mouseIn.getMousePos().x);
		m_mouseEvent.setY(m_mouseIn.getMousePos().y);
		m_mouseEvent.setButton(SDL_BUTTON_LEFT);
		m_mouseEvent.setState(1);
		
		/*if (!wScreen.getGUI().isMovingAWindow() && !wScreen.getGUI().isMouseOnAWindow() && wScreen.getGUI().isPositionOnButton(&mousePos) == -1)
			notifyObserver(wScreen.getHero(), &m_mouseEvent);
		else
			notifyObserver(&wScreen.getGUI(), &m_mouseEvent);*/

    }
   

    return dMenu;

}


void ska::InputListener::updateEvents()
{
    SDL_Event event;
	//WGameCore& wScreen = WGameCore::getInstance();
	m_mouseIn.setMouseLastState(SDL_BUTTON_LEFT, m_mouseIn.getMouseState(SDL_BUTTON_LEFT));
	m_mouseIn.setMouseLastState(SDL_BUTTON_RIGHT, m_mouseIn.getMouseState(SDL_BUTTON_RIGHT));
	m_mouseIn.setMouseLastPos(m_mouseIn.getMousePos());

	while(SDL_PollEvent(&event))
	{
		switch (event.type)
		{
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
            case SDL_QUIT:
				m_keyIn.setQuit(1);
                break;
            default:
                break;
		}


		if(event.type == SDL_WINDOWEVENT)
		{
			switch(event.window.event)
			{
				
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

ska::InputListener::~InputListener(){}





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


