#pragma once
#include <exception>
#include <string>

namespace ska {
	class GenericException : public std::exception {
	public:
		GenericException(std::string message, std::string type) throw();
		const char* what() const throw() override;
		virtual ~GenericException() throw();

	private:
		std::string m_message, m_typedMessage;
	};

}
