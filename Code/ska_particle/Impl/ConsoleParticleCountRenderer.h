#include "../ParticleRenderer.h"

namespace ska {
	class ConsoleParticleCountRenderer :
		public
		ska::ParticleRenderer {
	public:
		explicit ConsoleParticleCountRenderer(unsigned int frequency);
		virtual ~ConsoleParticleCountRenderer() = default;

		void display(const ParticleGroup& group) const override;
	private:
		const unsigned int m_frequency;

	};
}