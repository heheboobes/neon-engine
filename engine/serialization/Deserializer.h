#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <functional>

namespace engine::serialization {

struct VersionInfo {
    uint32_t major{1};
    uint32_t minor{0};
    uint32_t patch{0};

    bool operator>=(const VersionInfo& other) const;
    std::string toString() const;
};

struct DeserializeError {
    enum class Type {
        None,
        VersionMismatch,
        MissingField,
        TypeMismatch,
        DataCorruption,
        EndOfStream,
        InvalidFormat
    };

    Type type{Type::None};
    std::string message;
    std::string field;
    int64_t offset{0};
};

using ErrorCallback = std::function<void(const DeserializeError&)>;

class Deserializer {
public:
    virtual ~Deserializer() = default;

    void setErrorCallback(ErrorCallback cb);

    virtual bool readBool(const char* name, bool& out) = 0;
    virtual bool readInt32(const char* name, int32_t& out) = 0;
    virtual bool readUint32(const char* name, uint32_t& out) = 0;
    virtual bool readFloat(const char* name, float& out) = 0;
    virtual bool readDouble(const char* name, double& out) = 0;
    virtual bool readString(const char* name, std::string& out) = 0;

    virtual bool beginArray(const char* name, size_t& outCount) = 0;
    virtual void endArray() = 0;
    virtual bool beginObject(const char* name) = 0;
    virtual void endObject() = 0;

    bool readFloatArray(const char* name, float* data, size_t maxCount, size_t& outCount);
    bool readStringVector(const char* name, std::vector<std::string>& out);
    bool readComponent(const char* name, ComponentData& out);

    virtual bool validateVersion(const VersionInfo& expected) = 0;
    virtual bool hasErrors() const = 0;
    virtual std::vector<DeserializeError> getErrors() const = 0;

protected:
    void reportError(DeserializeError::Type type, const std::string& message,
                     const std::string& field = "", int64_t offset = 0);

    ErrorCallback m_errorCallback;
};

} // namespace engine::serialization
