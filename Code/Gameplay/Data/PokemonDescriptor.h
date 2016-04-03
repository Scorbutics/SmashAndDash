#ifndef DEF_POKEMONDESCRIPTOR
#define DEF_POKEMONDESCRIPTOR

#include <iostream>
#include "../../ska/Inputs/Readers/IniReader.h"


class PokemonDescriptor
{
	public:
	PokemonDescriptor();
	
	bool load(const ska::IniReader& data, const std::string& block);

	std::string getName();
	std::string getType(unsigned int number);
	std::string getDescription();
	char getCaptureRate();
	std::string getClass();

	void setName(std::string name);
	void setClass(std::string classe);
	void setType(unsigned int number, std::string type);
	void setDescription(std::string description);
	void setCaptureRate(char captureRate);
	
	~PokemonDescriptor();
	
	private:
	std::string m_name, m_class;
	std::string m_type1;
	std::string m_type2;
	std::string m_description;
	char m_captureRate;

};

#endif

