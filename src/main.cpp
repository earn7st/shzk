#include "application/Application.h"

int main(int argc, char* argv[])
{
	Application application("vkR");

	application.Init();

	application.Run();

	application.Shutdown();

	return 0;
}
