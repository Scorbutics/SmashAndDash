#include <sstream>
#include "FormalCalculator.h"
#include <iostream>
#include "CalculOperators.h"
#include "../../Utils/StringUtils.h"
#include "../../Utils/NumberUtils.h"
#include "../../Exceptions/ScriptSyntaxError.h"

ska::FormalCalculator::FormalCalculator()
{
}

float ska::FormalCalculator::interpretFormalCalculation(const std::string& s)
{
	/*int parenthesisCount;
	unsigned int index;*/
	std::string subStr = s;
	size_t	frParenthesisPos,	// last Right parenthesis position
		lParenthesisPos;		// Left parenthesis that matches the first right one
	float result = 0.0F;

	frParenthesisPos = subStr.find_first_of(')');
	if (frParenthesisPos != std::string::npos)
	{
		lParenthesisPos = subStr.substr(0, frParenthesisPos - 1).find_last_of('(');

		if (lParenthesisPos != std::string::npos)
		{
			result = interpretFormalCalculation(subStr.substr(lParenthesisPos + 1, frParenthesisPos - lParenthesisPos - 1));
			std::stringstream ss;
			ss << result;
			result = interpretFormalCalculation(subStr.substr(0, lParenthesisPos) + " " + ss.str() + " " + subStr.substr(frParenthesisPos + 1, subStr.size() - 1));
		}
		else
		{
			calculSyntaxError(subStr);
			return 0.0F;
		}

	}
	else if (s.find_first_of('(') != std::string::npos)
	{
		calculSyntaxError(subStr);
		return 0.0F;
	}
	else
	{
		//On se trouve ici si la chaîne entrée ne contient plus de parenthèse
		result = interpretSimpleCalculation(subStr);

	}

	return result;
}

/*
ligne de calcul sans parenthèse
*/
float ska::FormalCalculator::interpretSimpleCalculation(const std::string& s)
{
	float result = 0.0F;
	std::string subStr = s;

	while (CalculOperators::containsOperator(subStr))
	{
		size_t firstDivision = subStr.find_first_of(CalculOperators::divise);
		size_t firstMultiply = subStr.find_first_of(CalculOperators::multiply);

		size_t operatorPos = std::string::npos;

		if (firstDivision != std::string::npos)
			operatorPos = firstDivision;
		else if (firstMultiply != std::string::npos)
			operatorPos = firstMultiply;
		else
		{
			size_t firstModulo = subStr.find_first_of(CalculOperators::modulo);
			if (firstModulo != std::string::npos) {
				operatorPos = firstModulo;
			} else {
				size_t firstMinus = subStr.find_first_of(CalculOperators::minus);
				size_t firstPlus = subStr.find_first_of(CalculOperators::plus);
				if (firstMinus != std::string::npos)
					operatorPos = firstMinus;
				else if (firstPlus != std::string::npos)
					operatorPos = firstPlus;
			}
		}

		result = interpretSingleCalculation(operatorPos, &subStr);
	}

	return result;
}

//ligne de calcul à une seule opération sans parenthèse
float ska::FormalCalculator::interpretSingleCalculation(size_t operatorPos, std::string* result)
{
	const std::string& s = *result;
	float r;
	size_t posStart = positionOfAnyLastOperator(s.substr(0, operatorPos));
	size_t posEnd = positionOfAnyFirstOperator(s.substr(operatorPos + 1, s.size() - 1)) + operatorPos;

	float op1 = StringUtils::strToFloat(StringUtils::trim(s.substr(posStart + 1, operatorPos)));
	float op2 = StringUtils::strToFloat(StringUtils::trim(s.substr(operatorPos + 1, posEnd - 1)));

	if ((s[operatorPos] == CalculOperators::divise || s[operatorPos] == CalculOperators::modulo) && NumberUtils::absolute(op2) < 0.0001) {
		calculSyntaxError(s);
		return 0.0;
	}

	r = CalculOperators::result(op1, op2, s[operatorPos]);

	std::stringstream ss;
	ss << r;
	*result = ss.str() + s.substr(posEnd + 1, s.size() - 1);

	return r;
}

int ska::FormalCalculator::positionOfAnyLastOperator(const std::string& s)
{
	int index;
	for (index = (int)(s.size() - 1); index >= 0
		&& s[index] != CalculOperators::divise
		&& s[index] != CalculOperators::multiply
		&& s[index] != CalculOperators::plus
		&& s[index] != CalculOperators::minus
		&& s[index] != CalculOperators::modulo; index--);

	return index;
}

int ska::FormalCalculator::positionOfAnyFirstOperator(const std::string& s) {
	size_t index;
	for (index = 0; index < s.size()
		&& s[index] != CalculOperators::divise
		&& s[index] != CalculOperators::multiply
		&& s[index] != CalculOperators::plus
		&& s[index] != CalculOperators::minus
		&& s[index] != CalculOperators::modulo; index++);

	return static_cast<int>(index);
}

void ska::FormalCalculator::calculSyntaxError(const std::string& s)
{
	throw ScriptSyntaxError("La ligne suivante est impossible à traduire en calcul formel : \"" + s + "\"");
}

ska::FormalCalculator::~FormalCalculator()
{
}
