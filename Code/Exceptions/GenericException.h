#pragma once
#include <exception>
#include <string>

class GenericException : public std::exception
{
public:
	GenericException(std::string message) throw();
	const char* what() const throw();
	~GenericException() throw();

private:
	std::string m_message;
};

