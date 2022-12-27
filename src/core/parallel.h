#ifndef PARALLEL_H
#define PARALLEL_H

// core/parallel.h*
#include "pbrt.h"
#include "geometry.h"
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>

namespace pbrt {

// Parallel Declarations
class AtomicFloat {
  public:
    // AtomicFloat Public Methods
    explicit AtomicFloat(float v = 0) { bits = FloatToBits(v); }
    operator float() const { return BitsToFloat(bits); }
    float operator=(float v) {
        bits = FloatToBits(v);
        return v;
    }
    void Add(float v) {
#ifdef PBRT_FLOAT_AS_DOUBLE
        uint64_t oldBits = bits, newBits;
#else
        uint32_t oldBits = bits, newBits;
#endif
        do {
            newBits = FloatToBits(BitsToFloat(oldBits) + v);
        } while (!bits.compare_exchange_weak(oldBits, newBits));
    }

  private:
// AtomicFloat Private Data
#ifdef PBRT_FLOAT_AS_DOUBLE
    std::atomic<uint64_t> bits;
#else
    std::atomic<uint32_t> bits;
#endif
};

// Simple one-use barrier; ensures that multiple threads all reach a
// particular point of execution before allowing any of them to proceed
// past it.
//
// Note: this should be heap allocated and managed with a shared_ptr, where
// all threads that use it are passed the shared_ptr. This ensures that
// memory for the Barrier won't be freed until all threads have
// successfully cleared it.
class Barrier {
  public:
    Barrier(int count) : count(count) { CHECK_GT(count, 0); }
    ~Barrier() { isEqual(count, 0); }
    void Wait();

  private:
    std::mutex mutex;
    std::condition_variable cv;
    int count;
};

//extern PBRT_THREAD_LOCAL int ThreadIndex;
int NumSystemCores();

void ParallelFor(std::function<void(int64_t)> func, int64_t count,
                 int chunkSize = 1);
extern __thread int ThreadIndex;
void ParallelFor2D(std::function<void(Point2i)> func, const Point2i &count);
int MaxThreadIndex();
void ParallelCleanup();
void ParallelInit();
void MergeWorkerThreadStats();

}  // namespace pbrt

#endif  // PBRT_CORE_PARALLEL_H