#include <sstream>
#include "FormalCalculator.h"
#include <iostream>
#include "CalculOperators.h"
#include "../../Utils/StringUtils.h"
#include "..\..\Exceptions\ScriptSyntaxError.h"

using namespace std;

ska::FormalCalculator::FormalCalculator()
{
}

float ska::FormalCalculator::interpretFormalCalculation(const string& s)
{
	/*int parenthesisCount;
	unsigned int index;*/
	string subStr = s;
	size_t	frParenthesisPos,	// last Right parenthesis position
		lParenthesisPos;		// Left parenthesis that matches the first right one
	float result = 0.0F;

	frParenthesisPos = subStr.find_first_of(')');
	if (frParenthesisPos != string::npos)
	{
		lParenthesisPos = subStr.substr(0, frParenthesisPos - 1).find_last_of('(');

		if (lParenthesisPos != string::npos)
		{
			result = FormalCalculator::interpretFormalCalculation(subStr.substr(lParenthesisPos + 1, frParenthesisPos - lParenthesisPos - 1));
			stringstream ss;
			ss << result;
			result = FormalCalculator::interpretFormalCalculation(subStr.substr(0, lParenthesisPos) + " " + ss.str() + " " + subStr.substr(frParenthesisPos + 1, subStr.size() - 1));
		}
		else
		{
			FormalCalculator::calculSyntaxError(subStr);
			return 0.0F;
		}

	}
	else if (s.find_first_of('(') != string::npos)
	{
		FormalCalculator::calculSyntaxError(subStr);
		return 0.0F;
	}
	else
	{
		//On se trouve ici si la chaîne entrée ne contient plus de parenthèse
		result = FormalCalculator::interpretSimpleCalculation(subStr);

	}

	return result;
}

/*
ligne de calcul sans parenthèse
*/
float ska::FormalCalculator::interpretSimpleCalculation(const string& s)
{
	float result = 0.0F;
	string subStr = s;

	while (CalculOperators::containsOperator(subStr))
	{
		size_t firstDivision = subStr.find_first_of(CalculOperators::divise);
		size_t firstMultiply = subStr.find_first_of(CalculOperators::multiply);
		size_t operatorPos = string::npos;

		if (firstDivision != string::npos)
			operatorPos = firstDivision;
		else if (firstMultiply != string::npos)
			operatorPos = firstMultiply;
		else
		{
			size_t firstMinus = subStr.find_first_of(CalculOperators::minus);
			size_t firstPlus = subStr.find_first_of(CalculOperators::plus);
			if (firstMinus != string::npos)
				operatorPos = firstMinus;
			else if (firstPlus != string::npos)
				operatorPos = firstPlus;
		}

		result = FormalCalculator::interpretSingleCalculation(operatorPos, &subStr);
	}

	return result;
}

//ligne de calcul à une seule opération sans parenthèse
float ska::FormalCalculator::interpretSingleCalculation(size_t operatorPos, string* result)
{
	const string& s = *result;
	float r;
	int posStart = positionOfAnyLastOperator(s.substr(0, operatorPos));
	int posEnd = positionOfAnyFirstOperator(s.substr(operatorPos + 1, s.size() - 1)) + operatorPos;

	float op1 = ska::StringUtils::strToFloat(ska::StringUtils::trim(s.substr(posStart + 1, operatorPos)));
	float op2 = ska::StringUtils::strToFloat(ska::StringUtils::trim(s.substr(operatorPos + 1, posEnd - 1)));

	if (s[operatorPos] == CalculOperators::divise && abs(op1) < 0.0001 && abs(op2) < 0.0001)
	{
		calculSyntaxError(s);
		return 0.0;
	}

	r = CalculOperators::result(op1, op2, s[operatorPos]);

	stringstream ss;
	ss << r;
	*result = ss.str() + s.substr(posEnd + 1, s.size() - 1);

	return r;
}

int ska::FormalCalculator::positionOfAnyLastOperator(const string& s)
{
	int index;
	for (index = s.size() - 1; index >= 0
		&& s[index] != CalculOperators::divise
		&& s[index] != CalculOperators::multiply
		&& s[index] != CalculOperators::plus
		&& s[index] != CalculOperators::minus; index--);

	return index;
}

int ska::FormalCalculator::positionOfAnyFirstOperator(const string& s)
{
	size_t index;
	for (index = 0; index < s.size()
		&& s[index] != CalculOperators::divise
		&& s[index] != CalculOperators::multiply
		&& s[index] != CalculOperators::plus
		&& s[index] != CalculOperators::minus; index++);

	return index;
}

void ska::FormalCalculator::calculSyntaxError(const string& s)
{
	throw ska::ScriptSyntaxError("La ligne suivante est impossible à traduire en calcul formel : \"" + s + "\"");
}

ska::FormalCalculator::~FormalCalculator()
{
}
