#include "GenericException.h"


GenericException::GenericException(std::string message, std::string type) throw() {
	m_message = message;
	m_typedMessage = (type.empty() ? "" : (type + " : ")) + m_message;
}

const char* GenericException::what() const throw() {
	return m_typedMessage.c_str();
}

GenericException::~GenericException() throw()
{}
