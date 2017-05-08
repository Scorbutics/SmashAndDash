#pragma once
#include <memory>

#include "Scene/SceneHolderCore.h"
#include "../Exceptions/SceneDiedException.h"
#include "../Data/Events/WorldEvent.h"
#include "../Data/Events/SoundEvent.h"
#include "../Inputs/RawInputListener.h"
#include "../Inputs/InputContextManager.h"
#include "../Utils/TimeUtils.h"
#include "Window.h"
#include "GameApp.h"
#include "../../Game/Utils/IDs.h"

namespace ska {
    template <class EM, class ED, class D, class S>
    class GameCore : 
		public GameApp {
    public:
        GameCore() :
            m_soundManager(m_eventDispatcher),
            m_playerICM(m_rawInputListener) {

			ska::IniReader reader("gamesettings.ini");
			auto widthBlocks = reader.get<int>("Window width_blocks");
			auto heightBlocks = reader.get<int>("Window height_blocks");
			const auto& title = reader.get<std::string>("Window title");

			createWindow(title, widthBlocks, heightBlocks);

            m_eventDispatcher.template addMultipleObservers<SoundEvent, WorldEvent>(m_soundManager, m_soundManager);
        }

        virtual ~GameCore() {
			m_eventDispatcher.template removeMultipleObservers<SoundEvent, WorldEvent>(m_soundManager, m_soundManager);
        };



        void run() override {
            auto continuer = true;
            while (continuer) {
                continuer = refreshInternal();
            }
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
		void createWindow(const std::string& title, unsigned int wBlocks, unsigned int hBlocks) {
			m_mainWindow = std::make_unique<Window>(title, wBlocks * TAILLEBLOC, hBlocks * TAILLEBLOC);
		}

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
