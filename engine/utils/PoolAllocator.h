#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>
#include <cassert>

namespace engine::utils {

template <typename T, size_t Alignment = alignof(T)>
class PoolAllocator {
public:
    explicit PoolAllocator(size_t capacity);
    ~PoolAllocator();

    PoolAllocator(const PoolAllocator&) = delete;
    PoolAllocator& operator=(const PoolAllocator&) = delete;
    PoolAllocator(PoolAllocator&&) noexcept;
    PoolAllocator& operator=(PoolAllocator&&) noexcept;

    template <typename... Args>
    T* acquire(Args&&... args);

    void release(T* ptr);
    void releaseAll();

    size_t capacity() const { return m_capacity; }
    size_t size() const { return m_freeCount; }
    size_t used() const { return m_capacity - m_freeCount; }
    bool full() const { return m_freeCount == 0; }
    bool empty() const { return m_freeCount == m_capacity; }

    T* operator[](size_t index);
    const T* operator[](size_t index) const;

private:
    struct Block {
        alignas(Alignment) char data[sizeof(T)];
    };

    Block* m_blocks;
    size_t* m_freeList;
    size_t m_capacity;
    size_t m_freeCount;
    size_t m_head;
};

template <typename T, size_t Alignment>
PoolAllocator<T, Alignment>::PoolAllocator(size_t capacity)
    : m_capacity(capacity)
    , m_freeCount(capacity)
    , m_head(0)
{
    m_blocks = static_cast<Block*>(
        ::_aligned_malloc(capacity * sizeof(Block), Alignment));
    m_freeList = new size_t[capacity];
    for (size_t i = 0; i < capacity; ++i)
        m_freeList[i] = i;
}

template <typename T, size_t Alignment>
PoolAllocator<T, Alignment>::~PoolAllocator()
{
    for (size_t i = 0; i < m_capacity; ++i) {
        if (m_freeList[i] == static_cast<size_t>(-1))
            reinterpret_cast<T*>(m_blocks[i].data)->~T();
    }
    delete[] m_freeList;
    ::_aligned_free(m_blocks);
}

template <typename T, size_t Alignment>
PoolAllocator<T, Alignment>::PoolAllocator(PoolAllocator&& other) noexcept
    : m_blocks(other.m_blocks)
    , m_freeList(other.m_freeList)
    , m_capacity(other.m_capacity)
    , m_freeCount(other.m_freeCount)
    , m_head(other.m_head)
{
    other.m_blocks = nullptr;
    other.m_freeList = nullptr;
    other.m_capacity = 0;
    other.m_freeCount = 0;
    other.m_head = 0;
}

template <typename T, size_t Alignment>
auto PoolAllocator<T, Alignment>::operator=(PoolAllocator&& other) noexcept -> PoolAllocator&
{
    if (this != &other) {
        std::swap(m_blocks, other.m_blocks);
        std::swap(m_freeList, other.m_freeList);
        std::swap(m_capacity, other.m_capacity);
        std::swap(m_freeCount, other.m_freeCount);
        std::swap(m_head, other.m_head);
    }
    return *this;
}

template <typename T, size_t Alignment>
template <typename... Args>
T* PoolAllocator<T, Alignment>::acquire(Args&&... args)
{
    if (m_freeCount == 0)
        return nullptr;
    size_t idx = m_freeList[m_head];
    m_freeList[m_head] = static_cast<size_t>(-1);
    m_head = (m_head + 1) % m_capacity;
    --m_freeCount;
    return ::new (m_blocks[idx].data) T(std::forward<Args>(args)...);
}

template <typename T, size_t Alignment>
void PoolAllocator<T, Alignment>::release(T* ptr)
{
    if (!ptr)
        return;
    ptr->~T();
    size_t idx = (reinterpret_cast<Block*>(ptr) - m_blocks);
    m_head = (m_head == 0) ? m_capacity - 1 : m_head - 1;
    m_freeList[m_head] = idx;
    ++m_freeCount;
}

template <typename T, size_t Alignment>
void PoolAllocator<T, Alignment>::releaseAll()
{
    for (size_t i = 0; i < m_capacity; ++i) {
        if (m_freeList[i] == static_cast<size_t>(-1))
            reinterpret_cast<T*>(m_blocks[i].data)->~T();
    }
    m_freeCount = m_capacity;
    m_head = 0;
    for (size_t i = 0; i < m_capacity; ++i)
        m_freeList[i] = i;
}

template <typename T, size_t Alignment>
T* PoolAllocator<T, Alignment>::operator[](size_t index)
{
    if (index >= m_capacity)
        return nullptr;
    return reinterpret_cast<T*>(m_blocks[index].data);
}

template <typename T, size_t Alignment>
const T* PoolAllocator<T, Alignment>::operator[](size_t index) const
{
    if (index >= m_capacity)
        return nullptr;
    return reinterpret_cast<const T*>(m_blocks[index].data);
}

} // namespace engine::utils
