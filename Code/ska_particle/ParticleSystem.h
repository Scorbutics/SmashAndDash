#pragma once
#include <vector>
#include <memory>
#include "ParticleGroup.h"
#include "ParticleUpdater.h"
#include "ParticleRenderer.h"
#include "ParticleFactory.h"
#include "ParticleGenerator.h"

namespace ska {
	class ParticleSystem {
	public:
		explicit ParticleSystem(unsigned int density);

		template <class G, class ... Args>
		void addGenerator(Args&&... args) {
			m_generators.push_back(std::make_unique<G>(std::forward<Args>(args)...));
		}

		template <class U, class ... Args>
		void addUpdater(Args&&... args) {
			m_updaters.push_back(std::make_unique<U>(std::forward<Args>(args)...));
		}

		template <class R, class ... Args>
		void addRenderer(Args&&... args) {
			m_renderers.push_back(std::make_unique<R>(std::forward<Args>(args)...));
		}
		
		void refresh(unsigned int dt);
		void display() const;
		virtual ~ParticleSystem() = default;
	
	private:
		using ParticleUpdaterPtr = std::unique_ptr<ParticleUpdater>;
		using ParticleGeneratorPtr = std::unique_ptr<ParticleGenerator>;
		using ParticleRendererPtr = std::unique_ptr<ParticleRenderer>;

		unsigned int m_density;

		ParticleFactory m_factory;
		ParticleGroup m_group;
		std::vector<ParticleUpdaterPtr> m_updaters;
		std::vector<ParticleGeneratorPtr> m_generators;
		std::vector<ParticleRendererPtr> m_renderers;

	};
}
