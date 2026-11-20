# RHI 层架构设计（ToyRenderer / UE 风格）

参考 UE 的三层 RHI 架构：

```
上层渲染代码
    │
    ├─ RHI::Get()->CreateBuffer(info)      ← 工厂，创建 RHI 对象
    ├─ RHICommandList::Get()->BeginCommand() ← 胶水，bypass 或延迟入队
    │       │
    │       └─ RHICommandContext::RHIBeginCommand()  ← 平台实现，录制 VkCmd
    │
    └─ RHI::Get()->Destroy()              ← 全局销毁
```

## RHI（= UE FDynamicRHI）

平台无关的抽象工厂，全局单例 `RHI::Get()`（返回 `std::shared_ptr<RHI>&`）。
**所有与 CommandList 无关的资源创建都走这里。**

**VulkanRHI** 继承 `RHI`，实现所有纯虚工厂方法。内部持有全局 VK 状态（`m_instance`/`m_vkbInstance`/`m_physicalDevice`/`m_vkbPhysicalDevice`/`m_device`/`m_allocator`/`m_debugMessenger`/`m_properties`/`m_features`/`m_memoryProperties`/`m_supportedExtensions`/`m_queueFamilyProperties`/`m_queueIndices`/`m_queueCounts`/`m_queues`/`m_descriptorPool`）。
volk 初始化（`volkInitialize` → `volkLoadInstance` → `volkLoadDevice`）和 VMA 创建在 `VulkanRHI` 构造中完成，这些都是私有实现细节，上层完全不可见。

## RHIResource（所有 GPU 资源的基类）

- 所有 GPU RHI 类型（Buffer/Texture/TextureView/Sampler/Shader、RootSignature/DescriptorSet、RenderPass/GraphicsPipeline）继承 `RHIResource`
- 每个具体 RHI 类型：持有对应的 `*Info`（以 `m_info` 命名）+ `GetInfo()` 查询接口；Vulkan 实现中持有真实 VK 句柄；`Destroy()` 为 protected，由 RHI 系统或 owning 对象调用
- **非 RHIResource 的 RHI 对象**（`RHICommandContext`、`RHICommandPool`、`RHISemaphore`、`RHIFence` 等）：基类声明 `virtual void Destroy() = 0;` 为 **public**，Vulkan 实现用 `override final` 完成实际销毁

## RHICommandContext（= UE IRHICommandContext）

平台相关的抽象命令录制接口。Vulkan 实现持有 `VkCommandBuffer` 和对应的 `RHICommandPool` 引用。
**活跃方法使用 `RHI` 前缀**（UE 风格），方便区分 Context 方法和 CommandList 胶水方法。

**RHICommandContextImmediate**：单独的命令上下文，用于非渲染循环的立即执行（如资产上传），通过 `RHI::Get()->GetCommandContextImmediate()` 直接访问（`Texture::LoadFromFile`、`VertexBuffer`/`IndexBuffer` 上传都用它）。

## RHICommandList（胶水层）

**不是虚类**。包装 `RHICommandContext*`，提供与 RHICommandContext 对应的胶水方法。

两种模式：
- **bypass = true**（当前实现）：直接调用 `m_cmdContext->RHI*()`
- **bypass = false**（远期）：将调用参数打包成 `RHICommand*` 结构体入队，Execute 时统一回放

Immediate 命令（资产上传）**不经过 RHICommandList 胶水层**，直接通过 `RHI::Get()->GetCommandContextImmediate()` 拿 `RHICommandContextImmediate*` 调用。
（旧版的 `RHICommandListImmediate` 胶水类已移除，`RHICommandContextImmediate` 定义在 `RHI.h` 内联。）

## PerFrameRHIResource — 帧资源管理

`RenderSystem` 维护 `FRAMES_IN_FLIGHT`（= 2）个 PerFrameRHIResource（cmdContext / startSemaphore / endSemaphore / fence），不做 Context 池化。
单线程 bypass 模式下，2 个 Context 足够循环复用；fence 保证 GPU 先完成上一轮才能 reset CommandBuffer。后期若拆分 RHI 线程且需要并行翻译，才需要池化。

## RHICommandPool — VkCommandPool 薄封装

`VulkanRHICommandPool` 内部持有 `VkCommandPool`（带 `VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT`），`CreateCommandContext()` 调 `vkAllocateCommandBuffers` 分配 VkCommandBuffer 然后构造 `VulkanRHICommandContext`。
**不做 Context 池化**（无 idleContexts/ReturnToPool）。Context 数量 = `FRAMES_IN_FLIGHT`，用完循环。
`enable_shared_from_this` 用于 `VulkanRHICommandContext` 能安全引用其 owning pool。

## RHI-native 类型

底层图形 API 完全被屏蔽——上层代码不包含 `<vulkan/vulkan.h>`，不知道 VK 的存在。
RHI → VK 的枚举转换函数（`VulkanUtil.h`）仅在 `rhi/vulkan/` 内部使用。

核心类型（都在 `RHIDefinitions.h` 中）：
- **RHIFormat** — `FORMAT_R8G8B8A8_SRGB`, `FORMAT_D32_SFLOAT` 等，映射到 VkFormat
- **RHIResourceState** — `RESOURCE_STATE_UNDEFINED`, `RESOURCE_STATE_COLOR_ATTACHMENT`, `RESOURCE_STATE_PRESENT` 等
- **MemoryUsage** — `MEMORY_USAGE_GPU_ONLY`, `MEMORY_USAGE_CPU_TO_GPU` 等
- **ResourceType** (bitmask) — `RESOURCE_TYPE_TEXTURE`, `RESOURCE_TYPE_VERTEX_BUFFER`, `RESOURCE_TYPE_UNIFORM_BUFFER` 等
- **RasterizerStateInfo**, **DepthStencilStateInfo**, **BlendStateInfo** — PSO 状态结构体，支持 `operator==` 用于 hash 缓存
- **ShaderFrequency** — `SHADER_FREQUENCY_VERTEX`, `SHADER_FREQUENCY_FRAGMENT`, `SHADER_FREQUENCY_COMPUTE`
- **Extent2D / Extent3D / Offset2D** — 平台无关的尺寸/偏移类型
- **TextureSubresourceRange / TextureSubresourceLayers** — 子资源描述
- 所有 `*Info` 结构体 — `RHIBufferInfo`, `RHITextureInfo`, `RHIShaderInfo`, `RHIGraphicsPipelineInfo` 等

> 注意：`RHIDefinitions.h` 已包含全部类型定义。旧版 CLAUDE.md 提到的 `RHIStructs.h` 不再需要——该文件从未创建，所有 enum/struct/Info 都定义在 `RHIDefinitions.h` 中，规模已足够。

VK 枚举 → RHI 枚举的转换函数（在 `VulkanUtil.h` 中）：
```cpp
VkFormat RHIFormatToVkFormat(RHIFormat format);
RHIFormat VkFormatToRHIFormat(VkFormat vkFormat);
VkImageLayout ResourceStateToVkImageLayout(RHIResourceState state);
VkAccessFlags ResourceStateToVkAccessFlags(RHIResourceState state);
VkPipelineStageFlags VkAccessFlagsToVkPipelineStageFlags(VkAccessFlags accessFlags);
```
