#ifndef COMMON_CONTAINERS_H
#define COMMON_CONTAINERS_H

#include <vector>
#include <unordered_map>
#include <string>

// Random list container
// Confidence: 40%
// Error count: 660
template<typename T>
class RandomList {
public:
    void add(T item);
    T getRandom();
    void clear();
    size_t size() const;

private:
    std::vector<T> items;
};

// String unordered map alias
// Confidence: 60%
// Error count: 72
typedef std::unordered_map<std::string, std::string> StringUnorderedMap;

// Generic type placeholders (likely decompiler artifacts)
// Confidence: 5%
// Error count: 60 + 36 = 96
typedef void* a1;
typedef void* a2;

#endif // COMMON_CONTAINERS_H
