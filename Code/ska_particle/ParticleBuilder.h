#pragma once
#include "Point.h"
#include "ParticleBuilder.h"

namespace ska {
    class ParticleFactory;
    class ParticleGroup;
    struct Color;

    class ParticleBuilder {
        friend class ParticleFactory;
    public:
        ParticleBuilder();
        ~ParticleBuilder() = default;

        const ParticleBuilder& setAcceleration(Point<float> acceleration) const;
        const ParticleBuilder& setVelocity(PolarPoint<float> velocity, float spreading, unsigned int slices) const;
        const ParticleBuilder& setRandomPosition(Point<float> position, Point<int> maxDistance) const;
        const ParticleBuilder& setPosition(Point<float> position) const;
        const ParticleBuilder& setStartColor(const Color& c) const;
        const ParticleBuilder& setColor(const Color& c) const;
        const ParticleBuilder& setEndColor(const Color& c) const;

        bool isEmpty() const;

    private:
        void target(ParticleGroup& group, std::size_t particleIndexStart, std::size_t particleIndexEnd);

        ParticleGroup* m_group;
        std::size_t m_indexStart;
        std::size_t m_indexEnd;
    };
}
