#include "Editor.h"

#include "runtime/global/Engine.h"

namespace shzk
{
	void Editor::Init()
	{

	}

	void Editor::Run()
	{
		while (!Engine::ShouldClose())
		{
			Engine::Tick();
		}
	}
	
	void Editor::Shutdown()
	{
		Engine::Shutdown();
	}
}