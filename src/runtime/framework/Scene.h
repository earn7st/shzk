#pragma once

namespace vkR
{
	class Scene
	{
	public:
		Scene() = default;
		~Scene() = default;

		void Initialize();
		void Tick();
		void Clear();

	private:

		std::vector<Node> nodes;
	};
}