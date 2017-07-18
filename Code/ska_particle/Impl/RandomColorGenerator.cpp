#include "RandomColorGenerator.h"
#include "../ParticleBuilder.h"
#include "../ParticleFactory.h"
#include "Utils/NumberUtils.h"

ska::RandomColorGenerator::RandomColorGenerator(Color min, Color max) :
    m_min(min),
	m_max(max) {
}

void ska::RandomColorGenerator::build(ParticleBuilder& builder) const {
	Color c1;
	c1.r = ska::NumberUtils::random(m_min.r, m_max.r);
	c1.g = ska::NumberUtils::random(m_min.g, m_max.g);
	c1.b = ska::NumberUtils::random(m_min.b, m_max.b);
	c1.a = 255;

	Color c2;
	c2.r = ska::NumberUtils::random(m_min.r, m_max.r);
	c2.g = ska::NumberUtils::random(m_min.g, m_max.g);
	c2.b = ska::NumberUtils::random(m_min.b, m_max.b);
	c2.a = 255;

	auto& cStart = brightestColor(c1, c2);
	auto& cEnd = (&cStart == &c1) ? c2 : c1;

	builder.setStartColor(cStart);
	builder.setEndColor(cEnd);
}

const ska::Color& ska::RandomColorGenerator::brightestColor(const Color& c1, const Color& c2) {
	auto average1 = (c1.r + c1.g + c1.b) / 3;
	auto average2 = (c2.r + c2.g + c2.b) / 3;

	return (average1 >= average2) ? c1 : c2;
}
