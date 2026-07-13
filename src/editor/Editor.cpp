#include "Editor.h"

#include "runtime/core/Engine.h"

namespace vkR
{
	void Editor::Initialize(Engine* engine)
	{
		m_engine = engine;
	}

	void Editor::Run()
	{
		while (!m_engine->ShouldClose())
		{
			m_engine->Tick();
		}
	}

	void Editor::Shutdown()
	{
		m_engine->Shutdown();
	}
}