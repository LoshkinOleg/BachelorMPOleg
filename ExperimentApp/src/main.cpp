#include "Application.h"

int main()
{
	auto app = new bsExp::Application(5175); // Allocated on heap due to structure size apparently exceeding stack.
	const auto result = app->RunProgram();
	delete app;
	return result;
}