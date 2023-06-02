# Getting Started

## Prerequisites

- CMake 3.25+
- C++20 compiler (MSVC 2022, GCC 13+, Clang 16+)
- Vulkan SDK 1.3+
- Python 3.10+ (for asset pipeline)

## Installation

```bash
git clone --recursive https://github.com/xylotech/engine.git
cd engine
cmake -B build -DXYL_BUILD_EDITOR=ON
cmake --build build --config Release
```

## Your First Project

Create a new project directory and link against the engine:

```cmake
cmake_minimum_required(VERSION 3.25)
project(MyGame)

add_subdirectory(path/to/engine)
add_executable(mygame main.cpp)
target_link_libraries(mygame PRIVATE xylotech::engine)
```

## Creating an Entity

```cpp
#include <engine/ecs/World.h>
#include <engine/ecs/components/Transform.h>
#include <engine/ecs/components/MeshRenderer.h>

using namespace xy;

int main() {
    ecs::World world;

    ecs::Entity player = world.create(
        Transform{.position = {0, 1, 0}, .scale = {1, 1, 1}},
        MeshRenderer{.mesh = "assets/player.glb", .material = "default"}
    );

    world.add<Velocity>(player, {.linear = {0, 0, -5}});

    while (world.running()) {
        world.tick(1.0f / 60.0f);
    }

    return 0;
}
```

## Next Steps

- Read the [ECS Guide](ecs-guide.md) for queries, systems, and archetypes.
- See [API Reference](api-reference.md) for full class listings.
- Build instructions for each platform: [Build Instructions](build-instructions.md).
