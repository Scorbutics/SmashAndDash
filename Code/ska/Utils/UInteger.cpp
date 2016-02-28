#include "UInteger.h"

#include <sstream>

ska::UInteger::UInteger(unsigned int data)
{
	m_data = data;
}

ska::UInteger::UInteger()
{
	m_data = -1;
}

std::string ska::UInteger::toString() const
{
	std::ostringstream ss;
	ss << m_data;
	return ss.str();
}

unsigned int ska::UInteger::getData() const
{
	return m_data;
}

ska::UInteger::~UInteger()
{
}
