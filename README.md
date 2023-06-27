# Neon Engine

A high-performance 2D game engine built on SFML, featuring an archetype-based Entity Component System, modular architecture, and comprehensive tooling for 2D game development.

## Overview

Neon Engine provides a complete foundation for building 2D games in C++. It combines the power of SFML's multimedia capabilities with a modern ECS architecture, enabling efficient entity management, rendering, physics simulation, and more.

### Key Features

- **Archetype-based ECS** — High-performance entity-component system with contiguous memory layout, archetype chunk storage, and compile-time component queries
- **Fixed Timestep Game Loop** — Deterministic physics and consistent behavior across frame rates using the accumulator pattern with interpolation
- **2D Rendering Pipeline** — Sprite batching, layered rendering, camera system with smooth follow and screen shake, particle system with GPU-instanced quads
- **Physics & Collision** — Semi-implicit Euler integration, AABB collision detection with broad-phase spatial optimization, contact resolution with friction and restitution
- **Audio Engine** — Spatial audio, sound pooling, music player with crossfade and playlists, per-channel volume control
- **Input System** — Action-based input mapping with chord support, rebinding, axis bindings, joystick support
- **UI Framework** — Canvas-based widget tree with anchors, layout modes, theming via Style objects, event routing
- **Scene Management** — Scene stack with push/pop/switch, serialization, lifecycle callbacks
- **Scripting Support** — Component-based scripting with update callbacks, property tables
- **Tilemap System** — Multi-layer chunked tilemaps with animated tiles, parallax scrolling, collision flags
- **Debug Tooling** — In-game console, profiler with hierarchical timing, stats overlay, asset inspector
- **Serialization** — JSON and binary formats for scenes, components, and configuration

## Getting Started

### Prerequisites

- CMake 3.20+
- C++20 compatible compiler (MSVC 2022+, GCC 11+, Clang 14+)
- SFML 2.6+

### Building

```bash
git clone https://github.com/neon-engine/neon.git
cd neon
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

### Building with Examples

```bash
cmake .. -DBUILD_EXAMPLES=ON
cmake --build .
```

### Running Tests

```bash
cmake .. -DBUILD_TESTS=ON
cmake --build .
ctest --output-on-failure
```

## Architecture

```
neon/
├── engine/
│   ├── core/          # Application, GameLoop, Config, Time, Layer system
│   ├── ecs/           # Entity, Component, System, World, Archetype, Chunk
│   ├── components/    # Built-in components (Transform, Sprite, Physics, etc.)
│   ├── systems/       # Built-in systems (Render, Physics, Animation, etc.)
│   ├── graphics/      # Renderer, SpriteBatch, TextureManager, Camera, ShaderManager
│   ├── audio/         # AudioEngine, SoundManager, MusicPlayer
│   ├── input/         # InputManager, InputAction, InputBinding
│   ├── math/          # Vector2/3, Matrix3/4, Rect, MathUtils, Random
│   ├── physics/       # PhysicsWorld, CollisionDetection, BroadPhase, ContactResolver
│   ├── particles/     # ParticleSystem, ParticleEmitter, Particle
│   ├── tilemap/       # Tilemap, TileLayer, TileSet
│   ├── ui/            # Canvas, Widget, Button, Label, Panel, TextBox, Slider, etc.
│   ├── serialization/ # Serializer, Deserializer, JsonSerializer, BinarySerializer
│   ├── debug/         # DebugDraw, Profiler, Logger, Console, StatsOverlay, AssetInspector
│   ├── scene/         # Scene, SceneManager
│   └── utils/         # FileUtils, StringUtils, Timer, PoolAllocator, ResourceHandle
├── examples/          # Platformer, RPG, Pong example projects
├── tests/             # Unit tests for ECS, Math, Physics, Serialization
└── assets/            # Runtime assets (textures, sounds, fonts, shaders)
```

## ECS Quick Start

```cpp
#include <engine/ecs/World.h>
#include <engine/components/TransformComponent.h>
#include <engine/components/SpriteComponent.h>
#include <engine/systems/RenderSystem.h>

using namespace engine;

int main() {
    ecs::World world;
    world.RegisterSystem<systems::RenderSystem>();

    ecs::Entity player = world.CreateEntity();
    world.AddComponent<components::TransformComponent>(player, {100.f, 200.f});
    world.AddComponent<components::SpriteComponent>(player, "player_ship.png");

    while (world.IsRunning()) {
        world.Update(1.0f / 60.0f);
    }
    return 0;
}
```

## License

MIT License — see [LICENSE](LICENSE) for details.

## Contributing

Contributions are welcome. Please read our contributing guidelines and code of conduct before submitting pull requests.

## Version History

- **1.0.0** (2023-12-15) — Initial stable release
- **0.9.0** (2023-10-01) — UI framework, serialization, debug tools
- **0.8.0** (2023-07-01) — Physics, particles, tilemaps
- **0.7.0** (2023-04-01) — Audio, input systems, scripting
- **0.6.0** (2023-01-15) — Graphics pipeline, camera, animations
- **0.5.0** (2022-10-01) — ECS architecture, component/systems
- **0.4.0** (2022-07-15) — Math library, core infrastructure
- **0.3.0** (2022-05-01) — Application framework, game loop
- **0.2.0** (2022-03-01) — Project structure, build system
- **0.1.0** (2022-01-15) — Initial project setup

## Version History

### 0.9.0 - UI Framework and Debug Tools
