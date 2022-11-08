#pragma once

#include "Serializer.h"
#include <string>
#include <vector>
#include <unordered_map>

namespace engine::serialization {

struct JsonValue;

class JsonSerializer : public Serializer {
public:
    JsonSerializer(bool prettyPrint = true, int indentWidth = 4);
    ~JsonSerializer() override;

    void writeBool(const char* name, bool value) override;
    void writeInt32(const char* name, int32_t value) override;
    void writeUint32(const char* name, uint32_t value) override;
    void writeFloat(const char* name, float value) override;
    void writeDouble(const char* name, double value) override;
    void writeString(const char* name, const std::string& value) override;

    void beginArray(const char* name, size_t count) override;
    void endArray() override;
    void beginObject(const char* name) override;
    void endObject() override;

    std::vector<uint8_t> finalize() override;
    std::string finalizeString();

    void clear();

private:
    struct StackEntry {
        JsonValue* object;
        bool isArray;
        size_t elementCount{0};
    };

    JsonValue* m_root;
    JsonValue* m_current;
    std::vector<StackEntry> m_stack;
    bool m_prettyPrint;
    int m_indentWidth;

    JsonValue* createObject();
    JsonValue* createArray();
    void push(const char* name, JsonValue* val);
    void serializeValue(std::string& out, const JsonValue* val, int depth) const;
    std::string escapeString(const std::string& s) const;
};

} // namespace engine::serialization
