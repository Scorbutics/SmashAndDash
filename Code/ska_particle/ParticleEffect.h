#pragma once
#include <vector>
#include <memory>
#include "ParticleGroup.h"
#include "ParticleBuilder.h"
#include "ParticleFactory.h"

namespace ska {
    class ParticleUpdater;
    class ParticleRenderer;
    class ParticleGenerator;

	class ParticleEffect {
	public:
		ParticleEffect(unsigned int density, std::size_t systemSize);

		template <class GroupProperty, class ... Args>
		void applyGroupProperty(Args&&... args) {
			GroupProperty::applyProperty(m_group, std::forward<Args>(args)...);
		}

		template <class Generator, class ... Args>
		Generator& addGenerator(Args&&... args) {
			auto item = std::make_unique<Generator>(std::forward<Args>(args)...);
			auto& result = *item.get();
			m_generators.push_back(std::move(item));
			return result;
		}

		template <class Updater, class ... Args>
		Updater& addUpdater(Args&&... args) {
			auto item = std::make_unique<Updater>(std::forward<Args>(args)...);
			auto& result = *item.get();
			m_updaters.push_back(std::move(item));
			return result;
		}

		template <class Renderer, class ... Args>
		Renderer& addRenderer(Args&&... args) {
			auto item = std::make_unique<Renderer>(std::forward<Args>(args)...);
			auto& result = *item.get();
			m_renderers.push_back(std::move(item));
			return result;
		}

		void refresh(unsigned int dt);
		void display() const;
		virtual ~ParticleEffect() = default;


	private:
	    void generate(unsigned int dt);
	    void update(unsigned int dt);

		using ParticleUpdaterPtr = std::unique_ptr<ParticleUpdater>;
		using ParticleGeneratorPtr = std::unique_ptr<ParticleGenerator>;
		using ParticleRendererPtr = std::unique_ptr<ParticleRenderer>;

		unsigned int m_density;

		ParticleBuilder m_builder;
		ParticleFactory m_factory;
		ParticleGroup m_group;
		std::vector<ParticleUpdaterPtr> m_updaters;
		std::vector<ParticleGeneratorPtr> m_generators;
		std::vector<ParticleRendererPtr> m_renderers;
		unsigned int m_particleDimension;

	};
}
