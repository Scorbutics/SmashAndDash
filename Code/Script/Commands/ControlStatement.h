#pragma once
#include "AbstractCommand.h"
class ControlStatement :
	public AbstractCommand
{
public:
	ControlStatement();
	virtual ~ControlStatement();

	static const std::string& getCommandEndIf();
	static const std::string& getCommandElsif();
	static const std::string& getCommandElse();
	static const std::string& getCommandIf();

protected:
	virtual bool process(const std::string& extendedName, std::stringstream& streamCmd, std::vector<std::string>& args, std::ofstream& scriptList, std::unordered_map<std::string, std::string>& varMap, std::ifstream& fscript, int& active, std::string* result) override;
	virtual char getSeparator() override;

	virtual bool analyzeLine(const std::string& extendedName, std::stringstream& streamCmd, std::vector<std::string>& args, std::ofstream& scriptList, std::unordered_map<std::string, std::string>& varMap, std::ifstream& fscript, int& active, std::string* result) = 0;
	virtual const std::string& getCmdName() = 0;

};
