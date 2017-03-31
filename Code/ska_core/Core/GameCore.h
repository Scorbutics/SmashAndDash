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
    class GameCore : public SceneHolder {
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

            if (!(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024))) {
                std::cerr << "Impossible d'initialiser SDL_mixer : " << SDL_GetError() << std::endl;
            }

            if(TTF_Init() == -1) {
                std::cerr << "Erreur d'initialisation de TTF_Init : " << TTF_GetError() << std::endl;
            }

            m_mainWindow = std::make_unique<Window>(title, w, h);

            m_eventDispatcher.template addMultipleObservers<SoundEvent, WorldEvent>(m_soundManager, m_soundManager);
        }

        virtual ~GameCore() {
        	TTF_Quit();

            Mix_CloseAudio();
            Mix_Quit();

            IMG_Quit();

            SDL_Quit();
        };

        bool refresh() {
            auto continuer = true;
            while (continuer) {
                continuer = refreshInternal();
            }
            return true;
        }

        void nextScene(ScenePtr& scene) override {
            m_sceneHolder.nextScene(scene);
        }

        void update() override {
            m_sceneHolder.update();
        }

        ska::ScenePtr& getScene() override {
            return m_sceneHolder.getScene();
        }

    protected:
        EM& getEntityManager() {
            return m_entityManager;
        }

        ED& getEventDispatcher() {
            return m_eventDispatcher;
        }

        InputContextManager& getInputContext() {
            return m_playerICM;
        }

        void addInputContext(EnumContextManager em, InputContextPtr& c) {
            m_playerICM.addContext(em, c);
        }

        Window& getWindow() {
            return *m_mainWindow;
        }

    private:
        bool refreshInternal() {
            //t et t0 sont les temps pour la gestion des fps
            long t = 0;
            long t0 = 0;

            //Ici, transition entrante
            static const auto FPS = 63;
            static const auto TICKS = 1000 / FPS;

            try {
                for (;;) {
                    t = ska::TimeUtils::getTicks();
                    if (t - t0 > TICKS)  {
                        //Rafraîchissement à chaque frame : graphique puis évènementiel
                        graphicUpdate();
                        eventUpdate(t - t0);

                        m_mainWindow->display();
                        // Le temps "actuel" devient le temps "precedent" pour nos futurs calculs
                        //m_fpsCalculator.calculate(t - t0);
                        t0 = t;
                    } else {
                        /* Temporisation entre 2 frames */
                        TimeUtils::wait(TICKS - (t - t0));
                    }
                }
            } catch (ska::SceneDiedException& sde) {
                return false;
            }
        }

        void graphicUpdate(void) {
        	m_sceneHolder.getScene()->graphicUpdate(m_drawables);
            //TODO : dans GUI
            //m_drawables.add(m_fpsCalculator.getRenderable());
            m_drawables.draw();
            m_drawables.clear();
        }

        void eventUpdate(unsigned int ellapsedTime) {
            m_entityManager.refresh();
            /* Scene dependent event update */
            m_sceneHolder.getScene()->eventUpdate(ellapsedTime);
            m_sceneHolder.update();
        }


        EM m_entityManager;
        ED m_eventDispatcher;
        D m_drawables;
        S m_soundManager;

        std::unique_ptr<Window> m_mainWindow;

        RawInputListener m_rawInputListener;
        InputContextManager m_playerICM;

        SceneHolderCore m_sceneHolder;


    };
}
