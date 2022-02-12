#include <iostream>
#include <string>
#include <vector>
#include <chrono>

namespace tests
{

struct TestCase
{
    const char* suite;
    const char* name;
    bool (*func)();
};

static std::vector<TestCase>& registry()
{
    static std::vector<TestCase> instances;
    return instances;
}

struct Registrar
{
    Registrar(const char* suite, const char* name, bool (*func)())
    {
        registry().push_back({suite, name, func});
    }
};

#define TEST(suite, name) \
    static bool suite##_##name(); \
    static tests::Registrar reg_##suite##_##name(#suite, #name, suite##_##name); \
    static bool suite##_##name()

void runAll()
{
    int passed = 0, failed = 0;
    auto start = std::chrono::steady_clock::now();

    for (const auto& test : registry())
    {
        std::cout << "[" << test.suite << "] " << test.name << "... ";
        bool ok = test.func();
        std::cout << (ok ? "PASSED" : "FAILED") << "\n";
        if (ok) ++passed; else ++failed;
    }

    auto end = std::chrono::steady_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    std::cout << "\n========================================\n";
    std::cout << "Results: " << passed << " passed, "
              << failed << " failed, "
              << (passed + failed) << " total\n";
    std::cout << "Time: " << ms << "ms\n";
    std::cout << "========================================\n";
}

}

int main()
{
    tests::runAll();
    return 0;
}
