# SHZK — Vulkan Renderer

C++20 Vulkan 1.3 实时渲染器。参考 Unreal Engine 和 ToyRenderer (可在父目录/ToyRenderer下找到) 的架构。
RHI 层设计参考 ToyRenderer（UE 风格：FDynamicRHI + IRHICommandContext + FRenderResource）。
最后更新：2026-07-25

---

## ★ 当前阶段目标（2026-07-23）

### 核心任务：RHI 层按 ToyRenderer/UE 模式重新设计

当前 `RHI` 只是一个空壳工厂（无虚接口，无资源创建方法），`VulkanRHI` 混入了所有 VK 变量的创建和管理，上层拿不到任何 RHI 资源。
需要按 ToyRenderer 的 UE 风格重新拆分：

1. **RHI（≈ UE FDynamicRHI）** — 平台无关的抽象工厂，纯虚接口，负责所有 RHI 资源的创建，全局单例 `RHI::Get()`
2. **RHICommandContext（≈ UE IRHICommandContext）** — 平台相关的命令录制，缓存状态上下文，延迟生成 VK 指令
3. **RHICommandList** — 胶水层，非虚类，bypass 模式直接录制，deferred 模式入队 Command 结构体延迟执行
4. **RHIResource** — 所有 RHI 对象的基类，带 `RHIResourceType` 枚举，统一资源追踪
5. **Vulkan 实现层** — `vulkan/VulkanRHI*` 对应实现，平台无关枚举 → VK 枚举的映射仅在 vulkan/ 内部使用

### 后续 Roadmap（RHI 完成后恢复）

```
Resource → Asset → GltfImporter → Pipeline → Descriptor → Camera → Forward Pass
```

---

## 目录结构

```
SHZK/
├── CMakeLists.txt          # 根构建：Vulkan SDK、SDL3、shader 编译
├── shaders/                # GLSL 源码 → bin/shaders/*.spv
├── assets/                 # glTF 模型（DamagedHelmet 等）
├── extern/                 # 第三方库: fmt, vk-bootstrap, imgui, fastgltf, volk, VMA
└── src/
    ├── CMakeLists.txt      # shzk_runtime (库) + shzk_editor (可执行文件)
    ├── editor/             # Editor 入口
    │   ├── Editor.h/.cpp   # 编辑器主循环
    │   └── main.cpp        # WinMain
    └── runtime/            # 引擎 Runtime
        ├── core/           # Engine.h/.cpp, WindowSystem.h/.cpp
        ├── render/         # RenderSystem.h/.cpp
        ├── rendering/      # ⚠ 旧渲染代码（未编译，待删除）
        ├── rhi/            # RHI 封装 — UE 风格平台无关抽象
        │   ├── RHI.h/.cpp                # 抽象工厂 + 全局单例（FDynamicRHI）
        │   ├── RHIResource.h/.cpp        # RHI 资源基类 + 所有具体 RHI 类型声明
        │   ├── RHICommandContext.h/.cpp   # 抽象命令录制接口（IRHICommandContext）
        │   ├── RHICommandList.h/.cpp      # 胶水层 — bypass/deferred 两种模式
        │   ├── RHIStructs.h              # 所有平台无关的 enum/struct/Info 类型
        │   ├── RHIDefinitions.h          # 基础类型（Extent2D, RHIResourceType 等）
        │   ├── RHISurface.h              # Surface 接口
        │   ├── RHISwapchain.h            # Swapchain 接口
        │   └── vulkan/
        │       ├── VulkanRHI.h/.cpp              # 工厂实现 + 全局 VK 状态
        │       ├── VulkanRHIResource.h/.cpp      # Vulkan 资源实现（VkBuffer, VkImage, ...）
        │       ├── VulkanRHICommandContext.h/.cpp # Vulkan 命令录制
        │       ├── VulkanRHISurface.h/.cpp        # Vulkan Surface 实现
        │       └── VulkanUtils.h                 # VK_CHECK + RHI→VK 转换函数
        ├── resource/       # ★ RenderResource 层 — 对 RHI 的渲染向封装
        │   ├── GpuGeometry.h/.cpp            # vertexBuffer + indexBuffer
        │   ├── GpuTexture.h/.cpp             # image + imageView + sampler
        │   └── GpuMaterial.h/.cpp            # UBO + DescriptorSet
        ├── asset/          # ★ Asset 层 — 引擎资产（身份、元数据、加载状态）
        │   ├── Asset.h                  # AssetId, AssetType, name, sourcePath, loadState
        │   ├── MeshAsset.h/.cpp         # SubMesh 列表
        │   ├── MaterialAsset.h/.cpp     # PBR 参数 + 纹理引用
        │   ├── TextureAsset.h/.cpp      # 格式/尺寸元数据
        │   └── AssetRegistry.h/.cpp     # 全局 map<AssetId, unique_ptr<Asset>>
        ├── import/         # glTF/glb 导入器
        │   └── GltfImporter.h/.cpp
        └── framework/      # ECS 骨架
```

