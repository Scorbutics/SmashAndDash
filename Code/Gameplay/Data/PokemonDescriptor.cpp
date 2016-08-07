#include "PokemonDescriptor.h"
#include "../../ska/Inputs\Readers\IniReader.h"

PokemonDescriptor::PokemonDescriptor()
{
	m_name = "mob";
	m_type1 = "Aucun";
	m_type2 = "Aucun";
	m_class = "mob";
	m_description = "Aucune";
	m_captureRate = 0;
}

std::string PokemonDescriptor::getName()
{
	return m_name;
}

std::string PokemonDescriptor::getType(unsigned int number)
{
	if(number == 1)
		return m_type1;
	else
		return m_type2;
}

std::string PokemonDescriptor::getDescription()
{
	return m_description;
}

char PokemonDescriptor::getCaptureRate()
{
	return m_captureRate;
}

void PokemonDescriptor::setName(std::string name)
{
	m_name = name;
}

void PokemonDescriptor::setType(unsigned int number, std::string type)
{
	if(number == 1)
		m_type1 = type;
	else if(number == 2)
		m_type2 = type;
}

void PokemonDescriptor::setDescription(std::string description)
{
	m_description = description;
}

void PokemonDescriptor::setCaptureRate(char captureRate)
{
	m_captureRate = captureRate;
}

void PokemonDescriptor::setClass( std::string classe )
{
	m_class = classe;
}

std::string PokemonDescriptor::getClass()
{
	return m_class;
}

bool PokemonDescriptor::load(const ska::IniReader& data, const std::string& block)
{
	if(data.isLoaded()) {
		m_name = data.get<std::string>(block + " name");
		m_type1 = data.get<std::string>(block + " type1");
		m_type2 = data.get<std::string>(block + " type2");
		m_class = data.get<std::string>(block + " class");
		m_description = data.get<std::string>(block + " pokedex");
		m_captureRate = (char) data.get<int>(block + " capture_rate");

		return true;
	}


	return false;
}

PokemonDescriptor::~PokemonDescriptor()
{}

