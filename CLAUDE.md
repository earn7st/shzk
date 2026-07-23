# SHZK — Vulkan Renderer

C++20 Vulkan 1.3 实时渲染器。参考 Piccolo（GAMES104 课程）和 Spartan Engine 的架构。
最后更新：2026-07-22

---

## ★ 当前阶段目标（2026-07-22）

### 核心任务：RHI 重写 + Engine 全局化

这是 dev 分支的重大重构，目标：

1. **RHI 重写** — 参考 UE 的 RHI 封装思路重新设计 RHI 层
   - **CommandList 作为核心录制接口**：Device 退居纯工厂，CommandList 承担所有"录制命令"的职责
   - **更清晰的资源句柄**：考虑 shared_ptr / ref-counted 引用，明确资源生命周期
   - **更好的职责分离**：Device = 工厂 + 全局状态（Instance/GPU/VMA），CommandList = 录制 VkCmd，Queue = 提交执行
   - 不需要虚接口（Vulkan only），但封装层次要更接近 UE 的抽象级别
2. **Engine 全局化** — 提供 EngineContext 全局单例
   - 类似 UE 的 `GEngine` 模式，通过全局入口访问子系统
   - EngineContext 持有 Device、AssetRegistry、WindowSystem 等全局实例的引用
   - 避免到处传 shared_ptr，降低耦合

### 后续 Roadmap（RHI 完成后恢复）

```
Resource → Asset → GltfImporter → Pipeline → Descriptor → Camera → Forward Pass
```

详见下方「三层资产架构」和「当前实现状态」。

## 目录结构

```
SHZK/
├── CMakeLists.txt          # 根构建：Vulkan SDK、SDL3、shader 编译
├── shaders/                # GLSL 源码 → bin/shaders/*.spv
├── assets/                 # glTF 模型（DamagedHelmet 等）
├── extern/                 # git submodules: fmt, vk-bootstrap, imgui, fastgltf
└── src/
    ├── CMakeLists.txt      # SHZK_runtime (库) + SHZK_editor (可执行文件)
    ├── editor/             # Editor 入口
    │   ├── Editor.h/.cpp   # 编辑器主循环
    │   └── main.cpp        # WinMain
    └── runtime/            # 引擎 Runtime
        ├── core/           # Engine.h/.cpp, WindowSystem.h/.cpp
        ├── render/         # RenderSystem.h/.cpp
        ├── rendering/      # ⚠ 旧渲染代码（未编译，待删除）
        ├── rhi/            # RHI 封装 — 纯 Vulkan 薄封装
        │   ├── RHIDevice.h/.cpp     # Device — 全局单例资源（Instance/GPU/Surface/VMA）
        │   ├── RHIQueue.h/.cpp      # Queue — command pool + cmdList 轮转
        │   ├── RHICommandList.h/.cpp # CommandList — VkCmd 薄封装（barrier/rendering）
        │   ├── RHISwapchain.h/.cpp  # SwapChain — swapchain + 同步管理
        │   ├── RHIBuffer.h/.cpp     # 🔧 开发中 — VkBuffer + VmaAllocation
        │   ├── RHIUtil.h/.cpp       # VK_CHECK 宏 + CreateImageView helper
        │   └── RHIStructs.h         # SwapchainSupportDetails
        ├── resource/       # ⚠ 旧 GPU 资源代码（未编译，待重写为新架构）
        ├── asset/          # ★ 待创建 — 引擎资产层（AssetRegistry, MeshAsset 等）
        ├── import/         # ★ 待创建 — glTF/glb 导入器
        ├── framework/      # ECS 骨架（Node, Scene, Component, Transform, Mesh）
        └── gltf/           # ⚠ 旧 glTF 加载器（未编译，待移植）
```

## 构建目标

| Target | 类型 | 说明 |
|---|---|---|
| `SHZK_runtime` | 静态库 | 引擎核心，C++20，链接 Vulkan::Vulkan, fmt, vk-bootstrap, imgui, fastgltf |
| `SHZK_editor` | 可执行文件 | 编辑器应用，链接 `SHZK_runtime`（PRIVATE） |

Include 根路径：`src/`（target_include_directories PUBLIC），所以代码中写 `#include "runtime/core/Engine.h"`。

## 架构分层

