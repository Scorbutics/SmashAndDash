#include "UInteger.h"

#include <sstream>

UInteger::UInteger(unsigned int data)
{
	m_data = data;
}

UInteger::UInteger()
{
	m_data = -1;
}

std::string UInteger::toString() const
{
	std::ostringstream ss;
	ss << m_data;
	return ss.str();
}

unsigned int UInteger::getData() const
{
	return m_data;
}

UInteger::~UInteger()
{
}
