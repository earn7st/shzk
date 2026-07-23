#include <iostream>

#include "editor/Editor.h"
#include "runtime/global/Engine.h"

int main()
{
	shzk::EngineInitInfo engineInitInfo{};
	engineInitInfo.name = "shzk";
	engineInitInfo.width = 1280;
	engineInitInfo.height = 720;

	shzk::Engine::Init(engineInitInfo);

	shzk::Editor* editor = new shzk::Editor();
	editor->Run();
	editor->Shutdown();

	return 0;
}