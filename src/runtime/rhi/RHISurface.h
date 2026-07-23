#pragma once

#include "runtime/rhi/RHIDefinitions.h"

namespace shzk
{
    class RHISurface
    {
    public:
        RHISurface() = default;
        inline Extent2D GetExetent() const { return m_extent; }

    protected:
        Extent2D m_extent;
    };
}