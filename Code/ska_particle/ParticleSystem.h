#pragma once
#include <vector>
#include <memory>

namespace ska {
    class ParticleEffect;

	class ParticleSystem {
	public:
		ParticleSystem() = default;

		template <class ParticleEffectFactory, class ... Args>
		ParticleEffect& makeEffect(Args&&... args) {
			auto item = ParticleEffectFactory::createEffect(std::forward<Args>(args)...);
			auto& result = *item.get();
			m_effects.push_back(std::move(item));
			return result;
		}

		void refresh(unsigned int dt) {
			for(auto& e : m_effects) {
				e->refresh(dt);
			}
		}

		void display() const {
			for (const auto& e : m_effects) {
				e->display();
			}
		}

		virtual ~ParticleSystem() = default;

	private:
		std::vector<std::unique_ptr<ParticleEffect>> m_effects;

	};
}
