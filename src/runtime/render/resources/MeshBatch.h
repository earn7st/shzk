#pragma once

#include <memory>
#include <vector>
#include <glm/glm.hpp>

namespace shzk
{
	class VertexBuffer;
	class IndexBuffer;
	class Material;

// Unreal 中 MeshBatch 维护的是一组拥有相同的 VertexFactory + Material 的 Mesh，MeshBatchElement 中存储 Index 信息
// MeshBatch 的最根本的意义是希望减少 Pipeline 的切换，于是将不会导致 Pipeline 切换的 Mesh 放到一起
//		大概可以理解成，Vertex Factory 包含的信息会导致 vertex shader 中的顶点输入不同，比如 SkeletalMesh 相比 StaticMesh 可能要多出 boneID 和 boneWeight 信息，存在 layout(location=n) 的位置等等
//		这必定会导致 Shader 不同（正经引擎中 Shader 是通过 Material 和 VertexFactory 信息生成的），那么 Pipeline 肯定不同
//		至于 Material，一方面是 cullMode, fillMode, depthTest,blendState 这些都会影响 Pipeline，另外 Material 有可能包含用户自定义的 Shader

//	目前的实现先抛弃 VertexFactory 的抽象，之后做动画的时候再加上，现在统一用 interleaved vertex buffer 的布局
//	另外，其实如果不写用户自定义 Shader 的话，VertexFactory 也不一定要做 Key 的一部分，设计个通用的 layout 就可以(?)
//	仿照 ToyRenderer 的做法，最后在 BuildDrawCommand 的时候直接拿 PipelineState(cache一下) 做分组

	typedef struct MeshBatchElement
	{
		std::shared_ptr<VertexBuffer>	vertexBuffer;
		std::shared_ptr<IndexBuffer>	indexBuffer;
		uint32_t firstIndex = 0;
		uint32_t indexCount = 0;
		uint32_t baseVertexIndex = 0;
		glm::mat4 modelMatrix = glm::mat4(1.0f);
	} MeshBatchElement;
	
// 目前不能算作 batch，只是有一个元素的 batch
	typedef struct MeshBatch
	{
		// TODO: vertex factory?
		// std::vector<MeshBatchElement>	elements;
		MeshBatchElement			element;
		std::shared_ptr<Material>	material;
	} MeshBatch;
}
