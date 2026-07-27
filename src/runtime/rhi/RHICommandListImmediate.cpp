#include "RHICommandListImmediate.h"
#include "RHI.h"

namespace shzk
{
	std::shared_ptr<RHICommandListImmediate> RHICommandListImmediate::g_RhiCmdListImmediate = std::make_shared<RHICommandListImmediate>();

}
