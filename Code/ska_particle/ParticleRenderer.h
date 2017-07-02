#pragma once

namespace ska {
	class ParticleGroup;

	class ParticleRenderer {
	protected:
		ParticleRenderer() = default;
	public:
		virtual void display(const ParticleGroup& m_group) const = 0;
		virtual ~ParticleRenderer() = default;
	};
}