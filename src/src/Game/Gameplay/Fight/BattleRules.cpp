#include "Game/__internalConfig/LoggerConfig.h"
#include "BattleRules.h"
#include <algorithm>
#include "Scripting/ScriptVM.h"

BattleRules::BattleRules(ska::scripting::ScriptVM& vm, std::string rulesFile, std::string aiFile, std::string statusFile) :
	m_vm(vm),
	m_rulesFile(std::move(rulesFile)),
	m_aiFile(std::move(aiFile)),
	m_statusFile(std::move(statusFile)) {
}

long BattleRules::call(const std::string& file, const std::string& function, std::vector<ska::NodeValue> arguments, long fallback) {
	try {
		return m_vm.call<long>(file, function, std::move(arguments));
	} catch (std::exception& e) {
		if (m_reported.insert(file + function).second) {
			SLOG_STATIC(ska::LogLevel::Error, BattleRules) << "Battle rule \"" << function << "\" (" << file << ") : " << e.what() << " : a default value is used";
		}
		return fallback;
	}
}

int BattleRules::stat(int base, int level) {
	return static_cast<int>(call(m_rulesFile, "stat", { ska::NodeValue{ static_cast<long>(base) }, ska::NodeValue{ static_cast<long>(level) } },
		base * level / 50 + 5));
}

int BattleRules::maxHp(int baseHp, int level) {
	return std::max(1, static_cast<int>(call(m_rulesFile, "maxHp", { ska::NodeValue{ static_cast<long>(baseHp) }, ska::NodeValue{ static_cast<long>(level) } },
		baseHp * level / 50 + level + 10)));
}

int BattleRules::level(int experience, int experienceNeeded) {
	return std::max(1, static_cast<int>(call(m_rulesFile, "level", { ska::NodeValue{ static_cast<long>(experience) }, ska::NodeValue{ static_cast<long>(experienceNeeded) } },
		1 + (experienceNeeded > 0 ? experience / experienceNeeded : 0))));
}

int BattleRules::experience(int dropped, int level) {
	return std::max(0, static_cast<int>(call(m_rulesFile, "experience", { ska::NodeValue{ static_cast<long>(dropped) }, ska::NodeValue{ static_cast<long>(level) } },
		dropped * level)));
}

int BattleRules::damage(int power, int attack, int defense, int level, int effectiveness, bool stab) {
	const auto fallback = power <= 0 || effectiveness <= 0 ? 0 : std::max(1, power * effectiveness / 100);
	return std::max(0, static_cast<int>(call(m_rulesFile, "damage", { ska::NodeValue{ static_cast<long>(power) }, ska::NodeValue{ static_cast<long>(attack) },
		ska::NodeValue{ static_cast<long>(defense) }, ska::NodeValue{ static_cast<long>(level) }, ska::NodeValue{ static_cast<long>(effectiveness) }, ska::NodeValue{ stab } }, fallback)));
}

int BattleRules::effectiveness(const std::string& skillType, const std::string& type1, const std::string& type2) {
	return std::max(0, static_cast<int>(call(m_rulesFile, "effectiveness", { ska::NodeValue{ std::make_shared<std::string>(skillType) }, ska::NodeValue{ std::make_shared<std::string>(type1) }, ska::NodeValue{ std::make_shared<std::string>(type2) } }, 100)));
}

bool BattleRules::dodge(int monster, int startupLeft, int hp, int maxHp) {
	return callBool(m_aiFile, "dodge", { ska::NodeValue{ static_cast<long>(monster) }, ska::NodeValue{ static_cast<long>(startupLeft) },
		ska::NodeValue{ static_cast<long>(hp) }, ska::NodeValue{ static_cast<long>(maxHp) } }, false);
}

bool BattleRules::callBool(const std::string& file, const std::string& function, std::vector<ska::NodeValue> arguments, bool fallback) {
	try {
		return m_vm.call<bool>(file, function, std::move(arguments));
	} catch (std::exception& e) {
		if (m_reported.insert(file + function).second) {
			SLOG_STATIC(ska::LogLevel::Error, BattleRules) << "Battle rule \"" << function << "\" (" << file << ") : " << e.what() << " : a default value is used";
		}
		return fallback;
	}
}

