#pragma once

#include "RHIDefinitions.h"

namespace shzk
{
	class RHIResource
	{
	public:
		RHIResource() = delete;
		RHIResource(RHIResourceType resourceType) : m_resourceType(resourceType) {}
		virtual ~RHIResource() {}

		inline RHIResourceType GetType() { return m_resourceType; }

	private:
		RHIResourceType m_resourceType;

		virtual void Destroy();
	};
}