```
Editor (main.cpp → Editor::Run)
  └─ Engine (Init/Tick/Shutdown)
       ├─ WindowSystem   (SDL3 窗口 + 事件轮询)
       └─ RenderSystem   (→ rhi::Device → rhi::SwapChain + rhi::Queue)
```

**核心模式**：所有系统使用 `= default` 构造/析构 + 显式 `Initialize()` / `Shutdown()`，构造顺序依赖由 Engine::Initialize() 管理。

---

## 三层资产架构：Asset / Resource / RHI

这是 SHZK 的核心设计原则，参考了 Spartan Engine（全局静态 ResourceCache）和 Filament（无全局缓存，Bundle 模式）。

```
Source Art         Asset              Resource (GPU)       RHI
─────────         ─────              ──────────────       ───
DamagedHelmet     MeshAsset          GpuGeometry          rhi::Buffer
  .gltf        →  ├─ SubMeshes[0] → ├─ vertexBuffer →    VkBuffer + VmaAllocation
                  │                 └─ indexBuffer  →    VkBuffer + VmaAllocation
                  │
                  MaterialAsset      GpuMaterial
                  ├─ baseColor=0.8   ├─ uboBuffer →      rhi::Buffer
                  ├─ roughness=0.3   └─ descSet (5 textures)
                  │
                  TextureAsset       GpuTexture
                  ├─ 512x512 RGBA    ├─ image →          rhi::Image
                  └─ sRGB            ├─ imageView
                                     └─ sampler
```

| 层 | 目录 | 回答问题 | 例子 |
|---|---|---|---|
| **Asset** | `asset/` | "这是什么？" — 身份、名称、来源路径、元数据 | `MeshAsset("Helmet", path="...")` |
| **Resource** | `resource/` | "怎么画？" — GPU 端数据，可绑定、可 draw | `GpuGeometry` = vertexBuffer + indexBuffer |
| **RHI** | `rhi/` | "Vulkan 是什么？" — 纯 API 薄封装，不知道 Asset/Resource 存在 | `rhi::Buffer` = VkBuffer + VmaAllocation |

**关键区分**：
- Asset 可以有未加载状态（`LoadState::Unloaded`）。编辑器 Content Drawer 显示的是 Asset 列表。
- Resource 只在 Asset 被加载后才存在。Resource 对用户不可见，只对 RenderSystem 可见。
- RHI 层完全不知道 Asset/Resource——它只管理 VkBuffer/VkImage 等 GPU 对象。
- Asset 之间的引用通过 `AssetId`（uint64 hash），不是裸指针——编辑器中可以安全引用未加载的资产。

### 文件结构总览

```
src/runtime/
├── rhi/                         # RHI 层 — 纯 Vulkan 薄封装
│   ├── RHIBuffer.h/.cpp         ★ VkBuffer + VmaAllocation
│   └── RHIImage.h/.cpp          ★ VkImage + VkImageView + VkSampler
│
├── resource/                    # GPU 资源 — "怎么画的东西"
│   ├── GpuGeometry.h/.cpp       ★ vertexBuffer + indexBuffer (rhi::Buffer)
│   ├── GpuTexture.h/.cpp        ★ image + imageView + sampler (rhi::Image)
│   └── GpuMaterial.h/.cpp       ★ uboBuffer (rhi::Buffer) + VkDescriptorSet
│
├── asset/                       # 资产 — "这是什么东西"
│   ├── Asset.h                  ★ AssetId, AssetType, name, sourcePath, loadState
│   ├── MeshAsset.h/.cpp         ★ SubMesh 列表, 每个指向 GpuGeometry + MaterialAsset
│   ├── MaterialAsset.h/.cpp     ★ PBR 参数 + 纹理引用 (AssetId → TextureAsset)
│   ├── TextureAsset.h/.cpp      ★ 格式/尺寸元数据 + 指向 GpuTexture
│   └── AssetRegistry.h/.cpp     ★ 全局 map<AssetId, unique_ptr<Asset>>, Load/Unload
│
├── import/                      # 导入管线
│   └── GltfImporter.h/.cpp      ★ fastgltf → AssetRegistry + GPU upload
│
├── render/                      # 渲染
│   └── RenderSystem.h/.cpp
├── framework/                   # ECS
├── core/                        # 引擎核心
```

