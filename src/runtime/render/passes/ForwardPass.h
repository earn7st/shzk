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
		std::shared_ptr<Shader>	m_vertexShader;
		std::shared_ptr<Shader> m_fragShader;

		std::shared_ptr<RHIRootSignature> m_rootSignature;
			

	};

	class ForwardPassProcessor : public MeshPassProcessor
	{
	public:
		ForwardPassProcessor() = delete;
		ForwardPassProcessor(ForwardPass* pass) { m_pass = pass; }
		~ForwardPassProcessor() = default;

		virtual void Init() override final;
		
	protected:
		virtual void AddMeshBatch(const MeshBatch& batch) override final;

	private:
		ForwardPass* m_pass;
	};
}