---

## 命名约定

### 类命名

| 层级 | 前缀 | 示例 | 说明 |
|---|---|---|---|
| RHI 抽象 | `RHI*` | `RHI`, `RHIResource`, `RHISurface`, `RHISwapchain` | 平台无关接口/基类 |
| RHI Vulkan | `VulkanRHI*` | `VulkanRHI`, `VulkanRHISurface`, `VulkanRHIBuffer` | Vulkan 具体实现 |
| Info 结构体 | `*Info` | `RHIInfo`, `RHISurfaceInfo`, `RHIBufferInfo` | 构造参数 |
| 类型别名 | `*Ref` | `RHIBufferRef` = `std::shared_ptr<RHIBuffer>` | shared_ptr 引用 |

### 成员变量

- `m_` 前缀：`m_instance`, `m_device`, `m_allocator`, `m_rhiInfo`, `m_resourceType`
- 静态单例：`RHI::rhi`

### 命名空间

- 统一使用 `shzk`（小写）
- ⚠ `RHIResource.h` 当前错误使用了 `SHZK`（大写），需要修复

### 构建目标

- `shzk_runtime`（静态库，小写）
- `shzk_editor`（可执行文件）

---

## RHI 架构设计（ToyRenderer / UE 风格）

### 核心概念

参考 UE 的三层 RHI 架构：

```
上层渲染代码
    │
    ├─ RHI::CreateBuffer(info)           ← 工厂，创建 RHI 对象
    ├─ RHICommandList::DrawIndexed(...)  ← 胶水，bypass 或延迟入队
    │       │
    │       └─ RHICommandContext::DrawIndexed(...)  ← 平台实现，录制 VkCmd
    │
    └─ RHI::Tick()                      ← GC 无引用且长时间未用的资源
```

### 1. RHI（= UE FDynamicRHI）

平台无关的抽象工厂，全局单例 `RHI::Get()`（返回 `std::shared_ptr<RHI>`）。
**所有与 CommandList 无关的资源创建都走这里。**

当前 `RHI` 只有 `Init/Shutdown/Get` 三个静态方法和 `m_rhiInfo` 成员——需要扩展为完整的抽象工厂：

```cpp
class RHI {
public:
    static std::shared_ptr<RHI> Init(const RHIInfo& info);
    static void Shutdown();
    static std::shared_ptr<RHI> Get() { return rhi; }

    virtual void Tick();  // 资源 GC

    // 基本资源
    virtual std::shared_ptr<RHIQueue> GetQueue(const RHIQueueInfo& info) = 0;
    virtual std::shared_ptr<RHISurface> CreateSurface(void* windowHandle) = 0;
    virtual std::shared_ptr<RHISwapchain> CreateSwapChain(const RHISwapchainInfo& info) = 0;
    virtual std::shared_ptr<RHICommandPool> CreateCommandPool(const RHICommandPoolInfo& info) = 0;
    virtual std::shared_ptr<RHICommandContext> CreateCommandContext(
        std::shared_ptr<RHICommandPool> pool) = 0;

    // GPU 资源（工厂方法）
    virtual std::shared_ptr<RHIBuffer> CreateBuffer(const RHIBufferInfo& info) = 0;
    virtual std::shared_ptr<RHITexture> CreateTexture(const RHITextureInfo& info) = 0;
    virtual std::shared_ptr<RHITextureView> CreateTextureView(const RHITextureViewInfo& info) = 0;
    virtual std::shared_ptr<RHISampler> CreateSampler(const RHISamplerInfo& info) = 0;
    virtual std::shared_ptr<RHIShader> CreateShader(const RHIShaderInfo& info) = 0;

    // Pipeline / RootSignature
    virtual std::shared_ptr<RHIRootSignature> CreateRootSignature(
        const RHIRootSignatureInfo& info) = 0;
    virtual std::shared_ptr<RHIRenderPass> CreateRenderPass(const RHIRenderPassInfo& info) = 0;
    virtual std::shared_ptr<RHIGraphicsPipeline> CreateGraphicsPipeline(
        const RHIGraphicsPipelineInfo& info) = 0;
    virtual std::shared_ptr<RHIComputePipeline> CreateComputePipeline(
        const RHIComputePipelineInfo& info) = 0;

    // 同步
    virtual std::shared_ptr<RHIFence> CreateFence(bool signaled) = 0;
    virtual std::shared_ptr<RHISemaphore> CreateSemaphore() = 0;

    // 立即命令
    virtual std::shared_ptr<RHICommandContextImmediate> GetImmediateCommand() = 0;

protected:
    RHI() = delete;
    RHI(const RHIInfo& info) : m_rhiInfo(info) {}

    void RegisterResource(std::shared_ptr<RHIResource> resource);
    std::array<std::vector<std::shared_ptr<RHIResource>>, RHI_RESOURCE_TYPE_MAX_CNT> m_resourceMap;

    RHIInfo m_rhiInfo;

private:
    static std::shared_ptr<RHI> rhi;
};
```