### 命名约定

| 层 | 型命名 | 持有对象 | 可见范围 |
|---|---|---|---|
| `rhi::` | `Buffer`, `Image` | VkBuffer, VkImage | 仅 resource/ 和 import/ 内部使用 |
| `resource::` | `GpuGeometry`, `GpuTexture`, `GpuMaterial` | 上面的 rhi:: 对象 + 元数据 | asset/ 和 render/ 使用 |
| `asset::` | `MeshAsset`, `MaterialAsset`, `TextureAsset` | 上面的 resource:: 对象 + 身份 | engine、editor、renderer 使用 |

**RenderSystem 只能看到 `asset::` 和 `resource::`。`rhi::` 只有 `asset/import/` 内部使用。**

---

## 当前实现状态（2026-07-19）

### ✅ 已完成

| 模块 | 文件 | 说明 |
|---|---|---|
| Editor | `editor/main.cpp`, `Editor.h/.cpp` | 主循环，委托 Engine |
| Engine | `runtime/core/Engine.h/.cpp` | `Tick()` 中调用 `m_renderSystem->Tick()` |
| WindowSystem | `runtime/core/WindowSystem.h/.cpp` | SDL3 窗口 + 事件轮询 |
| **RHI Device** | `runtime/rhi/RHIDevice.h/.cpp` | Instance/GPU/Surface/VMA，Queue/SwapChain，ImmediateSubmit，MemoryCreateBuffer/Image，UploadDataToBuffer/Image |
| **RHI Queue** | `runtime/rhi/RHIQueue.h/.cpp` | 1 pool + 3 cmdList，轮转复用，Submit + Present |
| **RHI CommandList** | `runtime/rhi/RHICommandList.h/.cpp` | Begin/End, ImageBarrier (sync2), BeginRendering/EndRendering, Submit |
| **RHI SwapChain** | `runtime/rhi/RHISwapchain.h/.cpp` | Create/Acquire/Present，per-image semaphore 对 |
| **RHI Buffer** | `runtime/rhi/RHIBuffer.h/.cpp` | ✅ 完整 — 构造/析构 (RAII)，Device 工厂创建，mappable/non-mappable，UploadData |
| **RHI Image** | `runtime/rhi/RHIImage.h/.cpp` | ✅ 完整 — 构造/析构 (RAII)，Device 工厂创建，UploadData (staging + barrier)，ImageView 按需创建 |
| RHI Util | `runtime/rhi/RHIUtil.*` | VK_CHECK + SetBufferDebugName (PFN 版) |
| **RenderSystem** | `runtime/render/RenderSystem.h/.cpp` | Acquire → Barrier → BeginRendering(CLEAR) → EndRendering → Barrier → Submit → Present |
| CMake | `src/CMakeLists.txt` | 所有 RHI 文件已加入编译 |

### RHI 架构（最终版）

```
Device（全局单例资源 + 工厂）
 ├─ VkInstance, VkPhysicalDevice, VkDevice, VmaAllocator
 ├─ VkSurfaceKHR
 ├─ m_graphicsQueue (shared_ptr<Queue>)
 ├─ m_computeQueue (shared_ptr<Queue>)
 ├─ m_swapchain (shared_ptr<SwapChain>)
 ├─ ImmediateSubmit: 1 pool + 1 fence
 ├─ MemoryCreateBuffer / MemoryDestroyBuffer      ← VMA 工厂（Buffer 不直接调 VMA）
 ├─ MemoryCreateImage  / MemoryDestroyImage       ← VMA 工厂（Image 不直接调 VMA）
 ├─ UploadDataToBuffer / UploadDataToImage        ← staging 编排
 └─ CreateExtFunctions() → PFN_vkSetDebugUtilsObjectNameEXT 等动态加载

Queue
 ├─ VkQueue + VkCommandPool（1 个，3 个 cmdList 共享）
 └─ m_cmdLists[3] (shared_ptr<CommandList>)  ← 轮转复用

CommandList
 ├─ VkCommandBuffer（从 Queue pool 分配）
 ├─ VkFence（自管理，Submit signal，复用前 wait）
 └─ state: Idle → Recording → Submitted

SwapChain
 ├─ VkSwapchainKHR + images + imageViews
 ├─ per-image semaphore 对 (imageAcquired + renderComplete)
 └─ semaphoreIndex 轮转

Buffer / Image（Resource 类，构造函数 RAII）
 ├─ 构造 = 创建 GPU 资源，析构 = 销毁 GPU 资源
 ├─ 不可默认构造（Buffer() = delete），可移动
 ├─ VMA 调用不泄漏——通过 Device::MemoryCreate* 间接操作
 └─ Vulkan 类型公开暴露（RHI 是薄封装，不是多后端抽象）
```

