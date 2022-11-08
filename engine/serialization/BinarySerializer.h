#pragma once

#include "Serializer.h"
#include <vector>
#include <cstdint>
#include <string>

namespace engine::serialization {

struct BinaryHeader {
    char magic[4]{'B', 'I', 'N', '1'};
    uint32_t version{1};
    uint32_t flags{0};
    uint32_t uncompressedSize{0};
    uint32_t compressedSize{0};
    uint32_t crc32{0};
};

enum BinaryFlags : uint32_t {
    BIN_FLAG_COMPRESSED = 1 << 0,
    BIN_FLAG_CHECKSUM   = 1 << 1,
};

class BinarySerializer : public Serializer {
public:
    BinarySerializer(bool enableChecksum = true, bool enableCompression = false);
    ~BinarySerializer() override;

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

    void setCompressionLevel(int level);

private:
    std::vector<uint8_t> m_buffer;
    std::vector<size_t> m_blockStack;
    bool m_enableChecksum;
    bool m_enableCompression;
    int m_compressionLevel{6};

    void writeRaw(const void* data, size_t size);
    void writeTag(uint8_t typeId, const char* name);
    uint32_t calculateCRC32(const uint8_t* data, size_t size) const;
    std::vector<uint8_t> compress(const uint8_t* data, size_t size) const;
    std::vector<uint8_t> buildHeader();
};

} // namespace engine::serialization
