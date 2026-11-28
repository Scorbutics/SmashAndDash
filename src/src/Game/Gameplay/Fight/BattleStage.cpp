#include "Game/__internalConfig/LoggerConfig.h"
#include "BattleStage.h"
#include <algorithm>
#include "Graphic/System/CameraSystem.h"

namespace {
	constexpr unsigned int DamageMarkLife = 900;
	constexpr unsigned int ImpactMarkLife = 220;
	constexpr unsigned int SlashMarkLife = 240;
	constexpr unsigned int TrailMarkLife = 180;
	constexpr unsigned int TextMarkLife = 1100;
	constexpr unsigned int WaveMarkLife = 350;
}

void BattleStage::update(unsigned int ellapsedTime) {
	m_phaseTime += ellapsedTime;
	m_bannerAge += ellapsedTime;
	if (!m_speech.empty()) {
		m_speechAge += ellapsedTime;
		if (m_speechAge >= m_speechDuration) {
			m_speech.clear();
		}
	}

	// An evolution : its light gathers, then the flash (its sounds on time)
	if (evolution.active) {
		const auto before = evolution.time;
		evolution.time += ellapsedTime;
		const auto crossed = [before, this](unsigned int at) { return before < at && evolution.time >= at; };
		if (crossed(1)) {
			sound(BattleSound::BondCharge);
		}
		if (crossed(BattleEvolution::Morph)) {
			sound(BattleSound::BondStart);
		}
		if (crossed(BattleEvolution::Flash)) {
			sound(BattleSound::BondRelease);
		}
		if (crossed(BattleEvolution::Reveal)) {
			sound(BattleSound::ClashShine);
			sound(BattleSound::BallCaught);
		}
	}

	// The feedbacks live in real time (a damage number rises during a hit stop)
	for (auto& mark : m_marks) {
		mark.age += ellapsedTime;
	}
	m_marks.erase(std::remove_if(m_marks.begin(), m_marks.end(), [](const BattleMark& mark) {
		auto life = ImpactMarkLife;
		switch (mark.kind) {
		case BattleMark::Kind::Damage: life = DamageMarkLife; break;
		case BattleMark::Kind::Slash: life = SlashMarkLife; break;
		case BattleMark::Kind::Trail: life = TrailMarkLife; break;
		case BattleMark::Kind::Text: life = TextMarkLife; break;
		case BattleMark::Kind::Wave: life = WaveMarkLife; break;
		case BattleMark::Kind::Ghost: life = GhostMarkLife; break;
		default: break;
		}
		return mark.age >= life;
	}), m_marks.end());

	if (m_freeze > 0) {
		m_freeze = m_freeze > ellapsedTime ? m_freeze - ellapsedTime : 0;
		m_delta = 0;
		return;
	}
	if (m_slow > 0) {
		m_slow = m_slow > ellapsedTime ? m_slow - ellapsedTime : 0;
		const auto scaled = static_cast<float>(ellapsedTime) * m_slowFactor + m_slowRemainder;
		m_delta = static_cast<unsigned int>(scaled);
		m_slowRemainder = scaled - static_cast<float>(m_delta);
		return;
	}
	m_delta = ellapsedTime;
}

void BattleStage::say(std::string text, unsigned int duration) {
	m_speech = std::move(text);
	m_speechAge = 0;
	m_speechDuration = duration;
}

void BattleStage::hitStop(unsigned int duration) {
	m_freeze = std::max(m_freeze, duration);
}

void BattleStage::slowMotion(float factor, unsigned int duration) {
	m_slowFactor = factor;
	m_slow = duration;
	m_slowRemainder = 0.F;
}

void BattleStage::changePhase(BattlePhase phase) {
	m_phase = phase;
	m_phaseTime = 0;
}

void BattleStage::announce(std::string text) {
	m_banner = std::move(text);
	m_bannerAge = 0;
}

void BattleStage::mark(BattleMark mark) {
	m_marks.push_back(std::move(mark));
}

void BattleStage::shake(unsigned int duration, int intensity) {
	if (m_camera != nullptr) {
		m_camera->shake(duration, intensity);
	}
}
