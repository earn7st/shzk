#pragma once

#include "MeshPass.h"
#include "MeshPassProcessor.h"
#include "runtime/asset/Shader.h"

namespace shzk
{
	class ForwardPass : public MeshPass
	{
	public:
		ForwardPass() : MeshPass(PassType::Forward) {}
		~ForwardPass() = default;

		virtual void Init() override final;

	protected:
		Shader	m_vertexShader;
		Shader	m_fragShader;

		
	};

	class ForwardPassProcessor : public MeshPassProcessor
	{
	public:
		ForwardPassProcessor() = delete;
		ForwardPassProcessor(ForwardPass* pass) { m_pass = pass; }
		~ForwardPassProcessor() = default;

		virtual void Init() override final;
		
	protected:
		virtual void AddMeshBatch() override final;

	private:
		ForwardPass* m_pass;
	};
}