#pragma once

#include <memory>

namespace shzk
{
	class Scene;
	class Node;

	class Editor
	{
	public:
		Editor() = default;
		~Editor() = default;

		void Init(std::shared_ptr<Scene> scene);
		void Run();
		void Shutdown();	

		static std::shared_ptr<Node> CreateDefaultCamera();

	};
}