**VulkanRHI** 继承 `RHI`，实现所有纯虚工厂方法。内部持有全局 VK 状态（当前已有）：
- `m_instance` (`VkInstance`)
- `m_vkbInstance` (`vkb::Instance`)
- `m_physicalDevice` (`VkPhysicalDevice`)
- `m_vkbPhysicalDevice` (`vkb::PhysicalDevice`)
- `m_device` (`VkDevice`)
- `m_allocator` (`VmaAllocator`)
- `m_debugMessenger` (`VkDebugUtilsMessengerEXT`)
- `m_properties` / `m_features` / `m_memoryProperties`
- `m_supportedExtensions`

volk 初始化（`volkInitialize` → `volkLoadInstance` → `volkLoadDevice`）和 VMA 创建在 `VulkanRHI` 构造中完成，这些都是私有实现细节，上层完全不可见。

### 2. RHIResource（所有 RHI 对象的基类）

```cpp
class RHIResource {
public:
    RHIResource() = delete;
    RHIResource(RHIResourceType resourceType) : m_resourceType(resourceType) {}
    virtual ~RHIResource() {}

    inline RHIResourceType GetType() const { return m_resourceType; }
    virtual void* RawHandle() { return nullptr; }  // debug only

private:
    RHIResourceType m_resourceType;
    uint32_t m_lastUseTick = 0;  // GC 用

    virtual void Destroy() {}
    friend class RHI;
};
```

**所有 RHI 类型**均继承 `RHIResource`：
- `RHIQueue`, `RHISurface`, `RHISwapchain` — 基本资源
- `RHICommandPool`, `RHICommandContext` — 命令相关
- `RHIBuffer`, `RHITexture`, `RHITextureView`, `RHISampler`, `RHIShader` — GPU 资源
- `RHIRootSignature`, `RHIDescriptorSet` — 描述符相关
- `RHIRenderPass`, `RHIGraphicsPipeline`, `RHIComputePipeline` — 管线相关
- `RHIFence`, `RHISemaphore` — 同步原语

每个具体 RHI 类型：
- 持有对应的 `*Info` 结构体（以 `m_info` 命名）
- 提供 `GetInfo()` 查询接口
- 在 Vulkan 实现中持有真实的 VK 句柄

### 3. RHICommandContext（= UE IRHICommandContext）

平台相关的抽象命令录制接口。通过缓存状态上下文来降低不必要的状态切换指令。
Vulkan 实现持有 `VkCommandBuffer` 和对应的 `VkCommandPool`。

```cpp
class RHICommandContext : public RHIResource {
public:
    virtual void BeginCommand() = 0;
    virtual void EndCommand() = 0;
    virtual void Execute(std::shared_ptr<RHIFence> waitFence,
                         std::shared_ptr<RHISemaphore> waitSemaphore,
                         std::shared_ptr<RHISemaphore> signalSemaphore) = 0;

    // Barrier
    virtual void TextureBarrier(const RHITextureBarrier& barrier) = 0;
    virtual void BufferBarrier(const RHIBufferBarrier& barrier) = 0;

    // Copy
    virtual void CopyTextureToBuffer(...) = 0;
    virtual void CopyBufferToTexture(...) = 0;
    virtual void CopyBuffer(...) = 0;

    // Render Pass
    virtual void BeginRenderPass(std::shared_ptr<RHIRenderPass> renderPass) = 0;
    virtual void EndRenderPass() = 0;

    // State
    virtual void SetViewport(Offset2D min, Offset2D max) = 0;
    virtual void SetScissor(Offset2D min, Offset2D max) = 0;
    virtual void SetGraphicsPipeline(std::shared_ptr<RHIGraphicsPipeline> pipeline) = 0;
    virtual void SetComputePipeline(std::shared_ptr<RHIComputePipeline> pipeline) = 0;

    // Bind
    virtual void PushConstants(void* data, uint16_t size, ShaderFrequency frequency) = 0;
    virtual void BindDescriptorSet(std::shared_ptr<RHIDescriptorSet> descriptor,
                                   uint32_t set) = 0;
    virtual void BindVertexBuffer(std::shared_ptr<RHIBuffer> vb, uint32_t stream,
                                  uint32_t offset) = 0;
    virtual void BindIndexBuffer(std::shared_ptr<RHIBuffer> ib, uint32_t offset) = 0;

    // Draw / Dispatch
    virtual void Draw(uint32_t vertexCount, uint32_t instanceCount,
                      uint32_t firstVertex, uint32_t firstInstance) = 0;
    virtual void DrawIndexed(uint32_t indexCount, uint32_t instanceCount,
                             uint32_t firstIndex, uint32_t vertexOffset,
                             uint32_t firstInstance) = 0;
    virtual void Dispatch(uint32_t gx, uint32_t gy, uint32_t gz) = 0;
};
```

