#ifndef DEF_CALCULOPERATORS
#define DEF_CALCULOPERATORS

struct CalculOperators
{
	static const char plus = '+';
	static const char minus = '-';
	static const char divise = '/';
	static const char multiply = '*';

	static bool isOperator(char c)
	{
		return (c == plus || c == minus || c == divise || c == multiply);
	}

	static float result(float op1, float op2, char calcOperator)
	{
		if (calcOperator == plus)
			return op1 + op2;
		else if (calcOperator == divise)
			return op1 / op2;
		else if (calcOperator == multiply)
			return op1 * op2;
		else if (calcOperator == minus)
			return op1 - op2;

		return 0.0F;
	}

	static bool containsOperator(const std::string& s)
	{
		unsigned int i;
		for (i = 0; i < s.size() && !isOperator(s[i]); i++);
		return (i != s.size() && isOperator(s[i]));
	}
};

#endif