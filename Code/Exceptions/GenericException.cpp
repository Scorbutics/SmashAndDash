#include "GenericException.h"


GenericException::GenericException(std::string message) throw() {
	m_message = message;
}

const char* GenericException::what() const throw()
{
	return m_message.c_str();
}


GenericException::~GenericException() throw()
{}
