# SHZK — Vulkan Renderer

C++20 Vulkan 1.3 实时渲染器。参考 Unreal Engine 和 ToyRenderer (可在父目录/ToyRenderer下找到) 的架构。
RHI 层设计参考 ToyRenderer（UE 风格：FDynamicRHI + IRHICommandContext + FRenderResource）。
最后更新：2026-07-27

---

## ★ 当前阶段目标（2026-07-27）

### 核心任务：RHI 层 bypass 模式命令录制 + 第一个三角形

RHI 层基础框架已搭建完成：
- ✅ RHI 抽象工厂（`CreateCommandPool`、`CreateSemaphore`、`CreateFence` 等）
- ✅ VulkanRHI 全局 VK 状态（Instance、Device、VMA、Queues）
- ✅ RHICommandPool / VulkanRHICommandPool（薄封装 VkCommandPool，不做 Context 池化）
- ✅ RHICommandContext / VulkanRHICommandContext（BeginCommand / EndCommand）
- ✅ RHICommandList 全局单例（bypass 模式，forward 到 Context）
- ✅ RHISemaphore / RHIFence（同步原语）
- ✅ RenderSystem 维护 PerFrameRHIResource（FRAMES_IN_FLIGHT 个 Context + 同步对象）

### 当前 TODO

1. **补 VK 资源析构** — VulkanRHICommandPool/VulkanRHICommandContext/VulkanRHISemaphore/VulkanRHIFence 缺析构函数
2. **VulkanRHICommandContext 补全** — BeginCommand/EndCommand 实现 vkReset + vkBegin/vkEnd；构造接收 VkDevice + VkCommandPool
3. **RHIFence::Wait()** — 实现 vkWaitForFences
4. **RHICommandList 转发方法** — bypass 模式下 Begin/End/SetContext 转发到 Context
5. **RenderSystem::Tick()** — 帧循环：AcquireNextImage → Begin → 录制 → End → Submit → Present
6. **Swapchain** — 当前 RHISwapchain 为空壳，需要实现

完成后目标：画出第一个三角形（Clear + Present）

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
        │       ├── VulkanRHICommandPool.h/.cpp    # VkCommandPool 薄封装
        │       ├── VulkanRHISurface.h/.cpp        # Vulkan Surface 实现
        │       ├── VulkanRHIQueue.h               # Vulkan Queue 实现
        │       ├── VulkanRHISemaphore.h/.cpp      # Vulkan Semaphore 实现
        │       ├── VulkanRHIFence.h/.cpp           # Vulkan Fence 实现
        │       └── VulkanUtil.h                   # VK_CHECK + RHI→VK 转换函数
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
- 全局单例：`g_RhiCmdList`（静态成员）

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

