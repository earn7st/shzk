#pragma once
#include "RenderPass.h"

#include <memory>

namespace shzk
{
	class MeshPassProcessor;

	class MeshPass : public RenderPass
	{
	public:

	private:
		std::shared_ptr<MeshPassProcessor> m_meshPassProcessor;
	};
}