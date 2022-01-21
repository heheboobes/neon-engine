#ifndef ENGINE_CORE_SERVICE_LOCATOR_H
#define ENGINE_CORE_SERVICE_LOCATOR_H

#include <unordered_map>
#include <memory>
#include <typeindex>
#include <typeinfo>
#include <cassert>

namespace engine {
namespace core {

class ServiceLocator {
public:
    template<typename T>
    static void Register(std::unique_ptr<T> service)
    {
        static_assert(std::is_base_of<ServiceBase, T>::value,
                      "Service must derive from ServiceBase");
        auto& instance = GetInstance();
        std::type_index index = std::type_index(typeid(T));
        instance.m_services[index] = std::move(service);
    }

    template<typename T>
    static T& Get()
    {
        auto& instance = GetInstance();
        std::type_index index = std::type_index(typeid(T));
        auto it = instance.m_services.find(index);
        assert(it != instance.m_services.end() && "Service not registered");
        return static_cast<T&>(*it->second);
    }

    template<typename T>
    static void Unregister()
    {
        auto& instance = GetInstance();
        std::type_index index = std::type_index(typeid(T));
        instance.m_services.erase(index);
    }

    template<typename T>
    static bool Has()
    {
        auto& instance = GetInstance();
        std::type_index index = std::type_index(typeid(T));
        return instance.m_services.find(index) != instance.m_services.end();
    }

    static void ClearAll();
    static size_t GetServiceCount();

    struct ServiceBase {
        virtual ~ServiceBase() = default;
    };

private:
    ServiceLocator() = default;
    ~ServiceLocator() = default;
    ServiceLocator(const ServiceLocator&) = delete;
    ServiceLocator& operator=(const ServiceLocator&) = delete;

    static ServiceLocator& GetInstance();

    std::unordered_map<std::type_index, std::unique_ptr<ServiceBase>> m_services;
};

} // namespace core
} // namespace engine

#endif // ENGINE_CORE_SERVICE_LOCATOR_H
