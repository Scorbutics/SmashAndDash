#pragma once
#include <unordered_set>
#include "State.h"
#include "StateHolder.h"
#include "../../Draw/IGraphicSystem.h"
#include "../../ECS/ISystem.h"
#include "StateBuilder.h"

namespace ska {
	class Window;
	class InputContextManager;

	template <class EM, class ED>
	class StateBase : public State {
		
	public:
		StateBase(EM& em, ED& ed, StateHolder& sh) :
			State(sh),
			m_builder(sh, em, ed),
			m_state(0) {
		}

		StateBase(EM& em, ED& ed, State& oldState) :
			State(oldState),
			m_builder(oldState.m_holder, em, ed),
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

		template<class State>
		void transmitLinkedSubstates(State& state) {
			m_linkedSubStates = state.m_linkedSubStates;
		}

		template<class State, class ...Args>
		State* addSubState(Args&& ... args) {
			return m_builder.template addSubState<State, Args...>(m_subStates, std::forward<Args>(args)...);
		}

		template<class State, class ... Args>
		State* makeNextState(Args&&... args) {
			return m_builder.template makeNextState<State, Args...>(std::forward<Args>(args)...);
		}

		template<class SubState, class ... Args>
		State* makeNextStateAndTransmitLinkedSubstates(Args&&... args) {
			return m_builder.template makeNextStateAndTransmitLinkedSubstates<decltype(*this), SubState, Args...>(*this, std::forward<Args>(args)...);
		}

		template<class System, class ...Args>
		std::unique_ptr<System> createLogic(Args&&... args) {
			return m_builder.template createLogic<System>(std::forward<Args>(args)...);
		}

		virtual ~StateBase() = default;

    protected:
		using StateData = StateData<EM, ED>;

		template<class System, class ...Args>
		System* addPriorizedLogic(int priority, Args&& ... args) {
			return m_builder.addPriorizedLogic<System, Args...>(m_logics, priority, std::forward<Args>(args)...);
		}

		template<class System, class ...Args>
		System* addLogic(Args&& ... args) {
			return this->m_builder.addPriorizedLogic<System, Args...>(m_logics, static_cast<int>(m_logics.size()), std::forward<Args>(args)...);
		}

		template<class System, class ...Args>
		System* addPriorizedGraphic(int priority, Args&& ... args) {
			return m_builder.addPriorizedGraphic<System, Args...>(m_graphics, priority, std::forward<Args>(args)...);
		}
		
		template<class System, class ...Args>
		System* addGraphic(Args&& ... args) {
			return this->m_builder.addPriorizedGraphic<System, Args...>(m_graphics, static_cast<int>(m_graphics.size()), std::forward<Args>(args)...);
		}

		template <class T>
		ska::Runnable& queueTask(std::unique_ptr<T>&& t) {
			return m_builder.queueTask(std::forward<std::unique_ptr<T>>(t));
		}

		virtual void beforeLoad(std::unique_ptr<State>*) {
		}

		virtual void afterLoad(std::unique_ptr<State>*) {
		}

		virtual bool beforeUnload() {
			return false;
		}

		virtual bool afterUnload() {
			return false;
		}

		virtual void onGraphicUpdate(unsigned int, DrawableContainer& ) {
		}

		virtual void onEventUpdate(unsigned int ) {
		}

		
    private:
        bool waitTransitions() const {
			return !m_builder.hasRunningTask();
		}

		StateBuilder<EM, ED> m_builder;

		std::vector<std::unique_ptr<ISystem>> m_logics;
		std::vector<std::unique_ptr<IGraphicSystem>> m_graphics;

		std::vector<std::unique_ptr<State>> m_subStates;
		std::unordered_set<State*> m_linkedSubStates;
        int m_state;
		
		
	};
}
