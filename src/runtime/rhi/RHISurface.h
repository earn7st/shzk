#pragma once

#include "runtime/rhi/RHIDefinitions.h"

namespace shzk
{
    struct RHISurfaceInfo
    {
        Extent2D extent;
    };

    class RHISurface
    {
    public:
        RHISurface() = delete;
        RHISurface(const RHISurfaceInfo& info) : m_info(info){}

    private:
        RHISurfaceInfo m_info;
    };
}