#include "application/Application.h"

int main(int argc, char* argv[])
{
	Application application;

	application.Initialize("vkR");

	application.Run();

	application.Shutdown();

	return 0;
}
