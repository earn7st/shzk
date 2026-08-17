#include "RHIDefinitions.h"
#include "RHIDefinitions.h"
#include "runtime/log/Log.h"

namespace shzk
{
	RHIRootSignatureInfo& RHIRootSignatureInfo::AddEntry(const ShaderResourceEntry& entry)
	{
        for (ShaderResourceEntry& oldEntry : entries)
        {
            if (oldEntry.set == entry.set &&
                oldEntry.binding == entry.binding)
            {
                if (oldEntry.type != entry.type)
                {
                    SHZK_LOG_ERROR("Conflict shader resource entry!");
                }
                else
                {
                    /*
                    if ((oldEntry.size == 0 || entry.size == 0) && (oldEntry.size == 1 || entry.size == 1))  oldEntry.size = 0;
                    else oldEntry.size = std::max(oldEntry.size, entry.size);
                    */
                    oldEntry.frequency |= entry.frequency;
                }
                return *this;
            }
        }

        entries.push_back(entry);
        return *this;
	}

    RHIRootSignatureInfo& RHIRootSignatureInfo::AddEntry(const RHIRootSignatureInfo& other)
    {
        for (auto& entry : other.entries) AddEntry(entry);
        return *this;
    }

}
