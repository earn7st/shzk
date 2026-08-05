#pragma once

namespace shzk
{
	class DrawBatch;

	class Renderable
	{
	public:
		virtual CollectDrawBatch(std::vector<DrawBatch>& drawBatches) = 0;
	};
}