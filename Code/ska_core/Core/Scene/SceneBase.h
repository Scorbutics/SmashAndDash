#pragma once
#include <unordered_set>
#include "Scene.h"
#include "SceneHolder.h"
#include "../../Draw/IGraphicSystem.h"
#include "../../ECS/ISystem.h"
#include "Task/Task.h"

namespace ska {
	class Window;
	class InputContextManager;

	template <class EM, class ED>
	class SceneBase : public Scene {
	public:
		SceneBase(EM& em, ED& ed, Window& w, InputContextManager& ril, SceneHolder& sh) :
			Scene(sh),
			m_entityManager(em),
			m_eventDispatcher(ed),
			m_window(w),
			m_inputCManager(ril),
			m_state(0) {

		}

		SceneBase(EM& em, ED& ed, Window& w, InputContextManager& ril, Scene& oldScene) :
			Scene(oldScene),
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

			for (auto& s : m_subScenes) {
				s->graphicUpdate(ellapsedTime, drawables);
			}

			for (auto& s : m_linkedSubScenes) {
				s->graphicUpdate(ellapsedTime, drawables);
			}

		}

		virtual void eventUpdate(unsigned int ellapsedTime) override final {
			onEventUpdate(ellapsedTime);

			/* Logics */
			for (auto& s : m_logics) {
				s->update(ellapsedTime);
			}

			for (auto& s : m_subScenes) {
				s->eventUpdate(ellapsedTime);
			}

			for (auto& s : m_linkedSubScenes) {
				s->eventUpdate(ellapsedTime);
			}

		}

		void load(std::unique_ptr<Scene>* lastScene) override final {
			beforeLoad(lastScene);
			m_state = 1;

			for (auto& s : m_subScenes) {
				s->load(lastScene);
			}

			for (auto& s : m_linkedSubScenes) {
				s->load(lastScene);
			}

			m_state = 2;

			afterLoad(lastScene);

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
				for (auto& s : m_subScenes) {
					subscenesUnloaded &= !s->unload();
				}

				for (auto& s : m_linkedSubScenes) {
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

		void linkSubScene(Scene& subScene) {
			m_linkedSubScenes.insert(&subScene);
		}

		void unlinkSubScene(Scene& subScene) {
			m_linkedSubScenes.erase(&subScene);
		}

		ska::Runnable& queueTask(RunnablePtr& t) {
		    return m_holder.queueTask(t);
		}

		template<class SC, class ... Args>
		SC* makeNextScene(Args&&... args) {
			auto nScene = std::make_unique<SC>(m_entityManager, m_eventDispatcher, m_window, m_inputCManager, *this, std::forward<Args>(args)...);
			auto result = nScene.get();
			m_holder.nextScene(std::move(nScene));
			m_holder.update();
			return result;
		}

		template<class SC1, class SC, class ... Args>
		SC* makeNextSceneAndTransmitLinkedSubscenes(SC1& oldScene, Args&&... args) {
			auto nScene = std::make_unique<SC>(m_entityManager, m_eventDispatcher, m_window, m_inputCManager, *this, std::forward<Args>(args)...);
			auto result = nScene.get();
			m_holder.nextScene(std::move(nScene));
			result->transmitLinkedSubscenes(oldScene);
			m_holder.update();
			return result;
		}

		virtual ~SceneBase() = default;


	private:
        bool waitTransitions() const {
			return !m_holder.hasRunningTask();
		}

		template<class SC>
		void transmitLinkedSubscenes(SC& scene) {
			m_linkedSubScenes = scene.m_linkedSubScenes;
		}

		std::vector<std::unique_ptr<ISystem>> m_logics;
		std::vector<std::unique_ptr<IGraphicSystem>> m_graphics;

		std::vector<std::unique_ptr<Scene>> m_subScenes;
		std::unordered_set<Scene*> m_linkedSubScenes;

		int m_state;

	protected:
/*		template<typename  R, typename ...TR, typename ...TS>
		void transition(typename meta::Identity<std::function<R(Task<R, TaskReceiver<TR...>, TaskSender<TS...>>&, TR...)>>::type const& t) {
			m_holder.queueTask(std::make_unique<Task<R, TaskReceiver<TR...>, TaskSender<TS...>>>(t));
		}

		template<typename R, typename ...TR, typename ...TS>
		void transition(typename meta::Identity<std::function<R(Task<R, TaskReceiver<TR...>, TaskSender<TS...>>&, TR...)>>::type const& t, ITask<TR...>& previous) {
			m_holder.queueTask(std::make_unique<Task<R, TaskReceiver<TR...>, TaskSender<TS...>>>(t, previous));
		}*/

		virtual void beforeLoad(std::unique_ptr<Scene>* lastScene) {
		}

		virtual void afterLoad(std::unique_ptr<Scene>* lastScene) {
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
		SC* addSubScene(Args&& ... args){
			auto s = std::make_unique<SC>(m_entityManager, m_eventDispatcher, m_window, m_inputCManager, m_holder, std::forward<Args>(args)...);
			SC* result = static_cast<SC*>(s.get());
			m_subScenes.push_back(std::move(s));
			return result;
		}

		template<class S, class ...Args>
		S* addLogic(Args&& ... args) {
			auto s = std::make_unique<S>(m_entityManager, std::forward<Args>(args)...);
			S* result = static_cast<S*>(s.get());
			m_logics.push_back(std::move(s));
			return result;
		}

		template<class S, class ...Args>
		std::unique_ptr<S> createLogic(Args&&... args){
			return std::make_unique<S>(m_entityManager, std::forward<Args>(args)...);
		}

		template<class S, class ...Args>
		S* addGraphic(Args&& ... args) {
			auto s = std::make_unique<S>(m_entityManager, std::forward<Args>(args)...);
			S* result = static_cast<S*>(s.get());
			m_graphics.push_back(std::move(s));
			return result;
		}

		EM& m_entityManager;
		ED& m_eventDispatcher;
		Window& m_window;
		InputContextManager& m_inputCManager;
	};
}
