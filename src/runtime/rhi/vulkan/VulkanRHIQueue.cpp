#include "VulkanRHIQueue.h"

#include <volk/volk.h>

namespace shzk
{
	void VulkanRHIQueue::WaitIdle()
	{
		vkQueueWaitIdle(m_handle);
	}
}