```cpp
class RHI {
public:
    static std::shared_ptr<RHI> Init(const RHIInfo& info);
    static void Shutdown();
    static std::shared_ptr<RHI> Get() { return rhi; }

    virtual void Tick();  // 资源 GC

    // 基本资源
    virtual std::shared_ptr<RHIQueue> GetQueue(const RHIQueueInfo& info) = 0;
    virtual std::shared_ptr<RHISurface> CreateSurface(SDL_Window* window) = 0;
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
    virtual std::shared_ptr<RHIFence> CreateFence() = 0;
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

**VulkanRHI** 继承 `RHI`，实现所有纯虚工厂方法。内部持有全局 VK 状态：
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
Vulkan 实现持有 `VkCommandBuffer` 和对应的 `VkDevice` / `VkCommandPool` 引用（析构时归还）。

```cpp
class RHICommandContext {
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
- **bypass = true**（当前实现）：直接调用 `m_context->DrawIndexed(...)`
- **bypass = false**（远期）：将调用参数打包成 `RHICommand*` 结构体入队，Execute 时统一回放

```cpp
class RHICommandList {
public:
    static void Init(bool bypass = true);
    static std::shared_ptr<RHICommandList> Get();

    void Begin();
    void End();
    void SetContext(RHICommandContext* context);

    // 每个方法：if (m_bypass) { m_context->Method(...); }
    void DrawIndexed(...);
    void SetGraphicsPipeline(...);
    // ...

private:
    RHICommandContext* m_context;
    bool m_bypass;
};
```

### 5. PerFrameRHIResource — 帧资源管理

`RenderSystem` 维护 `FRAMES_IN_FLIGHT`（= 2）个 PerFrameRHIResource，不做 Context 池化：

```cpp
struct PerFrameRHIResource {
    std::shared_ptr<RHICommandContext> cmdContext;      // 从 RHICommandPool 创建
    std::shared_ptr<RHISemaphore>      startSemaphore;  // image acquired
    std::shared_ptr<RHISemaphore>      endSemaphore;    // render finished
    std::shared_ptr<RHIFence>          fence;           // 帧同步
};
```

单线程 bypass 模式下，`FRAMES_IN_FLIGHT` 个 Context 足够循环复用。fence 保证 GPU 先完成上一轮才能 reset CommandBuffer。后期若拆分 RHI 线程且需要并行翻译，才需要池化。

### 6. RHICommandPool — VkCommandPool 薄封装

```cpp
class RHICommandPool {
public:
    RHICommandPool(const RHICommandPoolInfo& info);
    virtual std::shared_ptr<RHICommandContext> CreateCommandContext() = 0;
};
```

`VulkanRHICommandPool` 内部持有 `VkCommandPool`（带 `VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT`），`CreateCommandContext()` 调 `vkAllocateCommandBuffers` 分沚 VkCommandBuffer 然后构造 `VulkanRHICommandContext`。

**不做 Context 池化**（无 idleContexts/ReturnToPool）。Context 数量 = `FRAMES_IN_FLIGHT`，用完循环。

### 7. RHI-native 类型

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

## 当前实现状态（2026-07-27）

### RHI 层

| 文件 | 状态 |
|---|---|
| `RHI.h/.cpp` | ✅ `Init/Shutdown/Get` + `GetQueue()` + `CreateSurface()` + `CreateCommandPool()` + `CreateSemaphore()` + `CreateFence()` |
| `RHIDefinitions.h` | ✅ `RHIInfo`, `RHIQueueType`, `RHIQueueInfo`, `RHICommandPoolInfo`, `Extent2D`, `FRAMES_IN_FLIGHT` |
| `RHIResource.h/.cpp` | ⚠ 基类 OK，`namespace SHZK` 错误（应为 `shzk`），`GetType()` return `resourceType` bug |
| `RHICommandContext.h` | ✅ 抽象接口 `BeginCommand()` / `EndCommand()`，其余方法注释中待激活 |
| `RHICommandContext.cpp` | ❌ 不存在 |
| `RHICommandList.h/.cpp` | ✅ 全局单例（`g_RhiCmdList` 静态成员），bypass 模式，`Begin()`/`End()` + `Init()`/`Get()`/`SetContext()` |
| `RHISurface.h` | ✅ 内联定义在 `RHI.h` 中 |
| `RHISwapchain.h` | ⚠ 空壳，持有 `RHISwapchainInfo` |
| `vulkan/VulkanRHI.h/.cpp` | ✅ 全局 VK 状态 + 初始化 + `GetQueue()` + `CreateSurface()` + `CreateCommandPool()` + `CreateSemaphore()` + `CreateFence()` |
| `vulkan/VulkanRHIQueue.h` | ✅ 已完成 |
| `vulkan/VulkanRHISurface.h/.cpp` | ✅ 已完成 |
| `vulkan/VulkanRHICommandPool.h/.cpp` | ✅ VkCommandPool 薄封装 + `CreateCommandContext()`，**缺析构** |
| `vulkan/VulkanRHICommandContext.h/.cpp` | ⚠ 头文件完成，cpp 空壳（Begin/End 未实现），**缺 VkDevice/VkPool 成员 + 析构** |
| `vulkan/VulkanRHISemaphore.h/.cpp` | ✅ 创建完成，**缺析构** |
| `vulkan/VulkanRHIFence.h/.cpp` | ✅ 创建完成，**缺析构**；`Wait()` 空壳待实现 |
| `vulkan/VulkanUtil.h` | ✅ `VK_CHECK` + `QueueFlagsToString` |
| `RHIStructs.h` | ❌ 待创建 |

### RenderSystem 层

| 文件 | 状态 |
|---|---|
| `RenderSystem.h/.cpp` | ✅ `Init()` 创建 RHI → Surface → Queue → CommandPool → 全局 CommandList → PerFrameRHIResources；`Tick()` 空壳 |
| PerFrameRHIResource | ✅ `std::array<PerFrameRHIResource, FRAMES_IN_FLIGHT>`，包含 cmdContext + startSemaphore + endSemaphore + fence |

### 同步与帧循环

| 组件 | 状态 |
|---|---|
| Semaphore / Fence | ✅ RHI 抽象 + Vulkan 实现已创建 |
| Swapchain | ❌ 空壳，待实现 |
| RenderSystem::Tick() | ❌ 空壳，待实现帧循环 |
| RHICommandList 转发方法 | ❌ SetContext/Execute/Draw* 等均注释中待实现 |

---

## 下一步开发计划

### 整体路线

```
★ RHI 层 bypass 模式打通第一个三角形
  ├─ 补 VK 资源析构（CommandPool/CommandContext/Semaphore/Fence）
  ├─ VulkanRHICommandContext BeginCommand/EndCommand 实现
  ├─ RHIFence::Wait() 实现
  ├─ RHICommandList 转发方法（SetContext + Begin/End 转发到 Context）
  ├─ Swapchain 实现
  └─ RenderSystem::Tick() 帧循环（Acquire → Record → Submit → Present）

RHI bypass 打通 → RenderResource → Asset → GltfImporter → Pipeline → Descriptor → Forward Pass
```

### Phase A：RHIStructs.h + 扩展 RHIDefinitions.h

**前置修复**：
- 修复 `RHIResource.h` 命名空间 `SHZK` → `shzk`
- 修复 `GetType()` 中 `resourceType` → `m_resourceType`

### Phase B：扩展 RHIResource.h

- 添加所有具体 RHI 类型的抽象声明
- 定义 `*Ref` 类型别名

### Phase C：扩展 RHI + VulkanRHI

- 添加更多 `Create*` 工厂方法（Buffer, Texture, Shader, Pipeline 等）

### Phase D：RHICommandList 转发方法

- bypass 模式下所有方法转发到 Context
- deferred 模式留待后期实现

### Phase E：VulkanRHIResource + VulkanRHICommandContext 完整实现

- 所有具体 RHI 类型的 Vulkan 实现
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
    VulkanRHICommandContext(VkCommandBuffer cmdBuffer, VkDevice device, VkCommandPool pool);
    // 所有工作在构造里完成，析构归还 VkCommandBuffer
};
```

**什么时候用**：构造参数即全部依赖，创建即就绪。

### 模式 4：全局变量 + trivial 构造

```cpp
class RHICommandList {
    static std::shared_ptr<RHICommandList> g_RhiCmdList;
public:
    RHICommandList() = default;
    static std::shared_ptr<RHICommandList> Get() { return g_RhiCmdList; }
};
```

**什么时候用**：需要全局访问，无构造依赖。运行时通过 setter 注入配置（`SetContext()`、`Init()`）。

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
| `RHICommandContext` | 模式 1 | 抽象接口，`VulkanRHICommandContext` RAII（模式 3） |
| `RHICommandList` | 模式 4 | 全局单例，`= default` 构造，运行时注入 Context |
| `RHICommandPool` | 模式 3 | RAII，封装 `VkCommandPool`，构造参数即全部依赖 |

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

### volk

- `extern/volk/volk.h` + `extern/volk/volk.c` 已添加到项目中
- `VulkanRHI.cpp` 中 `#define VOLK_IMPLEMENTATION` → `#include <volk/volk.h>` 内联编译
- 其他 `vulkan/` .cpp 文件必须在第一行 `#include <volk/volk.h>`（因为 `VK_NO_PROTOTYPES`）
- `VK_NO_PROTOTYPES` 全局设置（`src/CMakeLists.txt`）

