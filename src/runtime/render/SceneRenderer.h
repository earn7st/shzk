#pragma once

namespace shzk
{
	class Scene;

	class SceneRenderer
	{
	public:
		void Process(const std::shared_ptr<Scene>& scene);
		void ProcessNode(const std::shared_ptr<Node>& node);
	};
}