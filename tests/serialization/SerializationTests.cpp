#include "../main.cpp"
#include <engine/serialization/Serializer.h>
#include <engine/serialization/Deserializer.h>
#include <engine/ecs/World.h>
#include <sstream>
#include <string>
#include <cassert>

namespace tests::serialization
{

struct PlayerData
{
    std::string name;
    int level;
    float health;
    float mana;
    bool isAlive;
};

struct InventorySlot
{
    int itemId;
    int count;
    std::string itemName;
};

TEST(Serialization, IntegerRoundtrip)
{
    engine::serialization::Serializer ser;
    ser.writeInt("value", 42);

    std::stringstream ss;
    ser.save(ss);

    engine::serialization::Deserializer des;
    des.load(ss);

    int result = des.readInt("value");
    assert(result == 42);

    return true;
}

TEST(Serialization, FloatRoundtrip)
{
    engine::serialization::Serializer ser;
    ser.writeFloat("pi", 3.14159f);

    std::stringstream ss;
    ser.save(ss);

    engine::serialization::Deserializer des;
    des.load(ss);

    float result = des.readFloat("pi");
    assert(std::abs(result - 3.14159f) < 0.0001f);

    return true;
}

TEST(Serialization, StringRoundtrip)
{
    engine::serialization::Serializer ser;
    ser.writeString("greeting", "Hello, World!");

    std::stringstream ss;
    ser.save(ss);

    engine::serialization::Deserializer des;
    des.load(ss);

    std::string result = des.readString("greeting");
    assert(result == "Hello, World!");

    return true;
}

TEST(Serialization, BoolRoundtrip)
{
    engine::serialization::Serializer ser;
    ser.writeBool("flag", true);

    std::stringstream ss;
    ser.save(ss);

    engine::serialization::Deserializer des;
    des.load(ss);

    bool result = des.readBool("flag");
    assert(result == true);

    return true;
}

TEST(Serialization, MultipleValues)
{
    engine::serialization::Serializer ser;
    ser.writeInt("x", 10);
    ser.writeInt("y", 20);
    ser.writeString("label", "point");

    std::stringstream ss;
    ser.save(ss);

    engine::serialization::Deserializer des;
    des.load(ss);

    assert(des.readInt("x") == 10);
    assert(des.readInt("y") == 20);
    assert(des.readString("label") == "point");

    return true;
}

TEST(Serialization, ObjectRoundtrip)
{
    PlayerData p;
    p.name = "Hero";
    p.level = 5;
    p.health = 80.f;
    p.mana = 30.f;
    p.isAlive = true;

    engine::serialization::Serializer ser;
    ser.beginObject("player");
    ser.writeString("name", p.name);
    ser.writeInt("level", p.level);
    ser.writeFloat("health", p.health);
    ser.writeFloat("mana", p.mana);
    ser.writeBool("alive", p.isAlive);
    ser.endObject();

    std::stringstream ss;
    ser.save(ss);

    engine::serialization::Deserializer des;
    des.load(ss);

    des.beginObject("player");
    PlayerData result;
    result.name = des.readString("name");
    result.level = des.readInt("level");
    result.health = des.readFloat("health");
    result.mana = des.readFloat("mana");
    result.isAlive = des.readBool("alive");
    des.endObject();

    assert(result.name == "Hero");
    assert(result.level == 5);
    assert(std::abs(result.health - 80.f) < 0.0001f);
    assert(result.isAlive == true);

    return true;
}

TEST(Serialization, ArrayRoundtrip)
{
    int data[] = {1, 2, 3, 4, 5};

    engine::serialization::Serializer ser;
    ser.beginArray("numbers");
    for (int v : data)
        ser.writeInt("", v);
    ser.endArray();

    std::stringstream ss;
    ser.save(ss);

    engine::serialization::Deserializer des;
    des.load(ss);

    des.beginArray("numbers");
    for (int i = 0; i < 5; ++i)
    {
        int v = des.readInt("");
        assert(v == data[i]);
    }
    des.endArray();

    return true;
}

}
