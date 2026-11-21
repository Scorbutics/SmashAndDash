#include "Game/__internalConfig/LoggerConfig.h"
#include "BattleRules.h"
#include <algorithm>
#include "Scripting/ScriptVM.h"

BattleRules::BattleRules(ska::scripting::ScriptVM& vm, std::string rulesFile, std::string aiFile) :
	m_vm(vm),
	m_rulesFile(std::move(rulesFile)),
	m_aiFile(std::move(aiFile)) {
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

int BattleRules::damage(int power, int attack, int defense, int level) {
	return std::max(0, static_cast<int>(call(m_rulesFile, "damage", { ska::NodeValue{ static_cast<long>(power) }, ska::NodeValue{ static_cast<long>(attack) },
		ska::NodeValue{ static_cast<long>(defense) }, ska::NodeValue{ static_cast<long>(level) } }, power)));
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
