#include "Demangle.h"
#ifdef __GNUG__
#include <memory>
#include <cxxabi.h>
std::string ska::demangle(const std::string &s) {
	struct default_free { void operator ()(char *p) const { free(p); } };
	int r;
	std::unique_ptr<char, default_free> d(abi::__cxa_demangle(s.c_str(), 0, 0, &r));
	return r ? s : std::string(d.get());
}
#else
std::string ska::demangle(const std::string &s) { return s; }
#endif