int BattleRules::decide(int monster, int distanceBlocks, int hp, int maxHp, const std::vector<bool>& ready) {
	auto readyValues = std::make_shared<ska::NodeValueArrayRaw>();
	for (const auto skillReady : ready) {
		readyValues->push_back(ska::NodeValue{ static_cast<bool>(skillReady) });
	}
	const auto firstReady = std::find(ready.begin(), ready.end(), true);
	const auto fallback = firstReady == ready.end() ? -1L : static_cast<long>(firstReady - ready.begin());
	const auto decision = call(m_aiFile, "decide", { ska::NodeValue{ static_cast<long>(monster) }, ska::NodeValue{ static_cast<long>(distanceBlocks) },
		ska::NodeValue{ static_cast<long>(hp) }, ska::NodeValue{ static_cast<long>(maxHp) }, ska::NodeValue{ ska::NodeValueArray{ std::move(readyValues) } } }, fallback);
	return decision >= 0 && decision < static_cast<long>(ready.size()) && ready[static_cast<std::size_t>(decision)] ? static_cast<int>(decision) : -1;
}

int BattleRules::move(int monster, int distance, int reach, int hp, int maxHp) {
	const auto fallback = reach > 0 && distance > reach ? 1L : 0L;
	const auto movement = call(m_aiFile, "move", { ska::NodeValue{ static_cast<long>(monster) }, ska::NodeValue{ static_cast<long>(distance) },
		ska::NodeValue{ static_cast<long>(reach) }, ska::NodeValue{ static_cast<long>(hp) }, ska::NodeValue{ static_cast<long>(maxHp) } }, fallback);
	return movement >= 0 && movement <= 4 ? static_cast<int>(movement) : 0;
}

namespace {
	ska::NodeValue Text(const std::string& text) {
		return ska::NodeValue{ std::make_shared<std::string>(text) };
	}
}

bool BattleRules::statusLands(const std::string& status, int chance, int roll, const std::string& current) {
	const auto fallback = !status.empty() && status != "guard" && roll < chance && (status == "flinch" || current == "none");
	return callBool(m_statusFile, "lands", { Text(status), ska::NodeValue{ static_cast<long>(chance) }, ska::NodeValue{ static_cast<long>(roll) }, Text(current) }, fallback);
}

unsigned int BattleRules::statusDuration(const std::string& status, unsigned int effect) {
	const auto fallback = status == "sleep" ? std::min(effect, 3500u) : effect;
	return static_cast<unsigned int>(std::max(0L, call(m_statusFile, "duration", { Text(status), ska::NodeValue{ static_cast<long>(effect) } }, static_cast<long>(fallback))));
}

unsigned int BattleRules::statusStun(const std::string& status) {
	const auto fallback = status == "flinch" ? 450L : (status == "paralysis" ? 500L : 0L);
	return static_cast<unsigned int>(std::max(0L, call(m_statusFile, "stun", { Text(status) }, fallback)));
}

unsigned int BattleRules::statusTickPeriod(const std::string& status) {
	const auto fallback = status == "poison" || status == "burn" || status == "leech" ? 1000L : 0L;
	return static_cast<unsigned int>(std::max(0L, call(m_statusFile, "tickPeriod", { Text(status) }, fallback)));
}

int BattleRules::statusTickDamage(const std::string& status, int maxHp) {
	const auto fallback = status == "poison" || status == "burn" || status == "leech" ? std::max(1, maxHp / 16) : 0;
	return static_cast<int>(std::max(0L, call(m_statusFile, "tickDamage", { Text(status), ska::NodeValue{ static_cast<long>(maxHp) } }, fallback)));
}

bool BattleRules::statusLeech(const std::string& status) {
	return callBool(m_statusFile, "leech", { Text(status) }, status == "leech");
}

int BattleRules::statusSpeedPercent(const std::string& status) {
	return static_cast<int>(std::max(0L, call(m_statusFile, "speedPercent", { Text(status) }, status == "paralysis" ? 60L : 100L)));
}

int BattleRules::statusFailOdds(const std::string& status) {
	return static_cast<int>(std::max(0L, call(m_statusFile, "failOdds", { Text(status) }, status == "paralysis" ? 4L : 0L)));
}

int BattleRules::statusAttackPercent(const std::string& status, bool special) {
	return static_cast<int>(std::max(0L, call(m_statusFile, "attackPercent", { Text(status), ska::NodeValue{ special } }, status == "burn" && !special ? 50L : 100L)));
}

bool BattleRules::statusWakes(const std::string& status, int damage) {
	return callBool(m_statusFile, "wakes", { Text(status), ska::NodeValue{ static_cast<long>(damage) } }, status == "sleep" && damage > 0);
}
