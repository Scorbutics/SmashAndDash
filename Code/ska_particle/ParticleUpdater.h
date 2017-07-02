#pragma once

namespace ska {
	class ParticleGroup;

	class ParticleUpdater {
	protected:
		ParticleUpdater() = default;

	public:
		virtual void update(unsigned int dt, ParticleGroup& group) const = 0;
		virtual ~ParticleUpdater() = default;
	};
}
