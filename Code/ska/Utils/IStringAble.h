#ifndef DEF_ISTRINGABLE
#define DEF_ISTRINGABLE

#include <string>

namespace ska {
	class IStringAble
	{
	public:
		virtual std::string toString() const = 0;
	};
}
#endif