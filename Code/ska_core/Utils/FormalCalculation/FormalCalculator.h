#pragma once
#include <string>

namespace ska {
	class FormalCalculator
	{
	public:
		FormalCalculator();
		static float interpretFormalCalculation(const std::string& s);
		static float interpretSimpleCalculation(const std::string& s);
		static float interpretSingleCalculation(size_t operatorPos, std::string* result);
		static int positionOfAnyLastOperator(const std::string& s);
		static void calculSyntaxError(const std::string& s);
		static int positionOfAnyFirstOperator(const std::string& s);

		~FormalCalculator();
	};
}
