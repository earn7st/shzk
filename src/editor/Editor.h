#pragma once

namespace vkR
{
	class Engine;

	class Editor
	{
	public:
		Editor() = default;
		~Editor() = default;

		void Initialize(Engine* engine);
		void Run();
		void Shutdown();

	private:	
		Engine* m_engine = nullptr;
	};
}