**设计决策**：
- **System 类**（Device、Queue、Swapchain、CommandList）：`= default` ctor + `Initialize/Shutdown` 二段式——依赖拓扑复杂，需要 Engine 协调初始化顺序
- **Resource 类**（Buffer、Image、将来的 Shader、Pipeline）：构造函数 RAII 一段式——依赖总是可用（Device 先于一切），不允许空状态
- `Queue` 写死 `VK_SHARING_MODE_EXCLUSIVE`（vk-bootstrap 保证 graphics == present family）
- `CommandList` 自管理 fence，Submit signal，复用前 WaitForExecution
- `Swapchain::semaphoreIndex` 轮转，Present 后推进
- **VMA 不暴露给 Resource 类**：`Buffer::CreateResource` 调 `Device::MemoryCreateBuffer`，不直接调 `vmaCreateBuffer`
- **不用 RHIFormat / RHIExtent 等包装枚举**：`VkFormat`、`VkExtent3D` 直接暴露——没有多后端需求时不创造别名
- **`RHIBufferType` 有意义**：一个值推导 `VkBufferUsageFlags` + `VkMemoryPropertyFlags` 两个 Vulkan 设置，不是 1:1 映射

---

## 下一步开发计划

### 整体路线（2026-07-19 更新）

```
✅ RHI Buffer ──→ ✅ RHI Image ──→ ★ Resource ──→ Asset ──→ GltfImporter
                                       │              │
                                  GpuGeometry     MeshAsset
                                  GpuTexture      TextureAsset
                                                  MaterialAsset
                                                  AssetRegistry
```

之后才进入渲染管线：
```
GltfImporter 完成 → Pipeline → Descriptor → Camera → Forward Pass
```

### 当前阶段：Asset System（4 个 Phase）

---

### Phase 1：Resource 层

**内容**：
- `GpuGeometry`：持有 `rhi::Buffer` vertex + index buffer，记录 vertexCount / indexCount
- `GpuTexture`：持有 `rhi::Image`，记录格式和尺寸元数据

**新文件**：`src/runtime/resource/GpuGeometry.h/.cpp`, `GpuTexture.h/.cpp`

**注意**：`GpuMaterial` 暂时不创建——它依赖 DescriptorSet，等 Pipeline/Descriptor 做完再补。`GpuGeometry` 和 `GpuTexture` 不依赖任何未完成的模块，可以立即做。

---

### Phase 2：Asset 骨架

**内容**：
- `Asset` 基类：`AssetId` (uint64 hash), `AssetType` enum, `name`, `sourcePath`, `LoadState`
- `AssetRegistry`：全局 `map<AssetId, unique_ptr<Asset>>`，按 name/type 查找，`Load`/`Unload` 管理生命周期

**新文件**：`src/runtime/asset/Asset.h/.cpp`, `AssetRegistry.h/.cpp`

---

### Phase 3：具体 Asset 类型

**内容**：
- `MeshAsset`：`vector<SubMesh>`，每个 SubMesh 持有 `GpuGeometry` + 指向 MaterialAsset 的 AssetId
- `TextureAsset`：格式/尺寸元数据 + 持有 `GpuTexture`
- `MaterialAsset`：PBR 参数（baseColorFactor, metallicFactor, roughnessFactor 等）+ 纹理引用（baseColorTexture → AssetId of TextureAsset）。**不绑定 GPU 资源**——等 Descriptor 做完再补 `GpuMaterial`

**新文件**：`src/runtime/asset/MeshAsset.h/.cpp`, `TextureAsset.h/.cpp`, `MaterialAsset.h/.cpp`

---

### Phase 4：glTF Importer