**RHICommandContextImmediate**：单独的命令上下文，用于非渲染循环的立即执行（如资产上传）。
- `Flush()` 提交所有已记录的命令
- 拥有 Barrier、Copy、GenerateMips 等数据操作接口

### 4. RHICommandList（胶水层）

**不是虚类**。包装 `std::shared_ptr<RHICommandContext>`，提供与 RHICommandContext 完全相同的 API 表面。

两种模式：
- **bypass = true**：直接调用 `m_context->DrawIndexed(...)`
- **bypass = false**：将调用参数打包成 `RHICommand*` 结构体入队，Execute 时统一回放

这是 UE 经典的 RHI 命令模式——允许上层代码在任意线程录制命令，推迟到渲染线程执行。

```cpp
class RHICommandList {
public:
    RHICommandList(const CommandListInfo& info) : m_info(info) {}

    void DrawIndexed(...) {
        if (m_info.bypass) {
            m_info.context->DrawIndexed(...);   // 直接录制
        } else {
            ADD_COMMAND(DrawIndexed, ...);       // 入队
        }
    }
private:
    CommandListInfo m_info;
    std::vector<RHICommand*> m_commands;  // deferred 模式下的命令队列
};
```

### 5. RHI-native 类型

底层图形 API 完全被屏蔽——上层代码不包含 `<vulkan/vulkan.h>`，不知道 VK 的存在。
RHI → VK 的枚举转换函数（`VulkanUtils.h`）仅在 `rhi/vulkan/` 内部使用。

核心类型（在 `RHIStructs.h` 和 `RHIDefinitions.h` 中）：
- **RHIFormat** — `FORMAT_R8G8B8A8_SRGB`, `FORMAT_D32_SFLOAT` 等，映射到 VkFormat
- **RHIResourceState** — `RESOURCE_STATE_UNDEFINED`, `RESOURCE_STATE_COLOR_ATTACHMENT`, `RESOURCE_STATE_PRESENT` 等
- **MemoryUsage** — `MEMORY_USAGE_GPU_ONLY`, `MEMORY_USAGE_CPU_TO_GPU` 等
- **ResourceType** (bitmask) — `RESOURCE_TYPE_TEXTURE`, `RESOURCE_TYPE_VERTEX_BUFFER`, `RESOURCE_TYPE_UNIFORM_BUFFER` 等
- **RasterizerStateInfo**, **DepthStencilStateInfo**, **BlendStateInfo** — PSO 状态结构体，支持 `operator==` 用于 hash 缓存
- **ShaderFrequency** — `SHADER_FREQUENCY_VERTEX`, `SHADER_FREQUENCY_FRAGMENT`, `SHADER_FREQUENCY_COMPUTE`
- **Extent2D / Extent3D / Offset2D** — 平台无关的尺寸/偏移类型
- **TextureSubresourceRange / TextureSubresourceLayers** — 子资源描述
- 所有 `*Info` 结构体 — `RHIBufferInfo`, `RHITextureInfo`, `RHIShaderInfo`, `RHIGraphicsPipelineInfo` 等

VK 枚举 → RHI 枚举的转换函数示例（在 `VulkanUtils.h` 中）：
```cpp
VkFormat RHIFormatToVkFormat(RHIFormat format);
VkImageLayout RHIResourceStateToVkImageLayout(RHIResourceState state, ...);
VkAccessFlags RHIResourceStateToVkAccessFlags(RHIResourceState state);
```

---

## 三层资产架构：Asset / RenderResource / RHI

```
Source Art         Asset              RenderResource (GPU)   RHI
─────────         ─────              ────────────────────   ───
DamagedHelmet     MeshAsset          GpuGeometry            RHIBuffer
  .gltf        →  ├─ SubMeshes[0] → ├─ vertexBuffer →     VkBuffer + VmaAllocation
                  │                 └─ indexBuffer  →     VkBuffer + VmaAllocation
                  │
                  MaterialAsset      GpuMaterial             RHITexture / RHISampler
                  ├─ baseColor=0.8   └─ descSet (textures)    ↑ 实际持有
                  └─ roughness=0.3
```

| 层 | 目录 | 回答问题 | 依赖 |
|---|---|---|---|
| **Asset** | `asset/` | "这是什么？" — 身份、名称、来源路径、元数据、加载状态 | RenderResource |
| **RenderResource** | `resource/` | "怎么画？" — GPU 端数据组织，可绑定、可 draw | RHI |
| **RHI** | `rhi/` | "Vulkan 是什么？" — 平台无关抽象，纯虚接口 + Vulkan 实现 | extern (volk, VMA) |

