#include "Serializer.h"

namespace engine::serialization {

void Serializer::writeFloatArray(const char* name, const float* data, size_t count) {
    beginArray(name, count);
    for (size_t i = 0; i < count; ++i) {
        char elemName[32];
        std::snprintf(elemName, sizeof(elemName), "%zu", i);
        writeFloat(elemName, data[i]);
    }
    endArray();
}

void Serializer::writeInt32Array(const char* name, const int32_t* data, size_t count) {
    beginArray(name, count);
    for (size_t i = 0; i < count; ++i) {
        char elemName[32];
        std::snprintf(elemName, sizeof(elemName), "%zu", i);
        writeInt32(elemName, data[i]);
    }
    endArray();
}

void Serializer::writeStringVector(const char* name, const std::vector<std::string>& values) {
    beginArray(name, values.size());
    for (size_t i = 0; i < values.size(); ++i) {
        char elemName[32];
        std::snprintf(elemName, sizeof(elemName), "%zu", i);
        writeString(elemName, values[i]);
    }
    endArray();
}

void Serializer::writeComponent(const char* name, const ComponentData& comp) {
    beginObject(name);
    writeString("type", comp.typeName);
    beginArray("data", comp.bytes.size());
    for (size_t i = 0; i < comp.bytes.size(); ++i) {
        char elemName[16];
        std::snprintf(elemName, sizeof(elemName), "%zu", i);
        writeUint32(elemName, comp.bytes[i]);
    }
    endArray();
    endObject();
}

} // namespace engine::serialization
