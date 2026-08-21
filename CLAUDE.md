# SHZK — Vulkan Renderer

C++20 Vulkan 1.3 实时渲染器。参考 Unreal Engine 和 ToyRenderer (可在父目录/ToyRenderer下找到) 的架构。
RHI 层设计参考 ToyRenderer（UE 风格：FDynamicRHI + IRHICommandContext + FRenderResource）。
最后更新：2026-08-21

---

## ★ 当前阶段目标（2026-08-21）

### 核心任务：Forward Pass —— 画出 glTF 模型

已完成（RHI 层 + 资产管线打通）：
- ✅ RHI 抽象工厂：Buffer/Texture/TextureView/Shader/RootSignature/GraphicsPipeline 全部可创建
- ✅ `VulkanRHIGraphicsPipeline` 完整实现（dynamic rendering + dynamic state VIEWPORT/SCISSOR）
- ✅ glTF 导入：`GltfLoader::Load()` → textures / materials / models
- ✅ Texture：stb_image 加载 → RHI 创建 + staging 上传 + mip 生成
- ✅ Material：PBR 参数 + 纹理引用 + shader/general slots + pass mask + 管线状态
- ✅ VertexBuffer / IndexBuffer：CPU 数据 → staging → GPU buffer
- ✅ Shader：SPIR-V 加载（RenderResourceManager 缓存）
- ✅ SceneRenderer / MeshPassProcessor / MeshDrawCommand：MeshBatch → MeshDrawCommand

### 当前 TODO（距离画出 ForwardPass 的步骤）

1. **补齐 RHI draw 命令** — `RHICommandContext` / `RHICommandList` 中 `BeginRenderPass / EndRenderPass / SetViewport / SetScissor / SetGraphicsPipeline / BindVertexBuffer / BindIndexBuffer / BindDescriptorSet / SetPushConstants / DrawIndexed` 目前全部注释着，需先打通（映射 dynamic rendering 的 `vkCmdBeginRendering` 等）
2. **ForwardPass::Init() 填 RootSignature** — descriptor set layout（set 0 per-frame UBO + set 1 材质纹理）+ push constant range（model matrix + 材质标量）
3. **FindOrCreateGraphicsPipelineState 缓存** — `GraphicsMinimalPipelineState`（缓存键）+ output 信息（color/depth format，由 Pass 持有）→ 合成完整 `RHIGraphicsPipelineInfo` → 创建 `RHIGraphicsPipeline`
4. **MeshDrawShaderBindings** — per-draw 绑定（descriptor set + push constant 数据），在 `BuildMeshDrawCommands` 从 Material 填充
5. **MeshDrawCommand::SubmitDraw / SubmitDrawBegin / SubmitDrawEnd** — 绑 PSO → 绑 VB/IB → 绑 descriptor set → push 常量 → DrawIndexed
6. **per-frame UBO 上传** — viewProj 矩阵每帧更新（`RenderResourceManager::PerFrameResource` + `m_perFrameRootSignature`）
7. **RenderSystem::Tick 接上真正的 render pass** — 替换当前 Clear-only，`BeginRenderPass → SubmitDraw → EndRenderPass`