**关键区分**：
- Asset 之间通过 `AssetId`（uint64 hash）引用，不是裸指针
- RHI 层完全不知道 Asset/Resource 的存在——只管理 Buffer/Texture/Shader 等 GPU 对象
- `RenderResource::GpuGeometry` 持有 `std::shared_ptr<RHIBuffer>`，不直接碰 VK 句柄

---

## 构建目标

| Target | 类型 | 说明 |
|---|---|---|
| `shzk_runtime` | 静态库 | 引擎核心，C++20，链接 `Vulkan::Vulkan`, `fmt::fmt`, `vk-bootstrap`, `imgui`, `fastgltf::fastgltf` |
| `shzk_editor` | 可执行文件 | 编辑器应用，链接 `shzk_runtime`（PRIVATE） |

编译宏：`target_compile_definitions(shzk_runtime PUBLIC VK_NO_PROTOTYPES)`
Include 根路径：`src/` + `extern/`

---

## 当前实现状态（2026-07-25）

### RHI 层现状

| 现有文件 | 状态 | 最终归宿 |
|---|---|---|
| `RHI.h/.cpp` | `Init/Shutdown/Get` + `GetQueue()` + `CreateSurface()` 虚方法。`RHIQueue`/`RHISurface` 内联定义 | → 扩展为完整抽象工厂（添加 `CreateCommandPool`、`CreateCommandContext` 等） |
| `RHIDefinitions.h` | `Extent2D` + `RHIResourceType`（只有 Buffer/Texture）+ `RHIQueueInfo` | → 扩展 `RHIResourceType` 枚举，添加 `CommandPool` 等 |
| `RHIResource.h/.cpp` | 基类 OK，⚠ `namespace SHZK` 错误（应为 `shzk`），`GetType()` return `resourceType` bug | → 修复，RHICommandContext 继承它 |
| `RHICommandContext.h` | ⚠ 空壳 stub（无虚方法，无继承 RHIResource） | → Phase D：完整纯虚接口 |
| `RHICommandList.h/.cpp` | ❌ 不存在 | → Phase D：全局单例胶水层 |
| `RHISurface.h` | 内联定义在 `RHI.h` 中 | → 扩展虚接口，继承 `RHIResource` |
| `RHISwapchain.h` | 空壳，持有 `RHISwapchainInfo` | → 扩展虚接口，继承 `RHIResource` |
| `vulkan/VulkanRHI.h/.cpp` | ✅ 全局 VK 状态 + 初始化 + `CreateQueues()` + `CreateSurface()`。`CreateDescriptorPool()` / `CreateImmediateCommand()` 为空 | → 添加 `CreateCommandPool`、`CreateCommandContext` |
| `vulkan/VulkanRHISurface.h/.cpp` | ✅ 已完成 | |
| `vulkan/VulkanRHIQueue.h` | ✅ 已完成 | |
| `vulkan/VulkanRHIContext.h` | ⚠ 只有一个空的 `class VulkanContext`（名字错误） | → Phase E：VulkanRHICommandContext 实现 |
| `vulkan/VulkanUtils.h` | `VK_CHECK` + `QueueFlagsToString` | 保留，扩展 RHI→VK 枚举转换函数 |
| `RHIStructs.h` | ❌ 不存在 | → Phase A |
| `RHICommandConetxt.cpp` | ❌ 不存在（git 显示 untracked 但文件名拼写错误） | → 应为 `RHICommandContext.cpp` |

### 当前架构 vs 目标架构

```
当前：
  RHI (空壳, 3 static 方法) → VulkanRHI (VK 变量全混在一起，无接口分离)

目标：
  RHI (抽象工厂, N 个纯虚 Create*) → VulkanRHI (实现 + VK 全局状态)
  RHICommandContext (抽象录制) → VulkanRHICommandContext (实现 + VkCmdBuffer)
  RHIResource (基类) → N 个具体 RHI 类型 → VulkanRHI* 子类实现
  RHICommandList (胶水，非虚)
```

---

## 下一步开发计划

### 整体路线

```
★ RHI 重构（按 ToyRenderer 模式）
  ├─ Phase A：RHIStructs.h + 扩展 RHIDefinitions.h — 定义所有平台无关 enum/struct/Info
  ├─ Phase B：扩展 RHIResource.h — 定义所有 RHI 类型的抽象接口
  ├─ Phase C：扩展 RHI + VulkanRHI — 工厂方法 + 全局 VK 状态
  ├─ Phase D：RHICommandContext + RHICommandList — 命令录制
  └─ Phase E：VulkanRHIResource + VulkanRHICommandContext — Vulkan 侧实现

RHI 完成 → RenderResource → Asset → GltfImporter → Pipeline → Descriptor → Forward Pass
```

### Phase A：RHIStructs.h + 扩展 RHIDefinitions.h

