#pragma once
#include <memory>

#include "State/StateHolderCore.h"
#include "../Exceptions/StateDiedException.h"
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

			IniReader reader("gamesettings.ini");
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
		std::unique_ptr<SC> makeState(Args&&... args) {
			return std::make_unique<SC>(m_entityManager, m_eventDispatcher, *m_mainWindow, m_playerICM, m_stateHolder, std::forward<Args>(args)...);
        }

		template<class SC, class ... Args>
		SC& navigateToState(Args&&... args) {
			auto sc = makeState<SC, Args...>(std::forward<Args>(args)...);
			auto result = sc.get();
			m_stateHolder.nextState(std::move(sc));
			return *result;
		}

    protected:
		template<class I, class ... Args>
        void addInputContext(EnumContextManager em, Args&&... args) {
            m_playerICM.addContext(em, std::make_unique<I>(m_rawInputListener, std::forward<Args>(args)...));
        }

    private:
		void createWindow(const std::string& title, unsigned int wBlocks, unsigned int hBlocks) {
			m_mainWindow = std::make_unique<Window>(title, wBlocks * TAILLEBLOC, hBlocks * TAILLEBLOC);
		}

        bool refreshInternal() {
	        unsigned long t0 = ska::TimeUtils::getTicks();
			const auto ti = ticksWanted();
			auto accumulator = ti;

			try {
				
                for (;;) {
                    unsigned long t = ska::TimeUtils::getTicks();
					
					const auto ellapsedTime = t - t0;
					t0 = t;

                	accumulator += ellapsedTime;

					while (accumulator >= ti) {
						eventUpdate(ti);
						accumulator -= ti;
                    }

					graphicUpdate(ellapsedTime);
                }
            } catch (ska::StateDiedException&) {
                return true;
            }
        }

        void graphicUpdate(unsigned int ellapsedTime) {
        	m_stateHolder.graphicUpdate(ellapsedTime, m_drawables);
            m_drawables.draw();
            m_drawables.clear();

			m_mainWindow->display();
        }

        void eventUpdate(unsigned int ellapsedTime) {
			m_entityManager.refresh();
			/* Raw input acquisition */
			m_playerICM.refresh();

			m_stateHolder.update();
            m_stateHolder.eventUpdate(ellapsedTime);
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

        StateHolderCore m_stateHolder;


    };
}
