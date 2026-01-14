#ifndef COMMON_MEMORY_POOLS_H
#define COMMON_MEMORY_POOLS_H



// Dynamic memory pool (grows as needed)
// Confidence: 30% - Speculative based on common pool patterns
// Error count: 468
template<typename T, size_t BlockSize = 4096>
class DynamicPool {
public:
    DynamicPool();
    ~DynamicPool();

    T* allocate();
    void deallocate(T* ptr);
    void clear();

private:
    struct Block {
        T data[BlockSize];
        Block* next;
    };

    Block* head;
    T* freeList;
    size_t totalAllocated;      // Track total allocations
    size_t peakUsage;           // Peak memory usage
    /* Implementation: Free list with block-based growth
     * allocate(): Pop from freeList, or allocate new block if empty
     * deallocate(): Push to freeList head
     * clear(): Reset all blocks, rebuild freeList */
};

// Static memory pool (fixed size)
// Confidence: 30% - Speculative based on common pool patterns
// Error count: 84
template<typename T, size_t PoolSize>
class StaticPool {
public:
    StaticPool();
    ~StaticPool();

    T* allocate();
    void deallocate(T* ptr);
    bool isFull() const;
    size_t available() const;

private:
    T pool[PoolSize];
    bool used[PoolSize];
    size_t allocated;
    size_t nextFree;            // Hint for next free slot
    /* Implementation: Fixed-size pool with bitmap tracking
     * allocate(): Scan used[] from nextFree, return first free slot
     * deallocate(): Mark used[idx]=false, update nextFree if lower
     * Thread-safe version requires mutex or atomic bitmap */
};

#endif // COMMON_MEMORY_POOLS_H