完成后目标：glTF 模型渲染到屏幕

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
        ├── global/         # Engine 入口
        │   └── Engine.h/.cpp
        ├── core/           # 基础类型
        │   ├── Transform.h      # Transform 结构体（translation/rotation/scale）
        │   └── Primitive.h      # CPU 端网格数据（position, normal, texcoord）
        ├── log/            # 日志系统
        │   └── Log.h
        ├── WindowSystem.h/.cpp  # SDL3 窗口封装
        ├── render/         # RenderSystem + RenderResource + Pass
        │   ├── RenderSystem.h/.cpp         # 帧循环、Swapchain、PerFrameRHIResource、Pass 注册
        │   ├── SceneRenderer.h/.cpp        # 遍历 Scene → 收集 MeshBatch → 喂给 Pass
        │   ├── MeshBatch.h                 # MeshBatch / MeshBatchElement（VertexFactory + Material + index）
        │   ├── MeshDrawCommand.h/.cpp      # MeshDrawCommand + GraphicsMinimalPipelineState（PSO 缓存键）
        │   ├── MeshPassProcessor.h/.cpp    # BuildMeshDrawCommands：MeshBatch → MeshDrawCommand
        │   ├── passes/
        │   │   ├── RenderPass.h            # RenderPass 基类 + PassType/MeshPassType
        │   │   ├── MeshPass.h/.cpp         # MeshPass（持有 MeshPassProcessor）
        │   │   └── ForwardPass.h/.cpp      # ForwardPass + ForwardPassProcessor
        │   └── resources/
        │       ├── Buffer.h/.cpp            # VertexBuffer + IndexBuffer（RenderResource）
        │       ├── VertexFactory.h/.cpp     # VertexFactory / InterleavedVertexFactory（RHIVertexDeclaration）
        │       ├── GraphicsPipelineState.h  # GraphicsPipelineState（持有 RHIGraphicsPipeline，待补）
        │       └── RenderResourceManager.h/.cpp  # RHIShader 缓存 + per-frame UBO/descriptor
        ├── rhi/            # RHI 封装 — UE 风格平台无关抽象
        │   ├── RHI.h/.cpp                # 抽象工厂 + 全局单例（RHI）+ 所有 RHI 基类内联
        │   ├── RHIResource.h/.cpp        # RHI 资源基类 + GPU 资源类型声明
        │   ├── RHICommandList.h/.cpp      # 胶水层 — bypass 模式 forward 到 Context
        │   ├── RHIDefinitions.h/.cpp     # 所有 enum/struct/Info 类型 + 基础定义
        │   ├── RHIUtil.h                 # RHI 通用工具函数（IsDepthFormat 等）
        │   └── vulkan/
        │       ├── VulkanRHI.h/.cpp              # 工厂实现 + 全局 VK 状态 + CommandContext + ImmediateContext
        │       ├── VulkanRHIResource.h/.cpp      # Vulkan 资源实现（Buffer, Texture, TextureView, Shader, DescriptorSet, RootSignature, GraphicsPipeline）
        │       ├── VulkanRHICommandPool.h/.cpp    # VkCommandPool 薄封装
        │       ├── VulkanRHISurface.h/.cpp        # Vulkan Surface 实现
        │       ├── VulkanRHISwapchain.h/.cpp      # Vulkan Swapchain 实现
        │       ├── VulkanRHIQueue.h/.cpp          # Vulkan Queue 实现
        │       ├── VulkanRHISemaphore.h/.cpp      # Vulkan Semaphore 实现
        │       ├── VulkanRHIFence.h/.cpp           # Vulkan Fence 实现
        │       └── VulkanUtil.h                   # VK_CHECK + RHI→VK 转换函数
        ├── asset/          # ★ Asset 层 — 引擎资产（身份、元数据）
        │   ├── Asset.h                  # AssetType, name
        │   ├── Model.h/.cpp             # SubMesh 列表 + 引用 VertexBuffer/IndexBuffer/Material
        │   ├── Material.h/.cpp          # PBR 参数 + 纹理引用 + shader slots + pass mask + 管线状态
        │   ├── Texture.h/.cpp           # 文件路径 + GPU 资源（RHITexture/View），stb_image 加载 + mip 生成
        │   ├── Shader.h/.cpp            # Shader：path + frequency + entry → RHIShader
        │   └── AssetManager.h/.cpp      # 全局资产注册表（name → shared_ptr\<Asset\>）
        ├── import/         # glTF/glb 导入器
        │   └── GltfLoader.h/.cpp       # Load() → GltfLoadResult（textures/materials/models）
        └── framework/      # ECS 骨架
            ├── Scene.h/.cpp
            ├── Node.h/.cpp
            └── components/
                ├── Component.h              # Component 基类
                ├── Drawable.h               # Drawable 接口（CollectMeshBatchWithTransform）
                ├── TransformComponent.h/.cpp
                ├── CameraComponent.h/.cpp
                ├── MeshComponent.h/.cpp     # 持有 Model，实现 Drawable
                └── DirectionalLightComponent.h/.cpp
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
- 静态单例：所有全局单例使用 `g_` 前缀（`RHI::g_rhi`、`RHICommandList::g_rhiCmdList`、`Engine::g_engine`、`AssetManager::g_assetManager`、`RenderResourceManager::g_renderResourceManager`）

