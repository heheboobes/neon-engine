#pragma once

#include <string>
#include <vector>
#include <cstdint>

namespace engine::serialization {

struct ComponentData {
    std::string typeName;
    std::vector<uint8_t> bytes;
};

class Serializer {
public:
    virtual ~Serializer() = default;

    virtual void writeBool(const char* name, bool value) = 0;
    virtual void writeInt32(const char* name, int32_t value) = 0;
    virtual void writeUint32(const char* name, uint32_t value) = 0;
    virtual void writeFloat(const char* name, float value) = 0;
    virtual void writeDouble(const char* name, double value) = 0;
    virtual void writeString(const char* name, const std::string& value) = 0;

    virtual void beginArray(const char* name, size_t count) = 0;
    virtual void endArray() = 0;
    virtual void beginObject(const char* name) = 0;
    virtual void endObject() = 0;

    void writeFloatArray(const char* name, const float* data, size_t count);
    void writeInt32Array(const char* name, const int32_t* data, size_t count);
    void writeStringVector(const char* name, const std::vector<std::string>& values);
    void writeComponent(const char* name, const ComponentData& comp);

    virtual std::vector<uint8_t> finalize() = 0;

protected:
    int32_t m_indent{0};
};

} // namespace engine::serialization
