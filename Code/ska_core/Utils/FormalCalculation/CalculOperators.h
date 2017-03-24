#ifndef DEF_CALCULOPERATORS
#define DEF_CALCULOPERATORS

namespace ska {
	struct CalculOperators
	{
		static const char plus = '+';
		static const char minus = '-';
		static const char divise = '/';
		static const char multiply = '*';
		static const char modulo = ':';

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

			return (float)((int)op1 % (int)op2);
		}

		static bool containsOperator(const std::string& s)
		{
			unsigned int i;
			for (i = 0; i < s.size() && !isOperator(s[i]); i++);
			return (i != s.size() && isOperator(s[i]));
		}
	};
}
#endif