### 命名空间

- 统一使用 `shzk`（小写）

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
    ├─ RHI::Get()->CreateBuffer(info)      ← 工厂，创建 RHI 对象
    ├─ RHICommandList::Get()->BeginCommand() ← 胶水，bypass 或延迟入队
    │       │
    │       └─ RHICommandContext::RHIBeginCommand()  ← 平台实现，录制 VkCmd
    │
    └─ RHI::Get()->Destroy()              ← 全局销毁
```

### 1. RHI（= UE FDynamicRHI）

平台无关的抽象工厂，全局单例 `RHI::Get()`（返回 `std::shared_ptr<RHI>&`）。
**所有与 CommandList 无关的资源创建都走这里。**

```cpp
class RHI
{
private:
    static std::shared_ptr<RHI> g_rhi;

public:
    static std::shared_ptr<RHI> Init(const RHIInfo& rhiInfo);
    static std::shared_ptr<RHI>& Get() { return g_rhi; }

    virtual void Destroy() = 0;

    // Fundamentals
    virtual std::shared_ptr<RHIQueue> GetQueue(const RHIQueueInfo& info) = 0;
    virtual std::shared_ptr<RHISurface> CreateSurface(SDL_Window* window) = 0;
    virtual std::shared_ptr<RHISwapchain> CreateSwapchain(const RHISwapchainInfo& info) = 0;
    virtual std::shared_ptr<RHICommandPool> CreateCommandPool(const RHICommandPoolInfo& info) = 0;
    virtual std::shared_ptr<RHISemaphore> CreateSemaphore() = 0;
    virtual std::shared_ptr<RHIFence> CreateFence() = 0;

    // Resources
    virtual std::shared_ptr<RHIBuffer> CreateBuffer(const RHIBufferInfo& info) = 0;
    virtual std::shared_ptr<RHITexture> CreateTexture(const RHITextureInfo& info) = 0;
    virtual std::shared_ptr<RHITextureView> CreateTextureView(const RHITextureViewInfo& info) = 0;
    virtual std::shared_ptr<RHIShader> CreateShader(const RHIShaderInfo& info) = 0;
    virtual std::shared_ptr<RHIRootSignature> CreateRootSignature(const RHIRootSignatureInfo& info) = 0;
    virtual std::shared_ptr<RHIGraphicsPipeline> CreateGraphicsPipeline(
        const RHIGraphicsPipelineInfo& info) = 0;

    // Immediate command (non-virtual — always VulkanRHICommandContextImmediate)
    std::shared_ptr<RHICommandContextImmediate> GetCommandContextImmediate() const
    { return m_cmdContextImmediate; }

protected:
    RHI() = delete;
    RHI(const RHIInfo& rhiInfo) : m_rhiInfo(rhiInfo) {}
    ~RHI() = default;

