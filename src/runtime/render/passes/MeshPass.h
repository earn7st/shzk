#pragma once

#include <memory>

namespace shzk
{
	class MeshPassProcessor;

	class MeshPass
	{
	public:

	private:
		std::shared_ptr<MeshPassProcessor> m_meshPassProcessor;
	};
}