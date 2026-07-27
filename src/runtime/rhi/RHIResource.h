#pragma once

#include "RHIDefinitions.h"

namespace shzk
{
	class RHIResource
	{
	public:
		RHIResource() = delete;
		RHIResource(RHIResourceType resourceType) : m_resourceType(resourceType) {}
		~RHIResource() = default;

		virtual void Destroy() = 0;	// public?

		inline RHIResourceType GetType() { return m_resourceType; }

	private:
		RHIResourceType m_resourceType;
	};
}