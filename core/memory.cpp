#include "memory.h"

namespace pbrt {

// Memory Allocation Functions
void *AllocAligned(size_t size) {
#if defined(PBRT_HAVE__ALIGNED_MALLOC)
    return _aligned_malloc(size, PBRT_L1_CACHE_LINE_SIZE);
#else
    void *ptr;
    if (posix_memalign(&ptr, PBRT_L1_CACHE_LINE_SIZE, size) != 0) ptr = nullptr;
    return ptr;
#endif
}

void FreeAligned(void *ptr) {
    if (!ptr) return;
#if defined(PBRT_HAVE__ALIGNED_MALLOC)
    _aligned_free(ptr);
#else
    free(ptr);
#endif
}

}  // namespace pbrt