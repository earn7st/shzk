
// main.cpp �ͷ
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <iostream>
#include "editor/Editor.h"
#include "runtime/global/Engine.h"

int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_CHECK_ALWAYS_DF);

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