**前置修复**：
- 修复 `RHIResource.h` 命名空间 `SHZK` → `shzk`
- 修复 `GetType()` 中 `resourceType` → `m_resourceType`

**内容**：
- 扩展 `RHIResourceType` 枚举（Buffer, Texture, TextureView, Sampler, Shader, ..., Fence, Semaphore）
- `RHIFormat` 枚举（`FORMAT_R8G8B8A8_SRGB` 等）
- `RHIResourceState` 枚举（对标 VkImageLayout）
- `MemoryUsage` 枚举（`MEMORY_USAGE_GPU_ONLY` / `MEMORY_USAGE_CPU_TO_GPU` / `MEMORY_USAGE_GPU_TO_CPU` / `MEMORY_USAGE_CPU_ONLY`）
- `ResourceType` bitmask（`RESOURCE_TYPE_TEXTURE`, `RESOURCE_TYPE_VERTEX_BUFFER` 等）
- `ShaderFrequency` bitmask
- `RasterizerStateInfo`, `DepthStencilStateInfo`, `BlendStateInfo`（带 `operator==`）
- `Extent2D/3D`, `Offset2D/3D`（已有 `Extent2D`，扩展）
- `TextureSubresourceRange/Layers`
- 所有 `*Info` 结构体（`RHIBufferInfo`, `RHITextureInfo`, `RHIShaderInfo`, `RHIGraphicsPipelineInfo` 等）

### Phase B：扩展 RHIResource.h

**内容**：
- 在 `RHIResource` 基类添加 `m_lastUseTick`、`RawHandle()`、`Destroy()`
- 添加所有具体 RHI 类型的抽象声明：
  - `RHIQueue` — `WaitIdle()` 虚方法，持有 `RHIQueueInfo m_info`
  - `RHISurface` — 继承 `RHIResource`，持有 `m_extent`，`GetExtent()`
  - `RHISwapchain` — 继承 `RHIResource`，`GetCurrentFrameIndex()`, `GetTexture()`, `GetNewFrame()`, `Present()`
  - `RHICommandPool` — 持有 contexts 队列，`CreateCommandList()`
  - `RHIBuffer` — `Map() / UnMap()`，持有 `RHIBufferInfo m_info`
  - `RHITexture` / `RHITextureView` / `RHISampler` / `RHIShader`
  - `RHIRootSignature` / `RHIDescriptorSet`
  - `RHIRenderPass` / `RHIGraphicsPipeline` / `RHIComputePipeline`
  - `RHIFence` / `RHISemaphore`
- 定义 `*Ref` 类型别名（`using RHIBufferRef = std::shared_ptr<RHIBuffer>;`）

### Phase C：扩展 RHI + VulkanRHI

**内容**：
- `RHI` 添加纯虚工厂方法（所有 `Create*`）
- `RHI` 添加 `RegisterResource()` 和 `m_resourceMap` 用于 GC
- `RHI` 添加虚 `Tick()` 用于资源回收
- `RHI::Init()` 中 switch 创建 `VulkanRHI`
- `VulkanRHI` 实现所有 `Create*` → `new VulkanRHI*`
- `VulkanRHI` 内部保留现有全局 VK 状态（`m_instance`, `m_device`, `m_allocator` 等）
- 当前的 `CreateInstance/CreatePhysicalDevice/CreateLogicalDevice/CreateMemoryAllocator` 保留在 `VulkanRHI` 构造中

### Phase D：RHICommandContext + RHICommandList

**设计决策（2026-07-25）**：

**RHICommandContext**（≈ UE IRHICommandContext）：
- 抽象类，继承 `RHIResource`，纯虚接口
- Vulkan 实现 `VulkanRHICommandContext`：RAII 构造（模式 3），构造时从 `RHICommandPool` 分配 `VkCommandBuffer`
- 不需要独立的 `RHICommandContextImmediate` 类型——目前只有单线程录制，统一用同一个 Context 接口。后续需要 ImGui/资产上传等一次性操作时再扩展

**RHICommandList**（胶水层）：
- 全局单例 `g_cmdList`（模式 4），非虚类，`= default` 构造
- 不持有 `RHICommandPool`，不参与 Context 池化
- `RHICommandContext` 通过 `SetContext()` 运行时注入，CommandList 不拥有 Context 所有权
- `bypass` 通过 `SetBypass()` 在初始化阶段一次性配置
- 先只实现 **bypass 模式**（直接转发到 Context），deferred 模式留待多线程录制需求时再做
- API 表面与 RHICommandContext 一致（Begin/End/Submit + Draw/DrawIndexed + 状态绑定等）

**RHICommandPool**：
- 仅作为 `VkCommandPool` 的薄封装——给 `CreateCommandContext()` 提供 VkCommandBuffer 的分配来源
- **不做 Context 池化**——目前 FRAMES_IN_FLIGHT 个 Context 预分配后永久复用，不需要 idleContexts 队列和 ReturnToPool