**内容**：
- `GltfImporter::Import(path, device, registry)`：
  1. fastgltf 解析 glTF/glb
  2. images → `Device::UploadDataToImage` → GpuTexture → TextureAsset → registry
  3. materials → MaterialAsset（PBR 参数 + 纹理 AssetId 引用）→ registry
  4. meshes → vertices/indices → `Device::UploadDataToBuffer` → GpuGeometry → MeshAsset → registry
  5. scene nodes → framework::Node 层级（含 MeshComponent + Transform）

**新文件**：`src/runtime/import/GltfImporter.h/.cpp`

**注意**：Phase 4 期间遇到 RHI 层不完善的地方随时修补（如需要 Map/Unmap、Buffer resize 等）。

---

### 后续（Asset 完成后）

| 顺序 | 模块 | 说明 |
|---|---|---|
| 5 | Pipeline | `rhi::Shader` + `rhi::Pipeline`，参考 Spartan hash 缓存模式 |
| 6 | Descriptor | DescriptorSetLayout + Pool + Set，初期 set=0: FrameUBO, set=1: MaterialUBO+textures |
| 7 | Camera + FrameUBO | Camera 类 + FrameUniforms struct |
| 8 | GpuMaterial | 补上 MaterialAsset 的 GPU 绑定（UBO + DescriptorSet） |
| 9 | Framework 修复 | MeshComponent, TransformComponent, Scene |
| 10 | Forward Pass | 打通完整的 RenderScene：遍历 Scene → bind pipeline → bind descriptor → DrawIndexed

---

### RenderFrame 完整流程（远期）

```
RenderSystem::Tick()
  ├─ 1. Queue::NextCommandList()
  ├─ 2. SwapChain::AcquireNextImage()
  ├─ 3. Begin command buffer
  ├─ 4. ImageBarrier: UNDEFINED/PRESENT → COLOR_ATTACHMENT_OPTIMAL
  ├─ 5. BeginRendering (CLEAR color + depth)
  ├─ 6. Update frame UBO + bind pipeline
  ├─ 7. For each Node in Scene:
  │     ├─ Push constants (model matrix + materialIndex)
  │     ├─ Bind vertex/index buffer (通过 GpuGeometry)
  │     └─ DrawIndexed
  ├─ 8. EndRendering
  ├─ 9. ImageBarrier: COLOR_ATTACHMENT_OPTIMAL → PRESENT_SRC_KHR
  └─ 10. Submit + Present
```

---

## 关键技术笔记

### Barrier 转换时机

```
AcquireNextImage → image layout = UNDEFINED (首帧) / PRESENT_SRC_KHR (后续)
ProduceFrame 开头 → ImageBarrier(UNDEFINED → COLOR_ATTACHMENT_OPTIMAL)
SubmitAndPresent 中 → EndRendering() 后 → ImageBarrier(COLOR_ATTACHMENT_OPTIMAL → PRESENT_SRC_KHR)
```

在 `vkCmdBeginRendering` 开启的 render pass 内部，不能调用带 image barrier 的 `vkCmdPipelineBarrier2`（Vulkan 规范限制）。

### Mappable Buffer

| VmaMemoryUsage | Mappable? | GPU 访问速度 | 用途 |
|---|---|---|---|
| `GPU_ONLY` | ❌ | 极快（VRAM） | vertex/index buffer, texture, RT |
| `CPU_TO_GPU` | ✅ | 慢（PCIe 读） | staging buffer, uniform buffer |
| `CPU_ONLY` | ✅ | 不可用 | 纯 CPU 暂存 |
| `GPU_TO_CPU` | ✅ | 慢（PCIe 写） | readback（少见） |

### Instance Buffer

两种实现：
1. **Vertex Buffer (传统)**：`VK_VERTEX_INPUT_RATE_INSTANCE`，instance 数据通过 vertex attribute 流入，mat4 占 4 个 slot
2. **Storage Buffer (现代 bindless)**：SSBO 存储实例数据，shader 通过 `gl_InstanceIndex` 手动索引

SHZK 当前用 push constants 传 model matrix（最简方案），无需 instance buffer。

### Spartan Pipeline 管理

- 不在启动时预设 pipeline——每个 pass 运行时在栈上构建 `RHI_PipelineState`，按 `hash → GetOrCreatePipeline` 懒加载
- Hash 组合：primitiveTopology + swapchainFormat + rasterizer + blend + depthStencil + shader × N + RT objectId × N
- 全局 `VkPipelineCache` 持久化到磁盘（`pipeline_cache.bin`）
- 关键文件：`RHI_PipelineState.h/.cpp`、`Vulkan_Device.cpp:2195`、`Vulkan_Pipeline.cpp:95`