    RHIInfo m_rhiInfo;
    std::shared_ptr<RHICommandContextImmediate> m_cmdContextImmediate;
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
- `m_queueFamilyProperties` / `m_queueIndices` / `m_queueCounts` / `m_queues`
- `m_descriptorPool` (`VkDescriptorPool`)

volk 初始化（`volkInitialize` → `volkLoadInstance` → `volkLoadDevice`）和 VMA 创建在 `VulkanRHI` 构造中完成，这些都是私有实现细节，上层完全不可见。

### 2. RHIResource（所有 GPU 资源的基类）

```cpp
class RHIResource
{
public:
    RHIResource() = delete;
    RHIResource(RHIResourceType resourceType) : m_resourceType(resourceType) {}
    ~RHIResource() = default;

    inline RHIResourceType GetType() const { return m_resourceType; }

protected:
    virtual void Destroy() = 0;
    RHIResourceType m_resourceType;
};
```

**所有 GPU RHI 类型**均继承 `RHIResource`：
- `RHIBuffer`, `RHITexture`, `RHITextureView`, `RHISampler`, `RHIShader` — GPU 资源
- `RHIRootSignature`, `RHIDescriptorSet` — 描述符相关
- `RHIRenderPass`, `RHIGraphicsPipeline` — 管线相关

每个具体 RHI 类型：
- 持有对应的 `*Info` 结构体（以 `m_info` 命名）
- 提供 `GetInfo()` 查询接口
- 在 Vulkan 实现中持有真实的 VK 句柄
- `Destroy()` 为 protected，由 RHI 系统或 owning 对象调用

**非 RHIResource 的 RHI 对象**（`RHICommandContext`、`RHICommandPool`、`RHISemaphore`、`RHIFence` 等）：
- 基类声明 `virtual void Destroy() = 0;` 为 **public**
- Vulkan 实现用 `override final` 完成实际销毁

### 3. RHICommandContext（= UE IRHICommandContext）

平台相关的抽象命令录制接口。Vulkan 实现持有 `VkCommandBuffer` 和对应的 `RHICommandPool` 引用。
**活跃方法使用 `RHI` 前缀**（UE 风格），方便区分 Context 方法和 CommandList 胶水方法。

```cpp
class RHICommandContext
{
public:
    virtual void Destroy() = 0;

    virtual void RHIBeginCommand() = 0;
    virtual void RHIEndCommand() = 0;
    virtual void RHISubmit(
        std::shared_ptr<RHIFence> fence,
        std::shared_ptr<RHISemaphore> waitSemaphore,
        std::shared_ptr<RHISemaphore> signalSemaphore) = 0;
    virtual void RHITextureClearColor(std::shared_ptr<RHITexture> texture, glm::vec4 rgba) = 0;
    virtual void RHITextureBarrierCommand(const RHITextureBarrier& barrier) = 0;

    // 远期方法（当前注释中）：
    // RHICopyTextureToBuffer, RHICopyBufferToTexture, RHICopyBuffer,
    // RHIBeginRenderPass, RHIEndRenderPass,
    // RHISetViewport, RHISetScissor, RHISetGraphicsPipeline,
    // RHIPushConstants, RHIBindDescriptorSet, RHIBindVertexBuffer, RHIBindIndexBuffer,
    // RHIDraw, RHIDrawIndexed, RHIDispatch
};
```

**RHICommandContextImmediate**：单独的命令上下文，用于非渲染循环的立即执行（如资产上传）。
- 已实现并投入使用：`RHISubmit` / `RHITextureBarrierCommand` / `RHIBufferBarrierCommand` / `RHICopyTextureToBuffer` / `RHICopyBufferToTexture` / `RHICopyBuffer` / `RHICopyTexture` / `RHIGenerateMips`
- 通过 `RHI::Get()->GetCommandContextImmediate()` 直接访问（`Texture::LoadFromFile`、`VertexBuffer`/`IndexBuffer` 上传都用它）

### 4. RHICommandList（胶水层）

**不是虚类**。包装 `RHICommandContext*`，提供与 RHICommandContext 对应的胶水方法。

两种模式：
- **bypass = true**（当前实现）：直接调用 `m_cmdContext->RHI*()`
- **bypass = false**（远期）：将调用参数打包成 `RHICommand*` 结构体入队，Execute 时统一回放

```cpp
class RHICommandList
{
private:
    static std::shared_ptr<RHICommandList> g_rhiCmdList;

public:
    static void Init(bool bypass = true);
    static std::shared_ptr<RHICommandList>& Get() { return g_rhiCmdList; }

    inline void SetContext(RHICommandContext* context) { m_cmdContext = context; }
    inline RHICommandContext& GetContext() { return *m_cmdContext; }

    void BeginCommand();
    void EndCommand();
    void Submit(std::shared_ptr<RHIFence> fence,
                std::shared_ptr<RHISemaphore> waitSemaphore,
                std::shared_ptr<RHISemaphore> signalSemaphore);
    void TextureClearColor(std::shared_ptr<RHITexture> texture, glm::vec4 rgba);
    void TextureBarrier(const RHITextureBarrier& barrier);

private:
    RHICommandContext* m_cmdContext;
    bool m_bypass;
};
```

### 5. Immediate 命令的访问方式

Immediate 命令（资产上传）**不经过 RHICommandList 胶水层**，直接通过
`RHI::Get()->GetCommandContextImmediate()` 拿 `RHICommandContextImmediate*` 调用。
（旧版的 `RHICommandListImmediate` 胶水类已移除，`RHICommandContextImmediate` 定义在 `RHI.h` 内联。）

### 6. PerFrameRHIResource — 帧资源管理

`RenderSystem` 维护 `FRAMES_IN_FLIGHT`（= 2）个 PerFrameRHIResource，不做 Context 池化：

```cpp
struct PerFrameRHIResource
{
    std::shared_ptr<RHICommandContext> cmdContext;      // 从 RHICommandPool 创建
    std::shared_ptr<RHISemaphore>      startSemaphore;  // image acquired
    std::shared_ptr<RHISemaphore>      endSemaphore;    // render finished
    std::shared_ptr<RHIFence>          fence;           // 帧同步
};
```

单线程 bypass 模式下，`FRAMES_IN_FLIGHT` 个 Context 足够循环复用。fence 保证 GPU 先完成上一轮才能 reset CommandBuffer。后期若拆分 RHI 线程且需要并行翻译，才需要池化。

### 7. RHICommandPool — VkCommandPool 薄封装

```cpp
class RHICommandPool : public std::enable_shared_from_this<RHICommandPool>
{
public:
    RHICommandPool(const RHICommandPoolInfo& info) : m_info(info) {}
    virtual void Destroy() = 0;
    virtual std::shared_ptr<RHICommandContext> CreateCommandContext() = 0;
    inline std::shared_ptr<RHIQueue> GetQueue() { return m_info.queue; }

private:
    RHICommandPoolInfo m_info;
};
```

`VulkanRHICommandPool` 内部持有 `VkCommandPool`（带 `VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT`），`CreateCommandContext()` 调 `vkAllocateCommandBuffers` 分配 VkCommandBuffer 然后构造 `VulkanRHICommandContext`。

**不做 Context 池化**（无 idleContexts/ReturnToPool）。Context 数量 = `FRAMES_IN_FLIGHT`，用完循环。

`enable_shared_from_this` 用于 `VulkanRHICommandContext` 能安全引用其 owning pool。

### 8. RHI-native 类型

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

> 注意：`RHIDefinitions.h` 已包含全部类型定义。CLAUDE.md 旧版提到的 `RHIStructs.h` 不再需要——该文件从未创建，所有 enum/struct/Info 都定义在 `RHIDefinitions.h` 中，规模已足够。

VK 枚举 → RHI 枚举的转换函数（在 `VulkanUtil.h` 中）：
```cpp
VkFormat RHIFormatToVkFormat(RHIFormat format);
RHIFormat VkFormatToRHIFormat(VkFormat vkFormat);
VkImageLayout ResourceStateToVkImageLayout(RHIResourceState state);
VkAccessFlags ResourceStateToVkAccessFlags(RHIResourceState state);
VkPipelineStageFlags VkAccessFlagsToVkPipelineStageFlags(VkAccessFlags accessFlags);
```

---

## 三层资产架构：Asset / RenderResource / RHI

```
Source Art         Asset              RenderResource (GPU)   RHI
─────────         ─────              ────────────────────   ───
DamagedHelmet     Model              VertexBuffer           RHIBuffer
  .gltf        →  ├─ SubMeshes[0] → ├─ VertexBuffer(prim) → VkBuffer + VmaAllocation
                  │                 └─ IndexBuffer(idx)   → VkBuffer + VmaAllocation
                  │
                  Material            （无独立 RenderResource）
                  ├─ baseColor=0.8     Material 持有 PBR 参数 +
                  └─ roughness=0.3     shared_ptr<Texture> 引用

                  Texture             （自身即 RenderResource）
                  └─ "path.png" →    LoadFromFile → RHI::CreateTexture + upload
```

| 层 | 目录 | 回答问题 | 依赖 |
|---|---|---|---|
| **Asset** | `asset/` | "这是什么？" — 身份、名称、来源路径、元数据 | RenderResource |
| **RenderResource** | `render/resources/` | "怎么画？" — GPU 端数据组织，可绑定、可 draw | RHI |
| **RHI** | `rhi/` | "Vulkan 是什么？" — 平台无关抽象，纯虚接口 + Vulkan 实现 | extern (volk, VMA) |

**关键区分**：
- Asset 之间通过 `shared_ptr` 引用（同一 `GltfLoadResult` 内同生死）。远期升级为 `AssetId`（uint64 hash）
- RHI 层完全不知道 Asset/Resource 的存在——只管理 Buffer/Texture/Shader 等 GPU 对象
- `VertexBuffer` / `IndexBuffer` 持有 `std::shared_ptr<RHIBuffer>`，不直接碰 VK 句柄
- `Texture` 是特殊的——Asset 和 RenderResource 合二为一：持有文件路径（CPU 信息）和 `RHITexture`/`RHITextureView`（GPU 资源）

---

## 当前实现状态（2026-08-21）

### RHI 层 — ✅ 完整

| 组件 | 状态 |
|---|---|
| `RHI` 抽象工厂 | ✅ Init + Destroy + GetQueue/CreateSurface/CreateSwapchain/CreateCommandPool/CreateSemaphore/CreateFence + CreateBuffer/CreateTexture/CreateTextureView/CreateShader/CreateRootSignature/CreateGraphicsPipeline |
| `RHI::Tick` / 资源生命周期 | ✅ RegisterResource + DeferredResourceDeletes（`m_resourceMap` 每帧延后销毁） |
| `RHIDefinitions.h` | ✅ 全部 enum/struct/Info 类型 |
| `RHIResource` 基类 | ✅ GetType + Destroy |
| `RHICommandContext` | ✅ Begin/End/Submit/ClearColor/Barrier；draw 命令（BeginRenderPass/Draw 等）全部注释待补 |
| `RHICommandContextImmediate` | ✅ RHISubmit + Barrier/CopyBuffer/CopyBufferToTexture/CopyTexture/GenerateMips（资产上传用） |
| `RHICommandList` | ✅ bypass 模式 BeginCommand/EndCommand/Submit/TextureClearColor/TextureBarrier；draw 命令注释待补 |
| `RHISemaphore` / `RHIFence` | ✅ 创建 + Destroy + Wait |
| `RHISwapchain` / `VulkanRHISwapchain` | ✅ AcquireNextTexture + Present |
| `RHIQueue` / `VulkanRHIQueue` | ✅ GetQueue + WaitIdle |
| `VulkanRHI` | ✅ 完整 VK 初始化 + Destroy（逆序销毁） |
| `VulkanRHIResource` | ✅ Buffer, Texture, TextureView, Shader, DescriptorSet, RootSignature, GraphicsPipeline 全部实现 |
| `VulkanRHIGraphicsPipeline` | ✅ 完整实现（dynamic rendering + dynamic state VIEWPORT/SCISSOR） |
| `VulkanRHIFence` | ✅ Wait = vkWaitForFences + vkResetFences |
| `VulkanUtil.h` / `RHIUtil.h` | ✅ 完整 RHI↔VK 转换 + 格式工具 |

### RenderSystem 层 — ✅ 帧循环（仍 Clear-only）

| 组件 | 状态 |
|---|---|
| `RenderSystem::Init()` | ✅ Surface → Swapchain → CommandPool → PerFrameResources → Passes → SceneRenderer |
| `RenderSystem::Tick()` | 🔶 仍 Clear + Present（Clear-only），已接 `SceneRenderer::Process` 但尚未提交 draw |
| `PerFrameRHIResource` | ✅ FRAMES_IN_FLIGHT = 2，cmdContext + startSemaphore + endSemaphore + fence |
| `RenderSystem::Shutdown()` | ✅ 等待 Idle → 销毁 PerFrameResources → Swapchain → Surface |

### Asset / RenderResource / Framework 层 — ✅ 完成

| 组件 | 状态 |
|---|---|
| `Asset` / `AssetManager` | ✅ AssetType + name；name → shared_ptr\<Asset\> 注册表 + ProcessGltfLoadResult |
| `Model` | ✅ SubMesh 列表；Submesh 含 localTransform + Primitive + VertexFactory + VertexBuffer/IndexBuffer + Material |
| `Texture` | ✅ stb_image 加载 → RHI 创建 + staging 上传 + mip 生成 |
| `Material` | ✅ PBR 参数 + 纹理引用 + shader/general slots + pass mask + cull/fill/depth 状态 |
| `Shader` | ✅ SPIR-V 加载（RenderResourceManager::GetOrCreateRHIShader 缓存） |
| `VertexBuffer` / `IndexBuffer` | ✅ CPU 数据 → staging → GPU buffer（interleaved 32B 顶点） |
| `VertexFactory` | ✅ InterleavedVertexFactory + InitRHIDeclaration（position/normal/texcoord） |
| `RenderResourceManager` | ✅ RHIShader 缓存 + per-frame 资源 + m_perFrameRootSignature（待填 UBO） |
| `GltfLoader::Load()` | ✅ 完整：textures → materials → models（position/normal/texcoord/index → interleaved buffer → material 引用） |
| `Scene` / `Node` / Components | ✅ ECS（Transform/Camera/Mesh/DirectionalLight） |
| `SceneRenderer` | ✅ 遍历 Scene → MeshBatch → Pass Processor |
| `MeshPassProcessor` | ✅ Process/BuildMeshDrawCommands：MeshBatch → MeshDrawCommand |
| `MeshDrawCommand` | 🔶 结构完整（streams/indexBuffer/state/firstIndex/numPrimitives），SubmitDraw* 空、m_shaderBindings TODO |
| `ForwardPass` | 🔶 Init 创建 shader + 空 RootSignature（TODO）；Processor 填默认 blend/depth 状态 |
| `GraphicsPipelineState` | ⚠ 空壳（仅持有 m_rhiPipeline），待接 FindOrCreate 缓存 |

---

## 下一步开发计划

### 整体路线

```
✅ RHI 层 + glTF 导入 + Material/Texture/VertexBuffer/Shader 系统（全部完成）
✅ VulkanRHIGraphicsPipeline 创建（dynamic rendering）

🔶 Forward Pass（当前）—— 见「当前阶段目标」的 7 步
  1. 补齐 RHI draw 命令（BeginRenderPass → DrawIndexed）
  2. ForwardPass RootSignature + output 信息
  3. FindOrCreateGraphicsPipelineState 缓存
  4. MeshDrawShaderBindings
  5. MeshDrawCommand::SubmitDraw*
  6. per-frame UBO 上传
  7. RenderSystem::Tick 接 render pass

Forward Pass → glTF 模型渲染到屏幕
```

---

## 类初始化模式

四种初始化模式，按决策树选择：

### 模式 1：Static Factory Init — 有子类/多态

```cpp
class RHI {
    static std::shared_ptr<RHI> g_rhi;
public:
    static std::shared_ptr<RHI> Init(const RHIInfo& info);  // factory
    static std::shared_ptr<RHI>& Get() { return g_rhi; }
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
    VulkanRHICommandContext(VkCommandBuffer cmdBuffer, std::shared_ptr<RHICommandPool> cmdPool);
    // 所有工作在构造里完成，析构归还 VkCommandBuffer
};
```

**什么时候用**：构造参数即全部依赖，创建即就绪。

### 模式 4：全局变量 + trivial 构造

```cpp
class RHICommandList {
    static std::shared_ptr<RHICommandList> g_rhiCmdList;
public:
    static std::shared_ptr<RHICommandList>& Get() { return g_rhiCmdList; }
};
```

**什么时候用**：需要全局访问，无构造依赖。运行时通过 setter 注入配置。

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
| `VulkanRHISwapchain` | 模式 3 | RAII，构造参数足够，构造中完成 VK Swapchain 创建 |
| `Texture` | 模式 3 | RAII，`Texture(path)` 构造即加载并上传 GPU |
| `RenderResourceManager` | 模式 4 | 全局单例（`g_renderResourceManager`），`Init()` 初始化（类似 AssetManager） |
| `SceneRenderer` | 模式 3 | RAII，无外部依赖，`Process(scene)` 逐帧调用 |

---

## 关键技术笔记

### RHISwapchain 创建流程

`VulkanRHISwapchain` 构造中完成全套创建：
1. 查询 Surface Capabilities / Formats / PresentModes
2. 匹配目标 Format（RHIFormat → VkFormat）
3. 选择 PresentMode（MAILBOX 优先，fallback FIFO）
4. 选择 Extent（匹配窗口大小）
5. 创建 `VkSwapchainKHR`
6. 获取 Swapchain Images → 封装为 `VulkanRHITexture`（传入已存在的 `VkImage`）

### Destroy 顺序

`VulkanRHI::Destroy()` 按构造逆序销毁：
```
DescriptorPool → ImmediateCommand → VMA Allocator → Device → Debug Messenger → Instance
```

`RenderSystem::Shutdown()` 先 `WaitIdle` 再销毁 PerFrameResources → CommandPool → Swapchain → Surface。

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
RenderSystem::Tick() 开头 → TextureBarrier(UNDEFINED → TransferDst)
ClearColor 后 → TextureBarrier(TransferDst → Present)
Present 后 → 下一帧 Acquire 返回 PRESENT_SRC_KHR layout 的 image
```

### 全局单例汇总

| 单例 | 变量名 | 定义位置 |
|---|---|---|
| `RHI` | `RHI::g_rhi` | `RHI.cpp` |
| `RHICommandList` | `RHICommandList::g_rhiCmdList` | `RHICommandList.cpp` |
| `Engine` | `Engine::g_engine` | `Engine.cpp` |
| `AssetManager` | `AssetManager::g_assetManager` | `AssetManager.cpp` |
| `RenderResourceManager` | `RenderResourceManager::g_renderResourceManager` | `RenderResourceManager.cpp` |

### VULKAN_RHI() 宏

在 `VulkanUtil.h` 中定义，用于 Vulkan 实现代码中快速获取 `VulkanRHI` 引用：
```cpp
#define VULKAN_RHI() std::static_pointer_cast<VulkanRHI>(RHI::Get())
```

### Bindless（远期）

- 当前 SHZK 使用传统 per-material descriptor set，完全够用
- 每个 material 持有自己的 `RHIDescriptorSet`，draw 前 `vkCmdBindDescriptorSets` 切换
- 远期参考 ToyRenderer/UE5 实现全局 bindless 资源表：所有纹理/采样器注册到 Set 0 大数组中，draw call 间无需切换 descriptor set，仅 push 索引常量
- `VulkanRHI::CreateDescriptorPool()` 已设置 `VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT` 为远期做准备

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

---

## 用户偏好

- **永远不要尝试修改用户的代码**——只回答问题、解释原理、给出建议。用户可以自己改。不要用 Edit/Write/Bash(sed) 等任何方式修改源文件
- **每次回答前阅读并分析用户当前的实现作为前提给出答案**——不要凭记忆，不要假设用户改了或没改什么。必须 Read 相关文件后再回答
- **以工业界成熟引擎的做法为准**——RHI 层优先参考 ToyRenderer（UE 风格，单后端但有完整抽象），RenderResource/RenderSystem 同样参考 ToyRenderer。也要参考 UE5 实际源码（`C:\Users\earn\UE5\`），理解 UE 的实践并判断哪些适合 SHZK 的规模
- **类初始化按决策树选择模式**——有子类用 Static Factory Init（模式 1），依赖外部系统用 Two-Phase Init（模式 2），无依赖用 RAII 构造（模式 3），全局单例无依赖用 trivial 构造 + 运行时 setter 注入（模式 4）。详见上文"类初始化模式"
- System 类（RHI、RenderSystem）使用二段式：`= default` ctor/dtor + `Initialize()` / `Shutdown()`
- Resource 类（RHIResource、VertexBuffer、VulkanRHICommandContext）使用构造函数 RAII 一段式
- 所有 RHI 类型使用 `std::shared_ptr` 管理生命周期
- `*Info` 结构体用于构造参数传递，不搞多参数构造函数
- 成员变量 `m_` 前缀，全局/静态单例 `g_` 前缀
- 每个 .cpp 文件的 `vulkan/` 子目录下，`#include <volk/volk.h>` 必须在第一行
