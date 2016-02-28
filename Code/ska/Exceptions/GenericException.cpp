#include "GenericException.h"


ska::GenericException::GenericException(std::string message, std::string type) throw() {
	m_message = message;
	m_typedMessage = (type.empty() ? "" : (type + " : ")) + m_message;
}

const char* ska::GenericException::what() const throw() {
	return m_typedMessage.c_str();
}

ska::GenericException::~GenericException() throw()
{}
