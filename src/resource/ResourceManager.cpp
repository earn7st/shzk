#include "ResourceManager.h"

void ResourceManager::UnloadAll() {
    // Emergency cleanup method for system shutdown or major state changes
    for (auto& [type, typeResources] : resources) {
        for (auto& [id, resource] : typeResources) {
            resource->Unload();     // Ensure all resources clean up properly
        }
        typeResources.clear();      // Clear type-specific containers
    }
    refCounts.clear();              // Reset all reference counts
}