### Spartan Asset System

- 全局静态 `ResourceCache` singleton，`Load<T>(path)` 按路径缓存去重，返回 `shared_ptr<T>`
- 所有资产继承 `IResource`（ResourceType 枚举 + LoadFromFile/SaveToFile 虚函数）
- Render Component 持有裸指针指向缓存中的资源（缓存不释放，指针始终有效）
- 全局共享 `GeometryBuffer`：所有 mesh 的顶点/索引 append 到一个大 buffer，每个 mesh 只记 offset

### Geometry Buffer（Spartan 简化版）

- CPU 累积器（`vector<Vertex>`, `vector<uint32_t>`）+ GPU buffer 对
- Append 接口返回全局 offset
- BuildIfDirty 三路径：首次创建 / 增量上传 / 溢出重建
- growth_factor 1.25x：首次分配余量，避免每次加载都重建
- SHZK 初期不需要——每个 Geometry 独立 `rhi::Buffer`，远期再做全局池

### Bindless

- 所有纹理平铺在全局 `Textures[]` 数组，全帧 bind 一次
- 所有材质参数打包成一个 `MaterialsBuffer`（SSBO）
- Draw 时只 push materialIndex + geomOffset → shader 自行索引
- 不会减少 draw call 数量——它省的是 `vkCmdBindDescriptorSets` 和 `vkCmdBindVertexBuffers`
- SHZK 初期每个 material 一个 descriptor set，完全够用

---

### ⚠ 旧代码（未编译）

- `rendering/` — 旧 Renderer, SceneRenderer, Pipeline, PipelineBuilder, DescriptorAllocator 等
- `resource/` — 旧 ResourceManager, Geometry, Material, Mesh, Texture（用 `vk::`）
- `gltf/` — 旧 gltfLoader

以上模块在新 RHI 层完善后逐步重写，替换旧 `vk::` / `VulkanContext` 为 `rhi::*`。

---

## 参考项目

| | Piccolo | Spartan Engine | Filament |
|---|---|---|---|
| 位置 | `C:\Users\earn\workspace\Piccolo` | `C:\Users\earn\workspace\SpartanEngine` | `C:\Users\earn\workspace\filament` |
| 渲染 | Deferred (8 subpass), GLSL | GPU-Driven bindless, HLSL | Deferred, custom material system |
| RHI | 虚接口 `RHI` → `VulkanRHI` | 具体类，编译期选 D3D12/Vulkan | 虚接口 backend，各平台独立实现 |
| Editor/Runtime | `PiccoloRuntime` 库 + `PiccoloEditor` | `runtime/` + `editor/` 严格分离 | 纯库，无内置编辑器 |
| 资产系统 | 编辑器内置导入 + 自定义格式 | 全局静态 ResourceCache + IResource | 两层分离 AssetLoader/ResourceLoader |
| Pipeline 管理 | 每个 material 一个 pipeline | PSO hash → 全局缓存 GetOrCreatePipeline | 预编译 material package (.filamat) |

## 用户偏好

- **不要直接修改代码**——提供代码示例或口头指导，让用户自己改
- **每次回答前完整阅读当前实现的代码**——不要凭记忆，不要假设用户改了或没改什么
- **以工业界成熟引擎的做法为准**——优先参考 Spartan Engine（单后端 RHI 封装模式匹配 SHZK），其次 Filament（异步 Driver、两层 Asset/Resource），再参考具体问题的行业惯例。同时考虑 SHZK 当前阶段的复杂度是否值得——不是所有工业界做法都适用于个人项目
- System 类（Device、Queue、Swapchain、CommandList）使用 `= default` ctor/dtor + `Initialize()` / `Shutdown()` 二段式
- Resource 类（Buffer、Image、将来的 Shader、Pipeline）使用构造函数 RAII 一段式，`= delete` 默认构造
- `EngineSpec` 嵌套 `WindowSpec` 等配置结构体的组合模式
- SDL_Window 通过前向声明避免头文件传染，SDL3 header 仅在 `.cpp` 中包含
