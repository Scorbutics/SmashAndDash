#ifndef DEF_ISTRINGABLE
#define DEF_ISTRINGABLE

#include <string>

class IStringAble
{
	public:
		virtual std::string toString() const = 0;
};

#endif