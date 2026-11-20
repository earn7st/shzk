# SHZK — Vulkan Renderer

C++20 Vulkan 1.3 实时渲染器。参考 Unreal Engine 和 ToyRenderer (可在父目录/ToyRenderer下找到) 的架构。
RHI 层设计参考 ToyRenderer（UE 风格：FDynamicRHI + IRHICommandContext + FRenderResource）。

> RHI 架构设计 → `src/runtime/rhi/CLAUDE.md`（处理 rhi/ 下文件时自动加载）
> ToyRenderer/UE5 参考文件索引 → skill `rhi-reference`

---

## 三层资产架构：Asset / RenderResource / RHI

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

模式一览：
- 模式 1 Static Factory Init — 基类抽象，运行时决定具体子类，`Init()` 即 factory
- 模式 2 Two-Phase Init — 构造时依赖的系统还未就绪，先 trivial 构造，等依赖可用再 `Init()`
- 模式 3 RAII 构造 — 构造参数即全部依赖，创建即就绪
- 模式 4 全局变量 + trivial 构造 — 需要全局访问、无构造依赖，运行时通过 setter 注入配置

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

### Bindless（远期）

- 当前 SHZK 使用传统 per-material descriptor set，完全够用
- 每个 material 持有自己的 `RHIDescriptorSet`，draw 前 `vkCmdBindDescriptorSets` 切换
- 远期参考 ToyRenderer/UE5 实现全局 bindless 资源表：所有纹理/采样器注册到 Set 0 大数组中，draw call 间无需切换 descriptor set，仅 push 索引常量
- `VulkanRHI::CreateDescriptorPool()` 已设置 `VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT` 为远期做准备

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
