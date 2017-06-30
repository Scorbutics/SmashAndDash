#pragma once
#include <unordered_set>
#include "State.h"
#include "StateHolder.h"
#include "../../Draw/IGraphicSystem.h"
#include "../../ECS/ISystem.h"
#include "../../Core/Priorized.h"
#include <algorithm>

namespace ska {
	class Window;
	class InputContextManager;

	template <class EM, class ED>
	class StateBase : public State {
	public:
		StateBase(EM& em, ED& ed, Window& w, InputContextManager& ril, StateHolder& sh) :
			State(sh),
			m_entityManager(em),
			m_eventDispatcher(ed),
			m_window(w),
			m_inputCManager(ril),
			m_state(0) {

		}

		StateBase(EM& em, ED& ed, Window& w, InputContextManager& ril, State& oldState) :
			State(oldState),
			m_entityManager(em),
			m_eventDispatcher(ed),
			m_window(w),
			m_inputCManager(ril),
			m_state(0) {

		}

		virtual void graphicUpdate(unsigned int ellapsedTime, DrawableContainer& drawables) override final {
			onGraphicUpdate(ellapsedTime, drawables);

			/* Graphics */
			for (auto& s : m_graphics) {
				s->setDrawables(drawables);
				s->update(ellapsedTime);
			}

			for (auto& s : m_subStates) {
				s->graphicUpdate(ellapsedTime, drawables);
			}

			for (auto& s : m_linkedSubStates) {
				s->graphicUpdate(ellapsedTime, drawables);
			}

		}

		virtual void eventUpdate(unsigned int ellapsedTime) override final {
			onEventUpdate(ellapsedTime);

			/* Logics */
			for (auto& s : m_logics) {
				s->update(ellapsedTime);
			}

			for (auto& s : m_subStates) {
				s->eventUpdate(ellapsedTime);
			}

			for (auto& s : m_linkedSubStates) {
				s->eventUpdate(ellapsedTime);
			}

		}

		void load(std::unique_ptr<State>* lastState) override final {
			beforeLoad(lastState);
			m_state = 1;

			for (auto& s : m_subStates) {
				s->load(lastState);
			}

			for (auto& s : m_linkedSubStates) {
				s->load(lastState);
			}

			m_state = 2;

			afterLoad(lastState);

			m_state = 3;

		}

		bool unload() override final {
			if (m_state == 3) {
				auto beforeUnloaded = !beforeUnload();
				if(beforeUnloaded) {
					m_state = 2;
				}
			}

			//If main scene beforeUnload is finished, THEN we can unload subscenes
			if (m_state == 2) {
				auto wTransitions = waitTransitions();
				if (wTransitions) {
					m_state = 1;
				}
			}

			if(m_state == 1) {
				auto subscenesUnloaded = true;
				for (auto& s : m_subStates) {
					subscenesUnloaded &= !s->unload();
				}

				for (auto& s : m_linkedSubStates) {
					subscenesUnloaded &= !s->unload();
				}

				if (subscenesUnloaded) {
					m_state = 0;
				}
			}

			//If everything is unloaded, THEN we can call main scene afterUnload
			if(m_state == 0) {
				auto afterUnloaded = !afterUnload();
				if (afterUnloaded) {
					m_state = -1;
				}
			}

			if(m_state == -1) {
				auto wTransitions = waitTransitions();
				if (wTransitions) {
					m_state = -2;
				}
			}

			return m_state != -2;
		}

		void linkSubState(State& subState) {
			m_linkedSubStates.insert(&subState);
		}

		void unlinkSubState(State& subState) {
			m_linkedSubStates.erase(&subState);
		}

		template <class T>
		ska::Runnable& queueTask(std::unique_ptr<T>& t) {
		    return m_holder.queueTask(t);
		}

		template<class SC, class ... Args>
		SC* makeNextScene(Args&&... args) {
			auto nState = std::make_unique<SC>(m_entityManager, m_eventDispatcher, m_window, m_inputCManager, *this, std::forward<Args>(args)...);
			auto result = nState.get();
			m_holder.nextState(std::move(nState));
			m_holder.update();
			return result;
		}

		template<class SC1, class SC, class ... Args>
		SC* makeNextStateAndTransmitLinkedSubstates(SC1& oldScene, Args&&... args) {
			auto nState = std::make_unique<SC>(m_entityManager, m_eventDispatcher, m_window, m_inputCManager, *this, std::forward<Args>(args)...);
			auto result = nState.get();
			m_holder.nextState(std::move(nState));
			result->transmitLinkedSubstates(oldScene);
			m_holder.update();
			return result;
		}

		virtual ~StateBase() = default;

    protected:
		virtual void beforeLoad(std::unique_ptr<State>* lastScene) {
		}

		virtual void afterLoad(std::unique_ptr<State>* lastScene) {
		}

		virtual bool beforeUnload() {
			return false;
		}

		virtual bool afterUnload() {
			return false;
		}

		virtual void onGraphicUpdate(unsigned int ellapsedTime, DrawableContainer& drawables) {
		}

		virtual void onEventUpdate(unsigned int ellapsedTime) {
		}

		template<class SC, class ...Args>
		SC* addSubState(Args&& ... args){
			auto s = std::make_unique<SC>(m_entityManager, m_eventDispatcher, m_window, m_inputCManager, m_holder, std::forward<Args>(args)...);
			SC* result = static_cast<SC*>(s.get());
			m_subStates.push_back(std::move(s));
			return result;
		}

		template<class S, class ...Args>
		S* addLogic(Args&& ... args) {
			return addPriorizedLogic<S, Args...>(static_cast<int>(m_logics.size()), std::forward<Args>(args)...);
		}

        template<class S, class ...Args>
		S* addPriorizedLogic(int priority, Args&& ... args) {
			auto s = std::make_unique<S>(m_entityManager, std::forward<Args>(args)...);
			S* result = static_cast<S*>(s.get());
			m_logics.push_back(std::move(s));
			std::sort(m_logics.begin(), m_logics.end(), Priorized::comparatorInf<std::unique_ptr<ISystem>>);
			return result;
		}

		template<class S, class ...Args>
		std::unique_ptr<S> createLogic(Args&&... args){
			return std::make_unique<S>(m_entityManager, std::forward<Args>(args)...);
		}

		template<class S, class ...Args>
		S* addGraphic(Args&& ... args) {
			return addPriorizedGraphic<S, Args...>(static_cast<int>(m_graphics.size()), std::forward<Args>(args)...);
		}

        template<class S, class ...Args>
		S* addPriorizedGraphic(int priority, Args&& ... args) {
			auto s = std::make_unique<S>(m_entityManager, std::forward<Args>(args)...);
			S* result = static_cast<S*>(s.get());
			m_graphics.push_back(std::move(s));
			std::sort( m_graphics.begin(), m_graphics.end(), Priorized::comparatorInf<std::unique_ptr<IGraphicSystem>>);
			return result;
		}

		EM& m_entityManager;
		ED& m_eventDispatcher;
		Window& m_window;
		InputContextManager& m_inputCManager;

    private:
        bool waitTransitions() const {
			return !m_holder.hasRunningTask();
		}

		template<class SC>
		void transmitLinkedSubstates(SC& scene) {
			m_linkedSubStates = scene.m_linkedSubStates;
		}

		std::vector<std::unique_ptr<ISystem>> m_logics;
		std::vector<std::unique_ptr<IGraphicSystem>> m_graphics;

		std::vector<std::unique_ptr<State>> m_subStates;
		std::unordered_set<State*> m_linkedSubStates;
        int m_state;



	};
}
