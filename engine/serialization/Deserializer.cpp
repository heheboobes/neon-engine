#include "Deserializer.h"
#include <cstdio>

namespace engine::serialization {

bool VersionInfo::operator>=(const VersionInfo& other) const {
    if (major != other.major) return major > other.major;
    if (minor != other.minor) return minor > other.minor;
    return patch >= other.patch;
}

std::string VersionInfo::toString() const {
    char buf[48];
    std::snprintf(buf, sizeof(buf), "%u.%u.%u", major, minor, patch);
    return buf;
}

void Deserializer::setErrorCallback(ErrorCallback cb) {
    m_errorCallback = std::move(cb);
}

void Deserializer::reportError(DeserializeError::Type type, const std::string& message,
                               const std::string& field, int64_t offset) {
    DeserializeError err;
    err.type = type;
    err.message = message;
    err.field = field;
    err.offset = offset;
    if (m_errorCallback) {
        m_errorCallback(err);
    }
}

bool Deserializer::readFloatArray(const char* name, float* data, size_t maxCount, size_t& outCount) {
    if (!beginArray(name, outCount)) return false;
    if (outCount > maxCount) {
        reportError(DeserializeError::Type::DataCorruption,
                    "Array count exceeds maximum", name);
        outCount = maxCount;
    }
    for (size_t i = 0; i < outCount; ++i) {
        char elemName[32];
        std::snprintf(elemName, sizeof(elemName), "%zu", i);
        if (!readFloat(elemName, data[i])) {
            reportError(DeserializeError::Type::MissingField, elemName, name);
            return false;
        }
    }
    endArray();
    return true;
}

bool Deserializer::readStringVector(const char* name, std::vector<std::string>& out) {
    size_t count = 0;
    if (!beginArray(name, count)) return false;
    out.resize(count);
    for (size_t i = 0; i < count; ++i) {
        char elemName[32];
        std::snprintf(elemName, sizeof(elemName), "%zu", i);
        if (!readString(elemName, out[i])) return false;
    }
    endArray();
    return true;
}

bool Deserializer::readComponent(const char* name, ComponentData& out) {
    if (!beginObject(name)) return false;
    if (!readString("type", out.typeName)) return false;
    size_t dataSize = 0;
    if (!beginArray("data", dataSize)) return false;
    out.bytes.resize(dataSize);
    for (size_t i = 0; i < dataSize; ++i) {
        char elemName[16];
        std::snprintf(elemName, sizeof(elemName), "%zu", i);
        uint32_t byteVal = 0;
        if (!readUint32(elemName, byteVal)) return false;
        out.bytes[i] = static_cast<uint8_t>(byteVal & 0xFF);
    }
    endArray();
    endObject();
    return true;
}

} // namespace engine::serialization