**对比 UE5**：

| | UE5 | SHZK |
|---|---|---|
| CommandList | 全局单例 `GRHICommandList` | 全局单例 `g_cmdList` |
| Context 注入 | `ActivatePipelines()` 时懒获取 | `SetContext()` 一次性注入 |
| bypass 控制 | `LatchBypass()` 全局 latch | `SetBypass()` 初始化阶段配置 |
| 多线程 | 并行 Translate（TaskGraph） | 暂无（单线程录制） |
| 每帧清理 | MemStack 整块回收 | bypass 模式无需清理 |

**VulkanRHICommandContext**：
- 构造时从 Pool 分配 `VkCommandBuffer`（`vkAllocateCommandBuffers`）
- 持有 VkCommandBuffer，不持有 VkCommandPool（Pool 由外部管理）
- `Begin()` → `vkResetCommandBuffer` + `vkBeginCommandBuffer`
- `End()` → `vkEndCommandBuffer`
- `Submit()` → `vkQueueSubmit`（接收可选的 wait/signal semaphore + fence）
- Pipeline 状态缓存（记录当前绑定的 pipeline，避免冗余绑定）
- RenderPass 内状态追踪（确保 Draw 调用在 BeginRenderPass 和 EndRenderPass 之间）

### Phase E：VulkanRHIResource + VulkanRHICommandContext

**内容**：
- 所有具体 RHI 类型的 Vulkan 实现（`VulkanRHIBuffer`, `VulkanRHITexture`, ...）
- `VulkanRHIBuffer`：持有 `VkBuffer` + `VmaAllocation`，用 `RHIBufferInfo` 推导 usage/memory flags
- `VulkanRHITexture`：持有 `VkImage` + `VmaAllocation` + 默认 `VkImageView`
- `VulkanRHIShader`：持有 `VkShaderModule` + 反射信息
- `VulkanUtils.h` 中扩展 RHI→VK 枚举转换函数

---

## 类初始化模式

四种初始化模式，按决策树选择：

### 模式 1：Static Factory Init — 有子类/多态

```cpp
class RHI {
    static std::shared_ptr<RHI> rhi;
public:
    static std::shared_ptr<RHI> Init(const RHIInfo& info);  // factory
    static std::shared_ptr<RHI> Get() { return rhi; }
};
```

**什么时候用**：基类抽象，运行时决定具体子类。`Init()` 本质是 factory。

### 模式 2：Two-Phase Init — 依赖其他系统

```cpp
class RenderSystem {
public:
    RenderSystem() = default;   // trivial 构造
    void Init();                // 等 RHI::Get() 可用后
    void Shutdown();
};
```

**什么时候用**：构造时依赖的系统还未就绪，需要控制初始化顺序。

### 模式 3：RAII 构造 — 无外部依赖

```cpp
class VulkanRHICommandContext : public RHICommandContext {
public:
    VulkanRHICommandContext(RHICommandPoolRef pool, ...);  // 所有工作在构造里完成
};
```

**什么时候用**：构造参数即全部依赖，创建即就绪。

### 模式 4：全局变量 + trivial 构造

```cpp
// .h
extern RHICommandList g_cmdList;

// .cpp
RHICommandList g_cmdList;  // = default，进程级单例
```

**什么时候用**：需要全局访问，无构造依赖。运行时通过 setter 注入配置（`SetContext()`、`SetBypass()`）。

### 决策树

```
需要多态/有子类？
├─ 是 → 模式 1（Static Factory Init）
└─ 否 → 构造依赖其他全局系统？
        ├─ 是 → 模式 2（Two-Phase Init）
        └─ 否 → 构造参数即全部依赖？
                ├─ 是 → 模式 3（RAII 构造）
                └─ 否 / 顶层全局入口 → 模式 4（全局变量 + trivial 构造）
```

### SHZK 类初始化映射

| 类 | 模式 | 原因 |
|---|---|---|
| `RHI` | 模式 1 | 抽象工厂，`Init()` 创建 `VulkanRHI` 子类 |
| `VulkanRHI` | 模式 3 | RAII，构造参数足够，内部创建所有 VK 对象 |
| `RenderSystem` | 模式 2 | 依赖 `RHI::Get()` 已可用 |
| `Engine` | 模式 4 | 顶层入口，`main` 调 `Init()` 控制启动顺序 |
| `RHICommandContext` | 模式 1/3 | 抽象接口（模式 1）；`VulkanRHICommandContext` RAII（模式 3） |
| `RHICommandList` | 模式 4 | 全局单例，`= default` 构造，运行时注入 Context |
| `RHICommandPool` | 模式 3 | RAII，封装 `VkCommandPool`，构造参数足够 |

---

## 关键技术笔记

