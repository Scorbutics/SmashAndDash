#pragma once
namespace ska {
	enum EnumContextManager {
		CONTEXT_MAP,
		CONTEXT_GUI
	};
}

namespace std {
	template<>
	struct hash<ska::EnumContextManager> {
		std::size_t operator()(const ska::EnumContextManager& k) const {
	    	return k;
	    }

	};
}
