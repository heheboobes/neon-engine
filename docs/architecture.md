# Architecture Overview

Xylotech Engine is a modular, data-oriented game engine built in C++20 with a focus on cache-friendly ECS and deterministic networking.

## High-Level Layer Diagram

```
+------------------------------------------------------------------+
|  Game Layer (Scripting, Prefabs, Game States)                     |
+------------------------------------------------------------------+
            |                     |                    |
            v                     v                    v
+------------------------+ +----------+ +---------------------------+
|  Entity Component Sys  | |  Physics | |  Rendering Pipeline       |
|  (Archetype-based ECS) | |  (Jolt)  | |  (GPU-driven, Vulkan)    |
+------------------------+ +----------+ +---------------------------+
            |                     |                    |
            v                     v                    v
+------------------------------------------------------------------+
|  Core Layer (Job System, Memory Allocators, Asset Manager)       |
+------------------------------------------------------------------+
            |                     |                    |
            v                     v                    v
+------------------------------------------------------------------+
|  Platform Layer (Window, Input, File I/O, GPU Backend)           |
+------------------------------------------------------------------+
```

## Module Dependency Graph

```
engine/
  core/          -- no deps (macros, intypes, math)
    core/containers/     -- depends on core/
    core/jobs/           -- depends on core/containers/
  ecs/           -- depends on core/containers/, core/jobs/
  physics/       -- depends on ecs/
  render/        -- depends on ecs/, core/
    render/vulkan/       -- depends on render/
  audio/         -- depends on core/
  network/       -- depends on core/, ecs/
  asset/         -- depends on core/, core/jobs/
  input/         -- depends on platform/
  platform/      -- no engine deps (GLFW, SDL2 abstraction)
```

## Data Flow (Single Frame)

```
Input (platform)
  |
  v
Game State Update (game logic scripts)
  |
  v
ECS System Dispatch (parallel over archetype chunks)
  |--- Physics system (write transforms, colliders)
  |--- Animation system (write bones, skin data)
  |--- Script system (user systems)
  |
  v
Submit Renderables (gather draw calls from ECS)
  |
  v
GPU Frame (vkQueueSubmit, presentation)
```

## Key Design Decisions

- **Archetype-based ECS**: Entities with the same component layout share a contiguous chunk. Queries iterate dense arrays for high cache locality.
- **Job System**: A fiber-based work-stealing scheduler with explicit dependencies. Systems declare read/write sets; the scheduler resolves overlaps and inserts sync points automatically.
- **Deterministic Networking**: Fixed-timestep simulation with rollback. Physics and ECS snapshots are checksummed every tick for replays and anti-cheat.
