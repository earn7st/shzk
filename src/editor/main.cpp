#include <iostream>

#include "editor/Editor.h"
#include "runtime/core/Engine.h"

int main()
{
	vkR::EngineSpec engineSpec;
	engineSpec.windowSpec.width = 1920;
	engineSpec.windowSpec.height = 1080;
	engineSpec.windowSpec.title = "vkR";

	vkR::Engine* engine = new vkR::Engine();
	engine->Initialize(engineSpec);

	vkR::Editor* editor = new vkR::Editor();
	editor->Initialize(engine);

	editor->Run();

	editor->Shutdown();
}