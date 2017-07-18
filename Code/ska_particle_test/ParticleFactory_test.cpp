#include <doctest.h>
#include "ParticleGroup.h"
#include "ParticleFactory.h"
#include "ParticleBuilder.h"

TEST_CASE("[ParticleFactory]") {
    ska::ParticleGroup group(10);
    ska::ParticleBuilder builder;
    ska::ParticleFactory factory(builder);

    factory.prepareNextParticles(group, 2, 0);
    ska::Point<int> pos(14, 14);
    builder.setPosition(pos);

    CHECK(group.pos[0] == pos);
    CHECK(group.pos[1] == pos);
    CHECK(group.pos[2] != pos);
}
