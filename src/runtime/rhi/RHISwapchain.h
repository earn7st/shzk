#pragma once

namespace shzk
{
    struct RHISwapchainInfo
    {

    };

    class RHISwapchain
    {
    public:
        RHISwapchain() = delete;
        RHISwapchain(const RHISwapchainInfo& info) : m_info(info){}
    
    private:
        RHISwapchainInfo m_info;
    };
}