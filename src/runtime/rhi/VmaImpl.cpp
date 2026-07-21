// VMA Implementation — exactly one translation unit in the project.
// VMA is designed to be included twice in the same TU:
//   1. First: declarations (via headers that use VMA types)
//   2. Second: #define VMA_IMPLEMENTATION then #include again for definitions
//
// Since the header <vma/vk_mem_alloc.h> uses a traditional include guard,
// VMA_IMPLEMENTATION must be defined BEFORE the first inclusion in this TU.

#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.h>
