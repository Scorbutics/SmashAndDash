#pragma once
#include <algorithm>
#include "StateHolder.h"
#include "StateData.h"
#include "../Priorized.h"

namespace ska {
	template <class EntityManager, class EventDispatcher>
	class StateBuilder {
		public:
			StateBuilder(StateHolder& holder, EntityManager& em, EventDispatcher& ed) :
				m_data(em, ed), 
				m_holder(holder) {
			}

			template<class State, class ... Args>
			State* makeNextState(Args&&... args) {
				auto nState = std::make_unique<State>(m_data, m_holder, std::forward<Args>(args)...);
				auto result = nState.get();
				m_holder.nextState(std::move(nState));
				m_holder.update();
				return result;
			}

			template<class StateT, class SubState, class ... Args>
			SubState* makeNextStateAndTransmitLinkedSubstates(StateT& oldState, Args&&... args) {
				auto nState = std::make_unique<SubState>(m_data, oldState, std::forward<Args>(args)...);
				auto result = nState.get();
				m_holder.nextState(std::move(nState));
				result->transmitLinkedSubstates(oldState);
				m_holder.update();
				return result;
			}

			template<class StateT, class ...Args>
			StateT* addSubState(std::vector<std::unique_ptr<State>>& subStates, Args&& ... args) {
				auto s = std::make_unique<StateT>(m_data, m_holder, std::forward<Args>(args)...);
				auto result = static_cast<StateT*>(s.get());			
				subStates.push_back(std::move(s));
				return result;
			}

			template<class System, class ...Args>
			std::unique_ptr<System> createLogic(Args&&... args) {
				return std::make_unique<System>(m_data.m_entityManager, std::forward<Args>(args)...);
			}

			template<class System, class ...Args>
			System* addPriorizedLogic(std::vector<std::unique_ptr<ISystem>>& logics, int priority, Args&& ... args) {
				auto s = std::move(createLogic<System, Args...>(std::forward<Args>(args)...));
				s->setPriority(priority);
				auto result = static_cast<System*>(s.get());
				logics.push_back(std::move(s));
				std::sort(logics.begin(), logics.end(), Priorized::comparatorInf<std::unique_ptr<ISystem>>);
				return result;
			}

			template<class System, class ...Args>
			System* addPriorizedGraphic(std::vector<std::unique_ptr<IGraphicSystem>>& graphics, int priority, Args&& ... args) {
				auto system = std::make_unique<System>(m_data.m_entityManager, std::forward<Args>(args)...);
				auto result = static_cast<System*>(system.get());
				graphics.push_back(std::move(system));
				std::sort(graphics.begin(), graphics.end(), Priorized::comparatorInf<std::unique_ptr<IGraphicSystem>>);
				result->Priorized::setPriority(priority);
				return result;
			}

			template <class Task>
			ska::Runnable& queueTask(std::unique_ptr<Task>&& t){
				return m_holder.queueTask(std::forward<std::unique_ptr<Task>>(t));
			}

			bool hasRunningTask() const {
				return m_holder.hasRunningTask();
			}

	private:
		using StateData = StateData<EntityManager, EventDispatcher>;
		StateData m_data;
		StateHolder& m_holder;
	};
}
