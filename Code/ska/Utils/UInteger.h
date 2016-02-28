#pragma once
#include "IStringAble.h"

#include <string>

namespace ska {
	class UInteger : public IStringAble
	{
	public:
		UInteger(unsigned int);
		UInteger();

		unsigned int getData() const;

		virtual std::string toString() const;
		virtual ~UInteger();

	private:
		unsigned int m_data;
	};
}
