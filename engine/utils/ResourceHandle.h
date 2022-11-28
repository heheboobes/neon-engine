#pragma once

#include <cstddef>
#include <atomic>
#include <cassert>

namespace engine::utils {

class RefCounted {
public:
    RefCounted() : m_refCount(0) {}
    virtual ~RefCounted() = default;

    void addRef() { ++m_refCount; }
    void release() { if (--m_refCount == 0) delete this; }
    int refCount() const { return m_refCount.load(); }

private:
    std::atomic<int> m_refCount;
};

template <typename T>
class ResourceHandle {
public:
    ResourceHandle() : m_resource(nullptr), m_generation(0) {}

    explicit ResourceHandle(T* resource)
        : m_resource(resource), m_generation(resource ? resource->generation() : 0)
    {
        if (m_resource)
            m_resource->addRef();
    }

    ResourceHandle(const ResourceHandle& other)
        : m_resource(other.m_resource), m_generation(other.m_generation)
    {
        if (m_resource)
            m_resource->addRef();
    }

    ResourceHandle(ResourceHandle&& other) noexcept
        : m_resource(other.m_resource), m_generation(other.m_generation)
    {
        other.m_resource = nullptr;
        other.m_generation = 0;
    }

    ResourceHandle& operator=(const ResourceHandle& other)
    {
        if (this != &other) {
            release();
            m_resource = other.m_resource;
            m_generation = other.m_generation;
            if (m_resource)
                m_resource->addRef();
        }
        return *this;
    }

    ResourceHandle& operator=(ResourceHandle&& other) noexcept
    {
        if (this != &other) {
            release();
            m_resource = other.m_resource;
            m_generation = other.m_generation;
            other.m_resource = nullptr;
            other.m_generation = 0;
        }
        return *this;
    }

    ~ResourceHandle() { release(); }

    T* get() const
    {
        if (m_resource && m_resource->generation() == m_generation)
            return m_resource;
        return nullptr;
    }

    T* operator->() const { return get(); }
    T& operator*() const { return *get(); }

    explicit operator bool() const { return get() != nullptr; }

    bool isValid() const
    {
        return m_resource && m_resource->generation() == m_generation;
    }

    uint32_t generation() const { return m_generation; }

    void reset()
    {
        release();
        m_resource = nullptr;
        m_generation = 0;
    }

    bool operator==(const ResourceHandle& other) const
    {
        return m_resource == other.m_resource && m_generation == other.m_generation;
    }

    bool operator!=(const ResourceHandle& other) const { return !(*this == other); }

private:
    void release()
    {
        if (m_resource)
            m_resource->release();
    }

    T* m_resource;
    uint32_t m_generation;
};

template <typename T>
class WeakResourceHandle {
public:
    WeakResourceHandle() : m_resource(nullptr), m_generation(0) {}

    explicit WeakResourceHandle(T* resource)
        : m_resource(resource), m_generation(resource ? resource->generation() : 0) {}

    ResourceHandle<T> lock() const
    {
        if (m_resource && m_resource->generation() == m_generation)
            return ResourceHandle<T>(m_resource);
        return ResourceHandle<T>();
    }

    bool isValid() const
    {
        return m_resource && m_resource->generation() == m_generation;
    }

    void reset() { m_resource = nullptr; m_generation = 0; }

private:
    T* m_resource;
    uint32_t m_generation;
};

struct ResourceBase {
    ResourceBase() : m_generation(1) {}
    virtual ~ResourceBase() = default;
    uint32_t generation() const { return m_generation; }
    void bumpGeneration() { ++m_generation; }
private:
    uint32_t m_generation;
};

} // namespace engine::utils
