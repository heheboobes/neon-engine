#include "JsonSerializer.h"
#include <sstream>
#include <cstring>

namespace engine::serialization {

enum class JsonType { Null, Bool, Int32, Uint32, Float, Double, String, Object, Array };

struct JsonValue {
    JsonType type{JsonType::Null};
    union {
        bool boolVal;
        int32_t int32Val;
        uint32_t uint32Val;
        float floatVal;
        double doubleVal;
    } data{};
    std::string stringVal;
    std::vector<std::pair<std::string, JsonValue*>> children;
    std::vector<JsonValue*> arrayItems;
};

JsonSerializer::JsonSerializer(bool prettyPrint, int indentWidth)
    : m_prettyPrint(prettyPrint), m_indentWidth(indentWidth) {
    m_root = new JsonValue();
    m_root->type = JsonType::Object;
    m_current = m_root;
}

JsonSerializer::~JsonSerializer() {
    clear();
}

void JsonSerializer::clear() {
    std::vector<JsonValue*> all;
    std::function<void(JsonValue*)> collect = [&](JsonValue* v) {
        for (auto& [_, child] : v->children) collect(child);
        for (auto* item : v->arrayItems) collect(item);
        all.push_back(v);
    };
    collect(m_root);
    for (auto* v : all) delete v;
    m_root = nullptr;
    m_current = nullptr;
    m_stack.clear();
}

JsonValue* JsonSerializer::createObject() {
    auto* v = new JsonValue();
    v->type = JsonType::Object;
    return v;
}

JsonValue* JsonSerializer::createArray() {
    auto* v = new JsonValue();
    v->type = JsonType::Array;
    return v;
}

void JsonSerializer::push(const char* name, JsonValue* val) {
    if (m_current->type == JsonType::Array) {
        m_current->arrayItems.push_back(val);
    } else {
        m_current->children.emplace_back(name ? name : "", val);
    }
}

#define WRITE_IMPL(type, field, jsonType) \
    void JsonSerializer::write##type(const char* name, type value) { \
        auto* v = new JsonValue(); \
        v->type = jsonType; \
        v->data.field = value; \
        push(name, v); \
    }

WRITE_IMPL(Bool, boolVal, JsonType::Bool)
WRITE_IMPL(Int32, int32Val, JsonType::Int32)
WRITE_IMPL(Uint32, uint32Val, JsonType::Uint32)
WRITE_IMPL(Float, floatVal, JsonType::Float)
WRITE_IMPL(Double, doubleVal, JsonType::Double)

void JsonSerializer::writeString(const char* name, const std::string& value) {
    auto* v = new JsonValue();
    v->type = JsonType::String;
    v->stringVal = value;
    push(name, v);
}

void JsonSerializer::beginArray(const char* name, size_t) {
    auto* arr = createArray();
    push(name, arr);
    m_stack.push_back({m_current, m_current->type == JsonType::Array,
                       m_current->type == JsonType::Array ? m_current->arrayItems.size() : m_current->children.size()});
    m_current = arr;
}

void JsonSerializer::endArray() {
    if (!m_stack.empty()) {
        m_current = m_stack.back().object;
        m_stack.pop_back();
    }
}

void JsonSerializer::beginObject(const char* name) {
    auto* obj = createObject();
    push(name, obj);
    m_stack.push_back({m_current, m_current->type == JsonType::Array,
                       m_current->type == JsonType::Array ? m_current->arrayItems.size() : m_current->children.size()});
    m_current = obj;
}

void JsonSerializer::endObject() {
    if (!m_stack.empty()) {
        m_current = m_stack.back().object;
        m_stack.pop_back();
    }
}

std::string JsonSerializer::escapeString(const std::string& s) const {
    std::string out;
    out.reserve(s.size() + 2);
    out.push_back('"');
    for (char c : s) {
        switch (c) {
            case '"':  out += "\\\""; break;
            case '\\': out += "\\\\"; break;
            case '\n': out += "\\n";  break;
            case '\t': out += "\\t";  break;
            case '\r': out += "\\r";  break;
            default:   out.push_back(c);
        }
    }
    out.push_back('"');
    return out;
}

void JsonSerializer::serializeValue(std::string& out, const JsonValue* v, int depth) const {
    std::string indent(depth * m_indentWidth, ' ');
    std::string childIndent((depth + 1) * m_indentWidth, ' ');

    switch (v->type) {
        case JsonType::Null:   out += "null"; break;
        case JsonType::Bool:   out += v->data.boolVal ? "true" : "false"; break;
        case JsonType::Int32:  out += std::to_string(v->data.int32Val); break;
        case JsonType::Uint32: out += std::to_string(v->data.uint32Val); break;
        case JsonType::Float:  out += std::to_string(v->data.floatVal); break;
        case JsonType::Double: out += std::to_string(v->data.doubleVal); break;
        case JsonType::String: out += escapeString(v->stringVal); break;

        case JsonType::Object: {
            out += "{";
            if (m_prettyPrint) out += "\n";
            for (size_t i = 0; i < v->children.size(); ++i) {
                if (m_prettyPrint) out += childIndent;
                out += escapeString(v->children[i].first) + ":";
                if (m_prettyPrint) out += " ";
                serializeValue(out, v->children[i].second, depth + 1);
                if (i < v->children.size() - 1) out += ",";
                if (m_prettyPrint) out += "\n";
            }
            if (m_prettyPrint && !v->children.empty()) out += indent;
            out += "}";
            break;
        }
        case JsonType::Array: {
            out += "[";
            if (m_prettyPrint) out += "\n";
            for (size_t i = 0; i < v->arrayItems.size(); ++i) {
                if (m_prettyPrint) out += childIndent;
                serializeValue(out, v->arrayItems[i], depth + 1);
                if (i < v->arrayItems.size() - 1) out += ",";
                if (m_prettyPrint) out += "\n";
            }
            if (m_prettyPrint && !v->arrayItems.empty()) out += indent;
            out += "]";
            break;
        }
    }
}

std::vector<uint8_t> JsonSerializer::finalize() {
    std::string str = finalizeString();
    return std::vector<uint8_t>(str.begin(), str.end());
}

std::string JsonSerializer::finalizeString() {
    std::string out;
    serializeValue(out, m_root, 0);
    return out;
}

} // namespace engine::serialization
