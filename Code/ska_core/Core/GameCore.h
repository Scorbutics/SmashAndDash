#pragma once
#include <ctime>
#include <memory>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>

#include "Scene/SceneHolderCore.h"
#include "../Exceptions/SceneDiedException.h"
#include "../Data/Events/WorldEvent.h"
#include "../Data/Events/SoundEvent.h"
#include "../Inputs/RawInputListener.h"
#include "../Inputs/InputContextManager.h"
#include "../Utils/TimeUtils.h"
#include "Window.h"

namespace ska {
    template <class EM, class ED, class D, class S>
    class GameCore {
    public:
        GameCore(const std::string& title, unsigned int w, unsigned int h) :
            m_soundManager(m_eventDispatcher),
            m_playerICM(m_rawInputListener) {

            /* TODO : ne plus utiliser srand...  */
            srand(static_cast<unsigned int>(time(nullptr)));

            // Chargement des images, de l'audio et du texte
            if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
                throw ska::IllegalStateException("Erreur lors de l'initialisation de la SDL : " + std::string(SDL_GetError()));
            }

            /* Fix GDB Bug with named thread on windows (Mixer raises an exception when init) */
            if(!SDL_SetHint(SDL_HINT_WINDOWS_DISABLE_THREAD_NAMING, "1")) {
                std::clog << "Attention : Windows nomme actuellement les threads créés par l'application alors que le programme tente de désactiver cette fonctionnalité." << std::endl;
            }

            if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear")) {
                std::clog << "Attention : Linear texture filtering impossible à activer." << std::endl;
            }

            if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
                throw ska::IllegalStateException("Erreur lors de l'initialisation de SDL_image : " + std::string(IMG_GetError()));
            }

            if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024) == -1) {
				std::cerr << "Impossible d'initialiser SDL_mixer : " << Mix_GetError() << std::endl;
            }

            if(TTF_Init() == -1) {
                std::cerr << "Erreur d'initialisation de TTF_Init : " << TTF_GetError() << std::endl;
            }

            m_mainWindow = std::make_unique<Window>(title, w, h);

            m_eventDispatcher.template addMultipleObservers<SoundEvent, WorldEvent>(m_soundManager, m_soundManager);
        }

        virtual ~GameCore() {
			m_eventDispatcher.template removeMultipleObservers<SoundEvent, WorldEvent>(m_soundManager, m_soundManager);

        	TTF_Quit();
            Mix_CloseAudio();
            Mix_Quit();
            IMG_Quit();
            SDL_Quit();
        };

        bool run() {
            auto continuer = true;
            while (continuer) {
                continuer = refreshInternal();
            }
            return true;
        }

		template<class SC, class ... Args>
		std::unique_ptr<SC> makeScene(Args&&... args) {
			return std::make_unique<SC>(m_entityManager, m_eventDispatcher, *m_mainWindow, m_playerICM, m_sceneHolder, std::forward<Args>(args)...);
        }

		template<class SC, class ... Args>
		SC& navigateToScene(Args&&... args) {
			auto sc = makeScene<SC, Args...>(std::forward<Args>(args)...);
			auto result = sc.get();
			m_sceneHolder.nextScene(std::move(sc));
			m_sceneHolder.update();
			return *result;
		}

    protected:
        void addInputContext(EnumContextManager em, InputContextPtr&& c) {
            m_playerICM.addContext(em, std::move(c));
        }

    private:
        bool refreshInternal() {
            unsigned long t = 0;
			unsigned long t0 = 0;

            static const auto FPS = 63;
            static const auto TICKS = 1000 / FPS;

            try {
                for (;;) {
                    t = ska::TimeUtils::getTicks();
					
					const auto ellapsedTime = t - t0;

					if (ellapsedTime > TICKS)  {
                        graphicUpdate();
						eventUpdate(ellapsedTime);

                        m_mainWindow->display();
                        
                        //m_fpsCalculator.calculate(t - t0);
                        t0 = t;
                    } else {
                        /* Temporisation entre 2 frames */
						TimeUtils::wait(TICKS - (ellapsedTime));
                    }
                }
            } catch (ska::SceneDiedException&) {
                return false;
            }
        }

        void graphicUpdate(void) {
        	m_sceneHolder.graphicUpdate(m_drawables);
            //TODO : dans GUI
            //m_drawables.add(m_fpsCalculator.getRenderable());
            m_drawables.draw();
            m_drawables.clear();
        }

        void eventUpdate(unsigned int ellapsedTime) {
			m_entityManager.refresh();
			/* Raw input acquisition */
			m_playerICM.refresh();

			m_sceneHolder.update();
            m_sceneHolder.eventUpdate(ellapsedTime);
        }


        EM m_entityManager;

	protected:
        ED m_eventDispatcher;
	
	private:
        D m_drawables;
        S m_soundManager;

        std::unique_ptr<Window> m_mainWindow;

        RawInputListener m_rawInputListener;
        InputContextManager m_playerICM;

        SceneHolderCore m_sceneHolder;


    };
}
