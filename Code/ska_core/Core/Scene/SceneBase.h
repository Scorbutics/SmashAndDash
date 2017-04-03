#pragma once
#include <unordered_set>
#include "Scene.h"
#include "SceneHolder.h"
#include "../../Draw/IGraphicSystem.h"
#include "../../ECS/ISystem.h"

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
			m_inputCManager(ril) {

		}

		SceneBase(EM& em, ED& ed, Window& w, InputContextManager& ril, Scene& oldScene) :
			Scene(oldScene),
			m_entityManager(em),
			m_eventDispatcher(ed),
			m_window(w),
			m_inputCManager(ril) {

		}

		virtual void graphicUpdate(DrawableContainer& drawables) override final {
			onGraphicUpdate(drawables);

			/* Graphics */
			for (auto& s : m_graphics) {
				s->setDrawables(drawables);
				s->update();
			}

			for (auto& s : m_subScenes) {
				s->graphicUpdate(drawables);
			}

			for (auto& s : m_linkedSubScenes) {
				s->graphicUpdate(drawables);
			}

		}

		virtual void eventUpdate(unsigned int ellapsedTime) override final {
			onEventUpdate(ellapsedTime);

			/* Logics */
			for (auto& s : m_logics) {
				s->update();
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

			for (auto& s : m_subScenes) {
				s->load(lastScene);
			}

			for (auto& s : m_linkedSubScenes) {
				s->load(lastScene);
			}

			afterLoad(lastScene);
		}

		bool unload() override final {
			auto result = !beforeUnload();
			for (auto& s : m_subScenes) {
				result &= !s->unload();
			}

			for (auto& s : m_linkedSubScenes) {
				result &= !s->unload();
			}
			const auto afterUnloadResult = afterUnload();
			return !(result && !afterUnloadResult);
		}

		void linkSubScene(Scene& subScene) {
			m_linkedSubScenes.insert(&subScene);
		}

		void unlinkSubScene(Scene& subScene) {
			m_linkedSubScenes.erase(&subScene);
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
		template<class SC>
		void transmitLinkedSubscenes(SC& scene) {
			m_linkedSubScenes = scene.m_linkedSubScenes;
		}

		std::vector<std::unique_ptr<ISystem>> m_logics;
		std::vector<std::unique_ptr<IGraphicSystem>> m_graphics;

		std::vector<std::unique_ptr<Scene>> m_subScenes;
		std::unordered_set<Scene*> m_linkedSubScenes;

	protected:
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

		virtual void onGraphicUpdate(DrawableContainer& drawables) {
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
