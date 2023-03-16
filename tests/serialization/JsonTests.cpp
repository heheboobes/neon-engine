#include "../main.cpp"
#include <engine/serialization/JsonSerializer.h>
#include <engine/serialization/JsonDeserializer.h>
#include <sstream>
#include <string>
#include <cassert>

namespace tests::serialization
{

TEST(Json, SerializeEmptyObject)
{
    engine::serialization::JsonSerializer ser;
    ser.beginObject("root");
    ser.endObject();

    std::stringstream ss;
    ser.save(ss);

    std::string json = ss.str();
    assert(json.find('{') != std::string::npos);
    assert(json.find('}') != std::string::npos);

    return true;
}

TEST(Json, SerializePrimitives)
{
    engine::serialization::JsonSerializer ser;
    ser.beginObject("test");
    ser.writeInt("intVal", -42);
    ser.writeFloat("floatVal", 3.14f);
    ser.writeString("strVal", "hello");
    ser.writeBool("boolVal", true);
    ser.endObject();

    std::stringstream ss;
    ser.save(ss);

    std::string json = ss.str();
    assert(json.find("-42") != std::string::npos);
    assert(json.find("hello") != std::string::npos);
    assert(json.find("true") != std::string::npos);

    return true;
}

TEST(Json, DeserializePrimitives)
{
    std::string json = R"({
        "intVal": 99,
        "floatVal": 2.718,
        "strVal": "world",
        "boolVal": false
    })";

    std::stringstream ss(json);
    engine::serialization::JsonDeserializer des;
    des.load(ss);

    assert(des.readInt("intVal") == 99);
    assert(std::abs(des.readFloat("floatVal") - 2.718f) < 0.001f);
    assert(des.readString("strVal") == "world");
    assert(des.readBool("boolVal") == false);

    return true;
}

TEST(Json, RoundtripObject)
{
    engine::serialization::JsonSerializer ser;
    ser.beginObject("player");
    ser.writeString("name", "Alice");
    ser.writeInt("hp", 100);
    ser.writeFloat("xp", 250.5f);
    ser.writeBool("alive", true);
    ser.endObject();

    std::stringstream ss;
    ser.save(ss);

    engine::serialization::JsonDeserializer des;
    des.load(ss);

    des.beginObject("player");
    assert(des.readString("name") == "Alice");
    assert(des.readInt("hp") == 100);
    assert(std::abs(des.readFloat("xp") - 250.5f) < 0.001f);
    assert(des.readBool("alive") == true);
    des.endObject();

    return true;
}

TEST(Json, RoundtripArray)
{
    std::vector<int> expected = {10, 20, 30, 40, 50};

    engine::serialization::JsonSerializer ser;
    ser.beginArray("values");
    for (int v : expected)
        ser.writeInt("", v);
    ser.endArray();

    std::stringstream ss;
    ser.save(ss);

    engine::serialization::JsonDeserializer des;
    des.load(ss);

    std::vector<int> actual;
    des.beginArray("values");
    for (size_t i = 0; i < expected.size(); ++i)
        actual.push_back(des.readInt(""));
    des.endArray();

    assert(actual.size() == expected.size());
    for (size_t i = 0; i < expected.size(); ++i)
        assert(actual[i] == expected[i]);

    return true;
}

TEST(Json, NestedObject)
{
    engine::serialization::JsonSerializer ser;
    ser.beginObject("scene");
    ser.writeString("name", "Level1");
    ser.beginObject("camera");
    ser.writeFloat("x", 0.f);
    ser.writeFloat("y", 0.f);
    ser.writeFloat("zoom", 1.f);
    ser.endObject();
    ser.endObject();

    std::stringstream ss;
    ser.save(ss);

    engine::serialization::JsonDeserializer des;
    des.load(ss);

    des.beginObject("scene");
    assert(des.readString("name") == "Level1");
    des.beginObject("camera");
    assert(std::abs(des.readFloat("x")) < 0.001f);
    assert(std::abs(des.readFloat("zoom") - 1.f) < 0.001f);
    des.endObject();
    des.endObject();

    return true;
}

TEST(Json, MissingKey)
{
    std::string json = R"({"a": 1})";
    std::stringstream ss(json);
    engine::serialization::JsonDeserializer des;
    des.load(ss);

    bool found = des.hasKey("a");
    assert(found);

    found = des.hasKey("b");
    assert(!found);

    return true;
}

}