### RHIFormat → VkFormat 映射

在 `VulkanUtils.h` 中实现转换函数，仅在 `rhi/vulkan/` 内部使用，不向上暴露：
```cpp
VkFormat VulkanUtils::RHIFormatToVkFormat(RHIFormat format);
```

### RHIResourceState → VkImageLayout + VkAccessFlags

```cpp
VkImageLayout VulkanUtils::ToVkImageLayout(RHIResourceState state);
VkAccessFlags VulkanUtils::ToVkAccessFlags(RHIResourceState state);
VkPipelineStageFlags VulkanUtils::ToVkPipelineStage(RHIResourceState state);
```

### VMA

- VMA 不暴露给 RHI 接口——`VulkanRHI` 持有 `m_allocator`（`VmaAllocator`）
- 各 `VulkanRHI*` 资源通过 `VulkanRHI` 间接使用 VMA
- `vkGetInstanceProcAddr` / `vkGetDeviceProcAddr` 由 volk 提供，在 `CreateMemoryAllocator()` 中传给 VMA

### volk

- `extern/volk/volk.h` + `extern/volk/volk.c` 已添加到项目中
- `VulkanRHI.cpp` 中 `#define VOLK_IMPLEMENTATION` → `#include <volk/volk.h>` 内联编译
- `volk.c` 必须保留（被 `volk.h` 通过 `#include "volk.c"` 相对路径引用）
- `VK_NO_PROTOTYPES` 全局设置（`src/CMakeLists.txt`）

### Barrier 转换时机

```
AcquireNextImage → image layout = UNDEFINED (首帧) / PRESENT_SRC_KHR (后续)
ProduceFrame 开头 → TextureBarrier(UNDEFINED → COLOR_ATTACHMENT_OPTIMAL)
SubmitAndPresent 中 → EndRendering() 后 → TextureBarrier(COLOR_ATTACHMENT_OPTIMAL → PRESENT_SRC_KHR)
```

### Bindless（远期）

- ToyRenderer 使用 bindless 管理全局纹理/采样器/顶点缓冲数组
- SHZK 初期每个 material 一个 descriptor set，完全够用
- 远期参考 ToyRenderer 实现全局 bindless 分配

---

## 参考项目

### ToyRenderer（主要 RHI 参考）
- 位置：`../ToyRenderer/renderer/`
- RHI 设计完全参考 UE：FDynamicRHI → RHI，IRHICommandContext → RHICommandContext
- RHICommandList 胶水层 + bypass/deferred 双模式
- RHI-native 类型完备（RHIFormat, RHIResourceState, MemoryUsage 等）
- RenderResource 层对 RHI 做渲染向封装，RenderSystem 使用 RDG 管理 Pass

| | Piccolo | Spartan Engine | Filament | ToyRenderer |
|---|---|---|---|---|
| RHI 风格 | 虚接口 | 具体类，编译期选后端 | 虚接口 backend | **FDynamicRHI + IRHICommandContext（UE 风格）** |
| RHI 抽象程度 | 仅包装 Vulkan 部分功能 | 直接暴露 VK 类型 | 完全自定义类型 | **完整平台无关类型系统** |
| 命令录制 | 无抽象 | 无抽象 | 自定义 CommandStream | **RHICommandContext + RHICommandList 胶水** |
| 资源管理 | 无 | 无 | 两层 Asset/Resource | **RenderResource 层 + manager** |

---

## 用户偏好

- **不要直接修改代码**——提供代码示例或口头指导，让用户自己改
- **每次回答前阅读并分析用户当前的实现作为前提给出答案**——不要凭记忆，不要假设用户改了或没改什么。必须 Read 相关文件后再回答
- **以工业界成熟引擎的做法为准**——RHI 层优先参考 ToyRenderer（UE 风格，单后端但有完整抽象），RenderResource/RenderSystem 同样参考 ToyRenderer。也要参考 UE5 实际源码（`C:\Users\earn\UE5\`），理解 UE 的实践并判断哪些适合 SHZK 的规模
- **类初始化按决策树选择模式**——有子类用 Static Factory Init（模式 1），依赖外部系统用 Two-Phase Init（模式 2），无依赖用 RAII 构造（模式 3），全局单例无依赖用 trivial 构造 + 运行时 setter 注入（模式 4）。详见上文"类初始化模式"
- System 类（RHI、RenderSystem）使用二段式：`= default` ctor/dtor + `Initialize()` / `Shutdown()`
- Resource 类（RHIResource、GpuGeometry、VulkanRHICommandContext）使用构造函数 RAII 一段式
- 所有 RHI 类型使用 `std::shared_ptr` 管理生命周期
- `*Info` 结构体用于构造参数传递，不搞多参数构造函数
- 成员变量 `m_` 前缀，静态单例无前缀（`RHI::rhi`），全局单例 `g_` 前缀（`g_cmdList`）
