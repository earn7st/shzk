#pragma once

namespace shzk
{
	class DrawBatch;

	class Drawable
	{
	public:
		virtual void CollectDrawBatch(std::vector<DrawBatch>& drawBatches) = 0;
	};
}