### Windows API 宏污染

`<windows.h>`（SDL3 间接引入）会把 `CreateSemaphore` 宏替换为 `CreateSemaphoreA`。在 `RHI.h` 中已添加 `#ifdef CreateSemaphore / #undef CreateSemaphore` 修复。

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

### UE5 源码参考

位于 `C:\Users\earn\UE5\UnrealEngine-release\Engine\Source\Runtime\`

| 文件 | 用途 |
|---|---|
| `RHI/Public/RHIContext.h` | `IRHIComputeContext`, `IRHICommandContext` 接口定义 |
| `RHI/Public/DynamicRHI.h` | `FDynamicRHI` 抽象工厂 |
| `RHI/Public/RHICommandList.h` | `FRHICommandListBase` → `FRHIComputeCommandList` → `FRHICommandList` → `FRHICommandListImmediate` 继承树 |
| `RHI/Public/RHICommandListCommandExecutes.inl` | Command `Execute()` 宏展开 → `CmdList.GetContext().RHI*` |
| `RHI/Private/RHICommandList.cpp` | `ActivatePipelines()`, `Execute()`, context 分配 |
| `VulkanRHI/Private/VulkanCommands.cpp` | Vulkan `RHIGetCommandContext()` |
| `D3D12RHI/Private/D3D12Device.cpp` | D3D12 `ObtainContext()`/`ReleaseContext()` 对象池 |

| | Piccolo | Spartan Engine | Filament | ToyRenderer |
|---|---|---|---|---|
| RHI 风格 | 虚接口 | 具体类，编译期选后端 | 虚接口 backend | **FDynamicRHI + IRHICommandContext（UE 风格）** |
| RHI 抽象程度 | 仅包装 Vulkan 部分功能 | 直接暴露 VK 类型 | 完全自定义类型 | **完整平台无关类型系统** |
| 命令录制 | 无抽象 | 无抽象 | 自定义 CommandStream | **RHICommandContext + RHICommandList 胶水** |
| 资源管理 | 无 | 无 | 两层 Asset/Resource | **RenderResource 层 + manager** |

---

## 用户偏好

- **每次回答前阅读并分析用户当前的实现作为前提给出答案**——不要凭记忆，不要假设用户改了或没改什么。必须 Read 相关文件后再回答
- **以工业界成熟引擎的做法为准**——RHI 层优先参考 ToyRenderer（UE 风格，单后端但有完整抽象），RenderResource/RenderSystem 同样参考 ToyRenderer。也要参考 UE5 实际源码（`C:\Users\earn\UE5\`），理解 UE 的实践并判断哪些适合 SHZK 的规模
- **类初始化按决策树选择模式**——有子类用 Static Factory Init（模式 1），依赖外部系统用 Two-Phase Init（模式 2），无依赖用 RAII 构造（模式 3），全局单例无依赖用 trivial 构造 + 运行时 setter 注入（模式 4）。详见上文"类初始化模式"
- System 类（RHI、RenderSystem）使用二段式：`= default` ctor/dtor + `Initialize()` / `Shutdown()`
- Resource 类（RHIResource、GpuGeometry、VulkanRHICommandContext）使用构造函数 RAII 一段式
- 所有 RHI 类型使用 `std::shared_ptr` 管理生命周期
- `*Info` 结构体用于构造参数传递，不搞多参数构造函数
- 成员变量 `m_` 前缀，静态单例无前缀（`RHI::rhi`），全局单例 `g_` 前缀（`g_cmdList`）
- 每个 .cpp 文件的 `vulkan/` 子目录下，`#include <volk/volk.h>` 必须在第一行
