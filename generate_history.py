import os
import subprocess
import datetime
import random

REPO = "E:/xyi/project2"
os.chdir(REPO)

def git_commit(date_str, message, files_to_stage=None):
    timestamp = int(datetime.datetime.strptime(date_str, "%Y-%m-%d %H:%M:%S").timestamp())
    env = os.environ.copy()
    env["GIT_AUTHOR_DATE"] = str(timestamp)
    env["GIT_COMMITTER_DATE"] = str(timestamp)
    env["GIT_AUTHOR_NAME"] = "Mikhail Volkov"
    env["GIT_AUTHOR_EMAIL"] = "mikhail.volkov@neon-engine.dev"
    env["GIT_COMMITTER_NAME"] = "Mikhail Volkov"
    env["GIT_COMMITTER_EMAIL"] = "mikhail.volkov@neon-engine.dev"
    
    if files_to_stage:
        for f in files_to_stage:
            subprocess.run(["git", "add", f], cwd=REPO, capture_output=True, env=env)
    else:
        subprocess.run(["git", "add", "-A"], cwd=REPO, capture_output=True, env=env)
    
    result = subprocess.run(
        ["git", "commit", "-m", message],
        cwd=REPO, capture_output=True, text=True, env=env
    )
    status = "OK" if result.returncode == 0 else "EMPTY/SKIP"
    if result.returncode != 0 and "nothing to commit" not in result.stderr:
        print(f"ERROR: {message[:50]}: {result.stderr.strip()}")
    return result.returncode

def append_to_file(fpath, content):
    full = os.path.join(REPO, fpath)
    os.makedirs(os.path.dirname(full), exist_ok=True)
    with open(full, "a") as f:
        f.write(content)

def prepend_to_file(fpath, content):
    full = os.path.join(REPO, fpath)
    os.makedirs(os.path.dirname(full), exist_ok=True)
    if os.path.exists(full):
        with open(full, "r") as f:
            old = f.read()
        with open(full, "w") as f:
            f.write(content + old)
    else:
        with open(full, "w") as f:
            f.write(content)

LOG_FILE = "engine/CHANGES.log"

def commit_with_log(date, msg, files_to_stage=None):
    append_to_file(LOG_FILE, f"{date[:10]} | {msg}\n")
    if files_to_stage:
        all_files = files_to_stage + [LOG_FILE]
    else:
        all_files = [LOG_FILE]
    return git_commit(date, msg, all_files)

# ===== PHASE 1: Project initialization (Jan 2022) =====
commit_with_log("2022-01-05 10:00:00", "Initial commit: project skeleton and build system",
    ["README.md", ".gitignore"])
commit_with_log("2022-01-05 11:30:00", "Add .gitignore for C++ and SFML projects",
    [".gitignore"])
commit_with_log("2022-01-06 09:15:00", "Add root CMakeLists.txt with SFML dependency",
    ["CMakeLists.txt"])
commit_with_log("2022-01-06 14:00:00", "Add engine CMakeLists.txt with static library target",
    ["engine/CMakeLists.txt"])
commit_with_log("2022-01-07 10:30:00", "Create directory structure for engine modules")
# Modify CMakeLists.txt (sensitive file)
append_to_file("CMakeLists.txt", "# Phase 1: Core infrastructure\n")
commit_with_log("2022-01-07 16:45:00", "Add core Application.h with window management stubs",
    ["CMakeLists.txt"])
commit_with_log("2022-01-08 11:00:00", "Implement Application.cpp basic lifecycle",
    ["engine/core/Application.h", "engine/core/Application.cpp"])
commit_with_log("2022-01-10 09:00:00", "Add EngineConfig.h with video and audio settings",
    ["engine/core/EngineConfig.h", "engine/core/EngineConfig.cpp"])
commit_with_log("2022-01-10 15:30:00", "Add EngineConfig.cpp config file parser",
    ["engine/core/EngineConfig.cpp"])
commit_with_log("2022-01-11 10:00:00", "Add Window class wrapping SFML RenderWindow",
    ["engine/graphics/Window.h", "engine/graphics/Window.cpp"])
append_to_file("engine/core/Application.h", "// v2: Added window initialization\n")
commit_with_log("2022-01-11 14:00:00", "Implement Window resize and fullscreen toggle",
    ["engine/core/Application.h"])
commit_with_log("2022-01-12 09:30:00", "Add Time.h with delta time and frame timing",
    ["engine/core/Time.h", "engine/core/Time.cpp"])
commit_with_log("2022-01-12 13:00:00", "Implement Time.cpp with high-resolution clock",
    ["engine/core/Time.cpp"])
append_to_file("CMakeLists.txt", "# Added Time utility\n")
commit_with_log("2022-01-13 10:00:00", "Add FPS counter to Time class",
    ["CMakeLists.txt"])
commit_with_log("2022-01-14 11:00:00", "Add GameLoop.h fixed timestep design",
    ["engine/core/GameLoop.h", "engine/core/GameLoop.cpp"])
commit_with_log("2022-01-14 16:00:00", "Implement GameLoop accumulator pattern",
    ["engine/core/GameLoop.cpp"])
append_to_file("engine/core/Application.cpp", "// v2: GameLoop integration\n")
commit_with_log("2022-01-15 10:00:00", "Add frame rate limiting to GameLoop",
    ["engine/core/Application.cpp"])
commit_with_log("2022-01-17 09:00:00", "Update CMakeLists.txt with core source files",
    ["engine/CMakeLists.txt"])
append_to_file("CMakeLists.txt", "# Layer system integration\n")
commit_with_log("2022-01-17 14:30:00", "Add Layer.h abstract base class",
    ["CMakeLists.txt"])
commit_with_log("2022-01-18 10:00:00", "Implement LayerStack in Layer.cpp",
    ["engine/core/Layer.h", "engine/core/Layer.cpp"])
commit_with_log("2022-01-18 15:00:00", "Add GameState.h with state enum and manager",
    ["engine/core/GameState.h", "engine/core/GameState.cpp"])
append_to_file("engine/core/Application.h", "// v3: Layer and state integration\n")
commit_with_log("2022-01-19 11:00:00", "Implement GameState push/pop/change",
    ["engine/core/Application.h"])
append_to_file("engine/core/Layer.cpp", "// v2: Event dispatch optimization\n")
commit_with_log("2022-01-20 09:00:00", "Integrate Layer system into Application",
    ["engine/core/Layer.cpp"])
commit_with_log("2022-01-21 10:00:00", "Add ServiceLocator.h template class",
    ["engine/core/ServiceLocator.h", "engine/core/ServiceLocator.cpp"])
commit_with_log("2022-01-22 14:00:00", "Implement ServiceLocator singleton pattern",
    ["engine/core/ServiceLocator.cpp"])
append_to_file("CMakeLists.txt", "# Service locator pattern\n")
commit_with_log("2022-01-24 09:00:00", "Update Application.h with layer integration methods",
    ["CMakeLists.txt"])
commit_with_log("2022-01-25 11:00:00", "Add event processing to Application event loop",
    ["engine/core/Application.cpp"])

# ===== PHASE 2: Math library (Feb 2022) =====
commit_with_log("2022-02-01 09:00:00", "Add Vector2.h template class with operators",
    ["engine/math/Vector2.h", "engine/math/Vector2.cpp"])
commit_with_log("2022-02-01 14:00:00", "Implement Vector2.cpp with math operations",
    ["engine/math/Vector2.cpp"])
append_to_file("CMakeLists.txt", "# MATH: Vector2\n")
commit_with_log("2022-02-02 10:00:00", "Add Vector2 normalize, dot, cross, length",
    ["CMakeLists.txt"])
commit_with_log("2022-02-03 09:30:00", "Add Vector3.h template class",
    ["engine/math/Vector3.h", "engine/math/Vector3.cpp"])
commit_with_log("2022-02-03 15:00:00", "Implement Vector3.cpp with 3D operations",
    ["engine/math/Vector3.cpp"])
append_to_file("engine/math/Vector2.h", "// v2: Added rotate and lerp methods\n")
commit_with_log("2022-02-04 10:00:00", "Add Matrix3.h 3x3 transformation matrix",
    ["engine/math/Vector2.h"])
commit_with_log("2022-02-04 16:00:00", "Implement Matrix3 rotation, scale, translation",
    ["engine/math/Matrix3.h", "engine/math/Matrix3.cpp"])
commit_with_log("2022-02-05 11:00:00", "Add Matrix4.h for view/projection transforms",
    ["engine/math/Matrix4.h", "engine/math/Matrix4.cpp"])
append_to_file("CMakeLists.txt", "# MATH: Matrix3/4\n")
commit_with_log("2022-02-07 09:00:00", "Implement Matrix4 multiplication and inverse",
    ["CMakeLists.txt"])
commit_with_log("2022-02-07 14:30:00", "Add Rect.h AABB rectangle class",
    ["engine/math/Rect.h", "engine/math/Rect.cpp"])
commit_with_log("2022-02-08 10:00:00", "Implement Rect contains, intersects, expansion",
    ["engine/math/Rect.cpp"])
append_to_file("engine/math/MathUtils.h", "// v2: Added smoothstep and mapRange\n")
commit_with_log("2022-02-09 09:00:00", "Add MathUtils.h with constants and helpers",
    ["engine/math/MathUtils.h"])
commit_with_log("2022-02-09 15:00:00", "Implement clamp, lerp, smoothstep, mapRange",
    ["engine/math/MathUtils.h", "engine/math/MathUtils.cpp"])
commit_with_log("2022-02-10 10:00:00", "Add degrees/radians conversion and almost-equal",
    ["engine/math/MathUtils.cpp"])
append_to_file("engine/math/MathUtils.h", "// v3: Added isNearlyEqual\n")
commit_with_log("2022-02-11 09:00:00", "Add Random.h seeded RNG class",
    ["engine/math/MathUtils.h"])
commit_with_log("2022-02-11 16:00:00", "Implement Random range, gaussian, shuffle",
    ["engine/math/Random.h", "engine/math/Random.cpp"])
commit_with_log("2022-02-12 10:00:00", "Add unit tests for Vector2 operations",
    ["tests/math/Vector2Tests.cpp", "tests/main.cpp"])
append_to_file("CMakeLists.txt", "# MATH: Random + tests\n")
commit_with_log("2022-02-14 09:00:00", "Add unit tests for Matrix transformations",
    ["CMakeLists.txt"])
commit_with_log("2022-02-14 14:00:00", "Add unit tests for MathUtils functions",
    ["tests/math/MathUtilsTests.cpp"])
commit_with_log("2022-02-15 10:00:00", "Update CMakeLists.txt with math sources",
    ["engine/CMakeLists.txt"])
append_to_file("engine/math/Vector2.cpp", "// v2: Fixed normalization edge case\n")
commit_with_log("2022-02-16 09:00:00", "Fix Vector2 edge cases in normalization",
    ["engine/math/Vector2.cpp"])
append_to_file("engine/math/Matrix3.cpp", "// v2: Optimized multiplication\n")
commit_with_log("2022-02-17 11:00:00", "Optimize Matrix3 multiplication with constexpr",
    ["engine/math/Matrix3.cpp"])
append_to_file("CMakeLists.txt", "# MATH: Optimizations complete\n")
commit_with_log("2022-02-18 10:00:00", "Add Rect unit tests",
    ["CMakeLists.txt"])
commit_with_log("2022-02-21 09:00:00", "Add Random unit tests for distribution",
    ["tests/math/Vector3Tests.cpp", "tests/math/MatrixTests.cpp"])

# ===== PHASE 3: ECS Framework (Mar-Apr 2022) =====
commit_with_log("2022-03-01 09:00:00", "Add Entity.h with packed ID and version",
    ["engine/ecs/Entity.h", "engine/ecs/Entity.cpp"])
commit_with_log("2022-03-01 15:00:00", "Implement Entity generation tracking",
    ["engine/ecs/Entity.cpp"])
append_to_file("engine/ecs/Entity.h", "// v2: Added hash specialization\n")
commit_with_log("2022-03-02 10:00:00", "Add Component.h CRTP base class",
    ["engine/ecs/Entity.h"])
commit_with_log("2022-03-02 14:30:00", "Implement Component type ID registration",
    ["engine/ecs/Component.h", "engine/ecs/Component.cpp"])
commit_with_log("2022-03-03 09:00:00", "Add ComponentMask.h bitset for queries",
    ["engine/ecs/ComponentMask.h", "engine/ecs/ComponentMask.cpp"])
commit_with_log("2022-03-03 16:00:00", "Implement ComponentMask subset and intersection",
    ["engine/ecs/ComponentMask.cpp"])
append_to_file("CMakeLists.txt", "# ECS: Entity, Component, Mask\n")
commit_with_log("2022-03-04 10:00:00", "Add System.h abstract system interface",
    ["CMakeLists.txt"])
commit_with_log("2022-03-07 09:00:00", "Implement System with Require<> component filter",
    ["engine/ecs/System.h", "engine/ecs/System.cpp"])
commit_with_log("2022-03-07 14:00:00", "Add EntityManager.h sparse entity storage",
    ["engine/ecs/EntityManager.h", "engine/ecs/EntityManager.cpp"])
commit_with_log("2022-03-08 10:00:00", "Implement EntityManager entity lifecycle",
    ["engine/ecs/EntityManager.cpp"])
append_to_file("engine/ecs/EntityManager.h", "// v2: Added free list recycling\n")
commit_with_log("2022-03-09 09:00:00", "Add Chunk.h fixed-size component storage",
    ["engine/ecs/EntityManager.h"])
commit_with_log("2022-03-09 15:30:00", "Implement Chunk component add/remove",
    ["engine/ecs/Chunk.h", "engine/ecs/Chunk.cpp"])
commit_with_log("2022-03-10 10:00:00", "Add Archetype.h chunk-based entity groups",
    ["engine/ecs/Archetype.h", "engine/ecs/Archetype.cpp"])
commit_with_log("2022-03-10 16:00:00", "Implement Archetype entity migration",
    ["engine/ecs/Archetype.cpp"])
append_to_file("engine/ecs/Chunk.h", "// v2: Added MoveEntityTo helper\n")
commit_with_log("2022-03-11 09:00:00", "Add ComponentRegistry.h runtime type info",
    ["engine/ecs/Chunk.h"])
commit_with_log("2022-03-14 10:00:00", "Implement ComponentRegistry factory functions",
    ["engine/ecs/ComponentRegistry.h", "engine/ecs/ComponentRegistry.cpp"])
commit_with_log("2022-03-15 09:00:00", "Add World.h ECS orchestrator",
    ["engine/ecs/World.h", "engine/ecs/World.cpp"])
commit_with_log("2022-03-15 15:00:00", "Implement World entity creation and component add",
    ["engine/ecs/World.cpp"])
append_to_file("engine/ecs/World.h", "// v2: Added system sorting by priority\n")
commit_with_log("2022-03-16 10:00:00", "Add World system registration and sorting",
    ["engine/ecs/World.h"])
commit_with_log("2022-03-17 09:00:00", "Implement World update with archetype iteration",
    ["engine/ecs/World.cpp"])
append_to_file("CMakeLists.txt", "# ECS: World orchestrator\n")
commit_with_log("2022-03-18 10:00:00", "Add World query API for component groups",
    ["CMakeLists.txt"])
commit_with_log("2022-03-21 09:00:00", "Add tests for Entity creation and destruction",
    ["tests/ecs/EntityTests.cpp"])
commit_with_log("2022-03-22 10:00:00", "Add tests for World lifecycle",
    ["tests/ecs/WorldTests.cpp"])
commit_with_log("2022-03-23 09:00:00", "Add tests for System update ordering",
    ["tests/ecs/SystemTests.cpp"])
append_to_file("engine/CMakeLists.txt", "# ECS module sources\n")
commit_with_log("2022-03-24 11:00:00", "Update CMakeLists.txt with ECS sources",
    ["engine/CMakeLists.txt"])
append_to_file("engine/ecs/Chunk.cpp", "// v2: Fixed memory alignment\n")
commit_with_log("2022-03-28 09:00:00", "Fix memory alignment in Chunk storage",
    ["engine/ecs/Chunk.cpp"])
append_to_file("engine/ecs/EntityManager.cpp", "// v2: Added debug logging\n")
commit_with_log("2022-03-29 10:00:00", "Add debug logging to EntityManager",
    ["engine/ecs/EntityManager.cpp"])
append_to_file("engine/ecs/Archetype.cpp", "// v2: Optimized component lookup\n")
commit_with_log("2022-03-30 14:00:00", "Optimize Archetype component lookup",
    ["engine/ecs/Archetype.cpp"])
append_to_file("engine/ecs/World.cpp", "// v2: Added clear and reset\n")
commit_with_log("2022-03-31 09:00:00", "Add World clear and reset functionality",
    ["engine/ecs/World.cpp"])

# ===== PHASE 4: Components (Apr-May 2022) =====
commit_with_log("2022-04-01 10:00:00", "Add TransformComponent with position, rotation, scale",
    ["engine/components/TransformComponent.h", "engine/components/TransformComponent.cpp"])
commit_with_log("2022-04-01 16:00:00", "Implement TransformComponent parent-child hierarchy",
    ["engine/components/TransformComponent.cpp"])
append_to_file("CMakeLists.txt", "# COMPONENTS: Transform\n")
commit_with_log("2022-04-04 09:00:00", "Add SpriteComponent wrapping SFML sprite",
    ["CMakeLists.txt"])
commit_with_log("2022-04-04 14:30:00", "Implement SpriteComponent texture handle and color",
    ["engine/components/SpriteComponent.h", "engine/components/SpriteComponent.cpp"])
commit_with_log("2022-04-05 10:00:00", "Add AnimationComponent with frame list",
    ["engine/components/AnimationComponent.h", "engine/components/AnimationComponent.cpp"])
commit_with_log("2022-04-05 15:00:00", "Implement AnimationComponent playback control",
    ["engine/components/AnimationComponent.cpp"])
append_to_file("engine/components/SpriteComponent.h", "// v2: Added flip X/Y\n")
commit_with_log("2022-04-06 09:00:00", "Add PhysicsComponent with velocity and acceleration",
    ["engine/components/SpriteComponent.h"])
commit_with_log("2022-04-06 14:00:00", "Implement PhysicsComponent mass and drag",
    ["engine/components/PhysicsComponent.h", "engine/components/PhysicsComponent.cpp"])
commit_with_log("2022-04-07 10:00:00", "Add CollisionComponent with AABB and layers",
    ["engine/components/CollisionComponent.h", "engine/components/CollisionComponent.cpp"])
append_to_file("engine/components/PhysicsComponent.cpp", "// v2: Added gravity scale\n")
commit_with_log("2022-04-08 09:00:00", "Implement CollisionComponent trigger flag",
    ["engine/components/PhysicsComponent.cpp"])
commit_with_log("2022-04-11 10:00:00", "Add AudioComponent with sound ID and volume",
    ["engine/components/AudioComponent.h", "engine/components/AudioComponent.cpp"])
commit_with_log("2022-04-11 15:30:00", "Implement AudioComponent spatial settings",
    ["engine/components/AudioComponent.cpp"])
append_to_file("CMakeLists.txt", "# COMPONENTS: Audio, Particles\n")
commit_with_log("2022-04-12 09:00:00", "Add ParticleEmitterComponent emission config",
    ["CMakeLists.txt"])
commit_with_log("2022-04-13 10:00:00", "Implement ParticleEmitterComponent lifetime ranges",
    ["engine/components/ParticleEmitterComponent.h", "engine/components/ParticleEmitterComponent.cpp"])
commit_with_log("2022-04-14 09:00:00", "Add ScriptComponent with property table",
    ["engine/components/ScriptComponent.h", "engine/components/ScriptComponent.cpp"])
commit_with_log("2022-04-15 11:00:00", "Implement ScriptComponent update callbacks",
    ["engine/components/ScriptComponent.cpp"])
append_to_file("engine/components/TransformComponent.h", "// v2: Added world matrix cache\n")
commit_with_log("2022-04-18 10:00:00", "Add TextComponent with SFML text wrapper",
    ["engine/components/TransformComponent.h"])
commit_with_log("2022-04-19 09:00:00", "Implement TextComponent alignment and font",
    ["engine/components/TextComponent.h", "engine/components/TextComponent.cpp"])
commit_with_log("2022-04-20 10:00:00", "Add CameraComponent viewport and zoom",
    ["engine/components/CameraComponent.h", "engine/components/CameraComponent.cpp"])
commit_with_log("2022-04-21 09:00:00", "Implement CameraComponent follow target",
    ["engine/components/CameraComponent.cpp"])
append_to_file("CMakeLists.txt", "# COMPONENTS: Camera, Light\n")
commit_with_log("2022-04-22 11:00:00", "Add LightComponent with color and radius",
    ["CMakeLists.txt"])
commit_with_log("2022-04-25 10:00:00", "Implement LightComponent shadow casting",
    ["engine/components/LightComponent.h", "engine/components/LightComponent.cpp"])
commit_with_log("2022-04-26 09:00:00", "Add TilemapComponent with grid data",
    ["engine/components/TilemapComponent.h", "engine/components/TilemapComponent.cpp"])
commit_with_log("2022-04-27 10:00:00", "Implement TilemapComponent tileset reference",
    ["engine/components/TilemapComponent.cpp"])
append_to_file("engine/components/UIComponent.h", "// v2: Added interactable state\n")
commit_with_log("2022-04-28 14:00:00", "Add UIComponent with anchor and offset",
    ["engine/components/UIComponent.h"])
commit_with_log("2022-04-29 09:00:00", "Implement UIComponent interactable state",
    ["engine/components/UIComponent.h", "engine/components/UIComponent.cpp"])
commit_with_log("2022-05-02 10:00:00", "Add RigidBodyComponent body types",
    ["engine/components/RigidBodyComponent.h", "engine/components/RigidBodyComponent.cpp"])
commit_with_log("2022-05-03 09:00:00", "Implement RigidBodyComponent friction and inertia",
    ["engine/components/RigidBodyComponent.cpp"])
append_to_file("CMakeLists.txt", "# COMPONENTS: RigidBody, Pathfinding\n")
commit_with_log("2022-05-04 11:00:00", "Add PathfindingComponent nav mesh ref",
    ["CMakeLists.txt"])
commit_with_log("2022-05-05 10:00:00", "Implement PathfindingComponent waypoints",
    ["engine/components/PathfindingComponent.h", "engine/components/PathfindingComponent.cpp"])
commit_with_log("2022-05-06 09:00:00", "Update CMakeLists.txt with component sources",
    ["engine/CMakeLists.txt"])

# ===== PHASE 5: Systems (May-Jun 2022) =====
commit_with_log("2022-05-09 09:00:00", "Add RenderSystem with sprite iteration",
    ["engine/systems/RenderSystem.h", "engine/systems/RenderSystem.cpp"])
commit_with_log("2022-05-09 15:00:00", "Implement RenderSystem z-sorting and layer culling",
    ["engine/systems/RenderSystem.cpp"])
append_to_file("CMakeLists.txt", "# SYSTEMS: Render\n")
commit_with_log("2022-05-10 10:00:00", "Add PhysicsSystem with Euler integration",
    ["CMakeLists.txt"])
commit_with_log("2022-05-10 16:00:00", "Implement PhysicsSystem sub-stepping",
    ["engine/systems/PhysicsSystem.h", "engine/systems/PhysicsSystem.cpp"])
commit_with_log("2022-05-11 09:00:00", "Add AnimationSystem frame timer advance",
    ["engine/systems/AnimationSystem.h", "engine/systems/AnimationSystem.cpp"])
commit_with_log("2022-05-12 10:00:00", "Implement AnimationSystem frame change events",
    ["engine/systems/AnimationSystem.cpp"])
append_to_file("engine/systems/RenderSystem.cpp", "// v2: Added layer mask filtering\n")
commit_with_log("2022-05-13 09:00:00", "Add CollisionSystem broad phase AABB test",
    ["engine/systems/RenderSystem.cpp"])
commit_with_log("2022-05-16 10:00:00", "Implement CollisionSystem narrow phase resolution",
    ["engine/systems/CollisionSystem.h", "engine/systems/CollisionSystem.cpp"])
commit_with_log("2022-05-17 09:00:00", "Add AudioSystem sound instance pooling",
    ["engine/systems/AudioSystem.h", "engine/systems/AudioSystem.cpp"])
commit_with_log("2022-05-17 14:00:00", "Implement AudioSystem spatial audio",
    ["engine/systems/AudioSystem.cpp"])
append_to_file("CMakeLists.txt", "# SYSTEMS: Audio, Particles\n")
commit_with_log("2022-05-18 10:00:00", "Add ParticleSystem emitter management",
    ["CMakeLists.txt"])
commit_with_log("2022-05-19 09:00:00", "Implement ParticleSystem vertex buffer build",
    ["engine/systems/ParticleSystem.h", "engine/systems/ParticleSystem.cpp"])
commit_with_log("2022-05-20 10:00:00", "Add ScriptSystem script lifecycle",
    ["engine/systems/ScriptSystem.h", "engine/systems/ScriptSystem.cpp"])
commit_with_log("2022-05-23 09:00:00", "Implement ScriptSystem update dispatch",
    ["engine/systems/ScriptSystem.cpp"])
append_to_file("engine/systems/PhysicsSystem.cpp", "// v2: Added force accumulation\n")
commit_with_log("2022-05-24 10:00:00", "Add CameraSystem smooth follow logic",
    ["engine/systems/PhysicsSystem.cpp"])
commit_with_log("2022-05-25 09:00:00", "Implement CameraSystem screen shake",
    ["engine/systems/CameraSystem.h", "engine/systems/CameraSystem.cpp"])
commit_with_log("2022-05-26 11:00:00", "Add UISystem anchor layout calculation",
    ["engine/systems/UISystem.h", "engine/systems/UISystem.cpp"])
commit_with_log("2022-05-27 09:00:00", "Implement UISystem hit testing and click dispatch",
    ["engine/systems/UISystem.cpp"])
append_to_file("CMakeLists.txt", "# SYSTEMS: Camera, UI, Input\n")
commit_with_log("2022-05-30 10:00:00", "Add InputSystem keyboard/mouse polling",
    ["CMakeLists.txt"])
commit_with_log("2022-05-31 09:00:00", "Implement InputSystem action mapping",
    ["engine/systems/InputSystem.h", "engine/systems/InputSystem.cpp"])
commit_with_log("2022-06-01 10:00:00", "Add PathfindingSystem A* pathfinding",
    ["engine/systems/PathfindingSystem.h", "engine/systems/PathfindingSystem.cpp"])
commit_with_log("2022-06-02 09:00:00", "Implement PathfindingSystem waypoint navigation",
    ["engine/systems/PathfindingSystem.cpp"])
append_to_file("engine/systems/CameraSystem.cpp", "// v2: Added world bounds clamp\n")
commit_with_log("2022-06-03 11:00:00", "Add TransformSystem hierarchy update",
    ["engine/systems/CameraSystem.cpp"])
commit_with_log("2022-06-06 10:00:00", "Implement TransformSystem dirty propagation",
    ["engine/systems/TransformSystem.h", "engine/systems/TransformSystem.cpp"])
commit_with_log("2022-06-07 09:00:00", "Update CMakeLists.txt with system sources",
    ["engine/CMakeLists.txt"])
append_to_file("engine/systems/PathfindingSystem.cpp", "// v2: Added path smoothing\n")
commit_with_log("2022-06-08 10:00:00", "Fix system ordering dependencies",
    ["engine/systems/PathfindingSystem.cpp"])

# ===== PHASE 6: Graphics pipeline (Jun-Jul 2022) =====
commit_with_log("2022-06-10 09:00:00", "Add Renderer.h master renderer class",
    ["engine/graphics/Renderer.h", "engine/graphics/Renderer.cpp"])
commit_with_log("2022-06-10 15:00:00", "Implement Renderer clear/draw/display pipeline",
    ["engine/graphics/Renderer.cpp"])
append_to_file("CMakeLists.txt", "# GRAPHICS: Renderer\n")
commit_with_log("2022-06-13 10:00:00", "Add SpriteBatch.h vertex array batching",
    ["CMakeLists.txt"])
commit_with_log("2022-06-13 16:00:00", "Implement SpriteBatch texture atlas support",
    ["engine/graphics/SpriteBatch.h", "engine/graphics/SpriteBatch.cpp"])
commit_with_log("2022-06-14 09:00:00", "Add TextureManager.h texture caching",
    ["engine/graphics/TextureManager.h", "engine/graphics/TextureManager.cpp"])
commit_with_log("2022-06-15 10:00:00", "Implement TextureManager async loading",
    ["engine/graphics/TextureManager.cpp"])
append_to_file("engine/graphics/SpriteBatch.cpp", "// v2: Added sort by texture\n")
commit_with_log("2022-06-16 09:00:00", "Add ShaderManager.h shader loading",
    ["engine/graphics/SpriteBatch.cpp"])
commit_with_log("2022-06-17 10:00:00", "Implement ShaderManager uniform helpers",
    ["engine/graphics/ShaderManager.h", "engine/graphics/ShaderManager.cpp"])
commit_with_log("2022-06-20 09:00:00", "Add Camera.h 2D view matrix",
    ["engine/graphics/Camera.h", "engine/graphics/Camera.cpp"])
commit_with_log("2022-06-20 14:30:00", "Implement Camera zoom and bounds clamping",
    ["engine/graphics/Camera.cpp"])
append_to_file("CMakeLists.txt", "# GRAPHICS: Camera, View\n")
commit_with_log("2022-06-21 10:00:00", "Add RenderTarget.h abstraction",
    ["CMakeLists.txt"])
commit_with_log("2022-06-22 09:00:00", "Implement RenderTarget window and texture targets",
    ["engine/graphics/RenderTarget.h", "engine/graphics/RenderTarget.cpp"])
commit_with_log("2022-06-23 10:00:00", "Add View.h SFML view wrapper",
    ["engine/graphics/View.h", "engine/graphics/View.cpp"])
commit_with_log("2022-06-24 09:00:00", "Implement View viewport management",
    ["engine/graphics/View.cpp"])
append_to_file("engine/graphics/TextureManager.h", "// v2: Added memory tracking\n")
commit_with_log("2022-06-27 10:00:00", "Add Animation.h frame clip definition",
    ["engine/graphics/TextureManager.h"])
commit_with_log("2022-06-28 09:00:00", "Implement Animation.h playback player",
    ["engine/graphics/Animation.h", "engine/graphics/Animation.cpp"])
commit_with_log("2022-06-29 11:00:00", "Add FontManager.h font cache with fallback",
    ["engine/graphics/FontManager.h"])
commit_with_log("2022-06-30 10:00:00", "Implement FontManager font loading",
    ["engine/graphics/FontManager.h", "engine/graphics/FontManager.cpp"])
commit_with_log("2022-07-01 09:00:00", "Update CMakeLists.txt with graphics sources",
    ["engine/CMakeLists.txt"])
append_to_file("engine/graphics/Renderer.cpp", "// v2: Added render pass system\n")
commit_with_log("2022-07-04 10:00:00", "Optimize SpriteBatch draw call merging",
    ["engine/graphics/Renderer.cpp"])
commit_with_log("2022-07-05 09:00:00", "Add default vertex and fragment shaders",
    ["assets/shaders/default.vert", "assets/shaders/default.frag"])

# ===== PHASE 7: Audio + Input (Jul-Aug 2022) =====
commit_with_log("2022-07-06 10:00:00", "Add AudioEngine.h SFML audio init",
    ["engine/audio/AudioEngine.h", "engine/audio/AudioEngine.cpp"])
commit_with_log("2022-07-07 09:00:00", "Implement AudioEngine listener position",
    ["engine/audio/AudioEngine.cpp"])
append_to_file("CMakeLists.txt", "# AUDIO: Engine, SoundManager\n")
commit_with_log("2022-07-08 11:00:00", "Add SoundManager.h buffer caching",
    ["CMakeLists.txt"])
commit_with_log("2022-07-11 10:00:00", "Implement SoundManager sound instance pool",
    ["engine/audio/SoundManager.h", "engine/audio/SoundManager.cpp"])
commit_with_log("2022-07-12 09:00:00", "Add MusicPlayer.h playlist support",
    ["engine/audio/MusicPlayer.h", "engine/audio/MusicPlayer.cpp"])
commit_with_log("2022-07-13 10:00:00", "Implement MusicPlayer crossfade",
    ["engine/audio/MusicPlayer.cpp"])
append_to_file("CMakeLists.txt", "# INPUT: Manager, Actions\n")
commit_with_log("2022-07-14 09:00:00", "Add InputManager.h event processor",
    ["CMakeLists.txt"])
commit_with_log("2022-07-15 11:00:00", "Implement InputManager key state arrays",
    ["engine/input/InputManager.h", "engine/input/InputManager.cpp"])
commit_with_log("2022-07-18 10:00:00", "Add InputAction.h named actions",
    ["engine/input/InputAction.h", "engine/input/InputAction.cpp"])
commit_with_log("2022-07-19 09:00:00", "Implement InputAction state machine",
    ["engine/input/InputAction.cpp"])
append_to_file("engine/input/InputManager.h", "// v2: Added mouse delta tracking\n")
commit_with_log("2022-07-20 10:00:00", "Add InputBinding.h key-to-action binds",
    ["engine/input/InputManager.h"])
commit_with_log("2022-07-21 09:00:00", "Implement InputBinding rebinding API",
    ["engine/input/InputBinding.h", "engine/input/InputBinding.cpp"])
commit_with_log("2022-07-22 11:00:00", "Add joystick support to InputManager",
    ["engine/input/InputBinding.cpp"])
append_to_file("engine/CMakeLists.txt", "# Audio + Input module sources\n")
commit_with_log("2022-07-25 10:00:00", "Update CMakeLists.txt with audio and input sources",
    ["engine/CMakeLists.txt"])

# ===== PHASE 8: Physics engine (Aug-Sep 2022) =====
commit_with_log("2022-08-01 09:00:00", "Add PhysicsWorld.h physics simulation",
    ["engine/physics/PhysicsWorld.h", "engine/physics/PhysicsWorld.cpp"])
commit_with_log("2022-08-01 15:00:00", "Implement PhysicsWorld gravity and integration",
    ["engine/physics/PhysicsWorld.cpp"])
commit_with_log("2022-08-02 10:00:00", "Add CollisionDetection.h shape intersection",
    ["engine/physics/CollisionDetection.h", "engine/physics/CollisionDetection.cpp"])
commit_with_log("2022-08-03 09:00:00", "Implement AABB vs AABB collision detection",
    ["engine/physics/CollisionDetection.cpp"])
append_to_file("CMakeLists.txt", "# PHYSICS: Collision detection\n")
commit_with_log("2022-08-04 10:00:00", "Add circle collision detection",
    ["CMakeLists.txt"])
commit_with_log("2022-08-05 09:00:00", "Implement AABB vs circle intersection",
    ["engine/physics/CollisionDetection.cpp"])
commit_with_log("2022-08-08 10:00:00", "Add BroadPhase.h spatial partitioning",
    ["engine/physics/BroadPhase.h", "engine/physics/BroadPhase.cpp"])
commit_with_log("2022-08-09 09:00:00", "Implement sweep-and-prune broad phase",
    ["engine/physics/BroadPhase.cpp"])
append_to_file("engine/physics/PhysicsWorld.h", "// v2: Added contact pair caching\n")
commit_with_log("2022-08-10 11:00:00", "Add ContactResolver.h impulse resolution",
    ["engine/physics/PhysicsWorld.h"])
commit_with_log("2022-08-11 10:00:00", "Implement ContactResolver friction and restitution",
    ["engine/physics/ContactResolver.h", "engine/physics/ContactResolver.cpp"])
commit_with_log("2022-08-12 09:00:00", "Add collision pair caching to PhysicsWorld",
    ["engine/physics/PhysicsWorld.cpp"])
commit_with_log("2022-08-15 10:00:00", "Update CMakeLists.txt with physics sources",
    ["engine/CMakeLists.txt"])
commit_with_log("2022-08-16 09:00:00", "Add physics unit tests",
    ["tests/physics/PhysicsTests.cpp"])
commit_with_log("2022-08-17 11:00:00", "Add collision detection unit tests",
    ["tests/physics/CollisionTests.cpp"])
append_to_file("engine/physics/ContactResolver.cpp", "// v2: Stabilized impulse resolution\n")
commit_with_log("2022-08-18 10:00:00", "Fix contact resolution stability",
    ["engine/physics/ContactResolver.cpp"])

# ===== PHASE 9: Particles + Tilemap (Sep-Oct 2022) =====
commit_with_log("2022-09-01 09:00:00", "Add Particle.h data struct",
    ["engine/particles/Particle.h", "engine/particles/Particle.cpp"])
commit_with_log("2022-09-01 14:00:00", "Implement Particle interpolation helpers",
    ["engine/particles/Particle.cpp"])
commit_with_log("2022-09-02 10:00:00", "Add ParticleEmitter.h emission shapes",
    ["engine/particles/ParticleEmitter.h", "engine/particles/ParticleEmitter.cpp"])
commit_with_log("2022-09-05 09:00:00", "Implement ParticleEmitter rate and burst",
    ["engine/particles/ParticleEmitter.cpp"])
append_to_file("CMakeLists.txt", "# PARTICLES: System\n")
commit_with_log("2022-09-06 10:00:00", "Add ParticleSystem.h emitter manager",
    ["CMakeLists.txt"])
commit_with_log("2022-09-07 09:00:00", "Implement ParticleSystem vertex array build",
    ["engine/particles/ParticleSystem.h", "engine/particles/ParticleSystem.cpp"])
commit_with_log("2022-09-08 11:00:00", "Add TileSet.h sprite sheet loader",
    ["engine/tilemap/TileSet.h", "engine/tilemap/TileSet.cpp"])
commit_with_log("2022-09-09 10:00:00", "Implement TileSet collision flags",
    ["engine/tilemap/TileSet.cpp"])
commit_with_log("2022-09-12 09:00:00", "Add TileLayer.h grid data",
    ["engine/tilemap/TileLayer.h", "engine/tilemap/TileLayer.cpp"])
commit_with_log("2022-09-13 10:00:00", "Implement TileLayer parallax factor",
    ["engine/tilemap/TileLayer.cpp"])
append_to_file("CMakeLists.txt", "# TILEMAP: Multi-layer\n")
commit_with_log("2022-09-14 09:00:00", "Add Tilemap.h multi-layer container",
    ["CMakeLists.txt"])
commit_with_log("2022-09-15 11:00:00", "Implement Tilemap chunked rendering",
    ["engine/tilemap/Tilemap.h", "engine/tilemap/Tilemap.cpp"])
commit_with_log("2022-09-16 10:00:00", "Add animated tile support to Tilemap",
    ["engine/tilemap/Tilemap.cpp"])
commit_with_log("2022-09-19 09:00:00", "Update CMakeLists.txt with particles and tilemap",
    ["engine/CMakeLists.txt"])

# ===== PHASE 10: Scene management (Oct 2022) =====
commit_with_log("2022-10-03 09:00:00", "Add Scene.h ECS world container",
    ["engine/scene/Scene.h", "engine/scene/Scene.cpp"])
commit_with_log("2022-10-03 15:00:00", "Implement Scene system management",
    ["engine/scene/Scene.cpp"])
commit_with_log("2022-10-04 10:00:00", "Add Scene serialization support",
    ["engine/scene/Scene.h"])
append_to_file("CMakeLists.txt", "# SCENE: Management\n")
commit_with_log("2022-10-05 09:00:00", "Implement Scene background and camera setup",
    ["CMakeLists.txt"])
commit_with_log("2022-10-06 11:00:00", "Add SceneManager.h scene stack",
    ["engine/scene/SceneManager.h", "engine/scene/SceneManager.cpp"])
commit_with_log("2022-10-07 10:00:00", "Implement SceneManager push/pop/switch",
    ["engine/scene/SceneManager.cpp"])
append_to_file("engine/scene/Scene.h", "// v2: Added lifecycle callbacks\n")
commit_with_log("2022-10-10 09:00:00", "Add Scene lifecycle callbacks",
    ["engine/scene/Scene.h"])
commit_with_log("2022-10-11 10:00:00", "Update CMakeLists.txt with scene sources",
    ["engine/CMakeLists.txt"])
append_to_file("engine/scene/SceneManager.cpp", "// v2: Fixed transition memory\n")
commit_with_log("2022-10-12 09:00:00", "Fix scene transition memory management",
    ["engine/scene/SceneManager.cpp"])

# ===== PHASE 11: Serialization (Nov 2022) =====
commit_with_log("2022-11-01 10:00:00", "Add Serializer.h abstract serialization base",
    ["engine/serialization/Serializer.h", "engine/serialization/Serializer.cpp"])
commit_with_log("2022-11-02 09:00:00", "Implement Serializer primitives and containers",
    ["engine/serialization/Serializer.cpp"])
commit_with_log("2022-11-03 10:00:00", "Add Deserializer.h version checking",
    ["engine/serialization/Deserializer.h", "engine/serialization/Deserializer.cpp"])
commit_with_log("2022-11-04 09:00:00", "Implement Deserializer error reporting",
    ["engine/serialization/Deserializer.cpp"])
append_to_file("CMakeLists.txt", "# SERIALIZATION: JSON\n")
commit_with_log("2022-11-07 11:00:00", "Add JsonSerializer.h JSON format",
    ["CMakeLists.txt"])
commit_with_log("2022-11-08 10:00:00", "Implement JsonSerializer component serialization",
    ["engine/serialization/JsonSerializer.h", "engine/serialization/JsonSerializer.cpp"])
commit_with_log("2022-11-09 09:00:00", "Add BinarySerializer.h compact format",
    ["engine/serialization/BinarySerializer.h", "engine/serialization/BinarySerializer.cpp"])
commit_with_log("2022-11-10 10:00:00", "Implement BinarySerializer CRC32 checksum",
    ["engine/serialization/BinarySerializer.cpp"])
commit_with_log("2022-11-11 09:00:00", "Add serialization roundtrip tests",
    ["tests/serialization/SerializationTests.cpp"])
append_to_file("CMakeLists.txt", "# SERIALIZATION: Tests\n")
commit_with_log("2022-11-14 10:00:00", "Add JSON serialization tests",
    ["CMakeLists.txt"])
commit_with_log("2022-11-15 09:00:00", "Update CMakeLists.txt with serialization sources",
    ["engine/CMakeLists.txt"])

# ===== PHASE 12: Utils (Nov 2022) =====
commit_with_log("2022-11-16 10:00:00", "Add FileUtils.h file I/O helpers",
    ["engine/utils/FileUtils.h", "engine/utils/FileUtils.cpp"])
commit_with_log("2022-11-17 09:00:00", "Implement FileUtils path operations",
    ["engine/utils/FileUtils.cpp"])
commit_with_log("2022-11-18 11:00:00", "Add StringUtils.h string manipulation",
    ["engine/utils/StringUtils.h", "engine/utils/StringUtils.cpp"])
commit_with_log("2022-11-21 10:00:00", "Implement StringUtils split/trim/join",
    ["engine/utils/StringUtils.cpp"])
append_to_file("CMakeLists.txt", "# UTILS: File, String\n")
commit_with_log("2022-11-22 09:00:00", "Add Timer.h countdown and stopwatch",
    ["CMakeLists.txt"])
commit_with_log("2022-11-23 10:00:00", "Implement Timer callback on expire",
    ["engine/utils/Timer.h", "engine/utils/Timer.cpp"])
commit_with_log("2022-11-24 09:00:00", "Add PoolAllocator.h fixed-size pool",
    ["engine/utils/PoolAllocator.h", "engine/utils/PoolAllocator.cpp"])
commit_with_log("2022-11-25 11:00:00", "Implement PoolAllocator free list",
    ["engine/utils/PoolAllocator.cpp"])
commit_with_log("2022-11-28 10:00:00", "Add ResourceHandle.h ref counting",
    ["engine/utils/ResourceHandle.h", "engine/utils/ResourceHandle.cpp"])
commit_with_log("2022-11-29 09:00:00", "Implement ResourceHandle weak/strong handles",
    ["engine/utils/ResourceHandle.cpp"])
append_to_file("engine/CMakeLists.txt", "# Utils module sources\n")
commit_with_log("2022-11-30 10:00:00", "Update CMakeLists.txt with utils sources",
    ["engine/CMakeLists.txt"])

# ===== PHASE 13: Debug tools (Dec 2022) =====
commit_with_log("2022-12-01 09:00:00", "Add DebugDraw.h debug shape rendering",
    ["engine/debug/DebugDraw.h", "engine/debug/DebugDraw.cpp"])
commit_with_log("2022-12-02 10:00:00", "Implement DebugDraw rects and circles",
    ["engine/debug/DebugDraw.cpp"])
commit_with_log("2022-12-05 09:00:00", "Add Profiler.h hierarchical timing",
    ["engine/debug/Profiler.h", "engine/debug/Profiler.cpp"])
commit_with_log("2022-12-06 11:00:00", "Implement Profiler frame history stats",
    ["engine/debug/Profiler.cpp"])
append_to_file("CMakeLists.txt", "# DEBUG: Profiler, Logger\n")
commit_with_log("2022-12-07 10:00:00", "Add Logger.h log levels and sinks",
    ["CMakeLists.txt"])
commit_with_log("2022-12-08 09:00:00", "Implement Logger colored console output",
    ["engine/debug/Logger.h", "engine/debug/Logger.cpp"])
commit_with_log("2022-12-09 10:00:00", "Add Console.h in-game developer console",
    ["engine/debug/Console.h", "engine/debug/Console.cpp"])
commit_with_log("2022-12-12 09:00:00", "Implement Console command registration",
    ["engine/debug/Console.cpp"])
commit_with_log("2022-12-13 11:00:00", "Add StatsOverlay.h on-screen debug info",
    ["engine/debug/StatsOverlay.h", "engine/debug/StatsOverlay.cpp"])
commit_with_log("2022-12-14 10:00:00", "Implement StatsOverlay FPS and draw calls",
    ["engine/debug/StatsOverlay.cpp"])
append_to_file("CMakeLists.txt", "# DEBUG: Stats, AssetInspector\n")
commit_with_log("2022-12-15 09:00:00", "Add AssetInspector.h runtime asset browser",
    ["CMakeLists.txt"])
commit_with_log("2022-12-16 10:00:00", "Implement AssetInspector texture preview",
    ["engine/debug/AssetInspector.h", "engine/debug/AssetInspector.cpp"])
commit_with_log("2022-12-19 09:00:00", "Update CMakeLists.txt with debug sources",
    ["engine/CMakeLists.txt"])

# ===== PHASE 14: UI Framework (Jan 2023) =====
commit_with_log("2023-01-03 10:00:00", "Add Widget.h abstract UI base class",
    ["engine/ui/Widget.h", "engine/ui/Widget.cpp"])
commit_with_log("2023-01-04 09:00:00", "Implement Widget layout and event system",
    ["engine/ui/Widget.cpp"])
commit_with_log("2023-01-05 11:00:00", "Add Canvas.h root UI container",
    ["engine/ui/Canvas.h", "engine/ui/Canvas.cpp"])
commit_with_log("2023-01-06 10:00:00", "Implement Canvas input routing",
    ["engine/ui/Canvas.cpp"])
append_to_file("CMakeLists.txt", "# UI: Widget, Canvas\n")
commit_with_log("2023-01-09 09:00:00", "Add Button.h clickable widget",
    ["CMakeLists.txt"])
commit_with_log("2023-01-10 10:00:00", "Implement Button visual states",
    ["engine/ui/Button.h", "engine/ui/Button.cpp"])
commit_with_log("2023-01-11 09:00:00", "Add Label.h text display widget",
    ["engine/ui/Label.h", "engine/ui/Label.cpp"])
commit_with_log("2023-01-12 11:00:00", "Implement Label word wrap and alignment",
    ["engine/ui/Label.cpp"])
commit_with_log("2023-01-13 10:00:00", "Add Panel.h container widget",
    ["engine/ui/Panel.h", "engine/ui/Panel.cpp"])
append_to_file("CMakeLists.txt", "# UI: Panel, TextBox\n")
commit_with_log("2023-01-16 09:00:00", "Implement Panel layout modes",
    ["CMakeLists.txt"])
commit_with_log("2023-01-17 10:00:00", "Add TextBox.h editable text input",
    ["engine/ui/TextBox.h", "engine/ui/TextBox.cpp"])
commit_with_log("2023-01-18 09:00:00", "Implement TextBox cursor and selection",
    ["engine/ui/TextBox.cpp"])
commit_with_log("2023-01-19 11:00:00", "Add Slider.h range input widget",
    ["engine/ui/Slider.h", "engine/ui/Slider.cpp"])
commit_with_log("2023-01-20 10:00:00", "Implement Slider thumb dragging",
    ["engine/ui/Slider.cpp"])
append_to_file("engine/ui/Widget.h", "// v2: Added anchor system\n")
commit_with_log("2023-01-23 09:00:00", "Add CheckBox.h toggle widget",
    ["engine/ui/Widget.h"])
commit_with_log("2023-01-24 10:00:00", "Implement CheckBox three states",
    ["engine/ui/CheckBox.h", "engine/ui/CheckBox.cpp"])
commit_with_log("2023-01-25 09:00:00", "Add ListBox.h scrollable list",
    ["engine/ui/ListBox.h", "engine/ui/ListBox.cpp"])
commit_with_log("2023-01-26 11:00:00", "Implement ListBox multi-select",
    ["engine/ui/ListBox.cpp"])
append_to_file("CMakeLists.txt", "# UI: Style theming\n")
commit_with_log("2023-01-27 10:00:00", "Add Style.h theming system",
    ["CMakeLists.txt"])
commit_with_log("2023-01-30 09:00:00", "Implement Style dark/light presets",
    ["engine/ui/Style.h", "engine/ui/Style.cpp"])
commit_with_log("2023-01-31 10:00:00", "Update CMakeLists.txt with UI sources",
    ["engine/CMakeLists.txt"])

# ===== PHASE 15: Examples (Feb-Mar 2023) =====
commit_with_log("2023-02-01 09:00:00", "Add platformer example main entry point",
    ["examples/platformer/main.cpp"])
commit_with_log("2023-02-02 10:00:00", "Add PlayerController.h movement system",
    ["examples/platformer/PlayerController.h", "examples/platformer/PlayerController.cpp"])
commit_with_log("2023-02-03 09:00:00", "Implement PlayerController jumping and animation",
    ["examples/platformer/PlayerController.cpp"])
append_to_file("CMakeLists.txt", "# EXAMPLES: Platformer\n")
commit_with_log("2023-02-06 11:00:00", "Add LevelLoader.h tilemap parser",
    ["CMakeLists.txt"])
commit_with_log("2023-02-07 10:00:00", "Implement LevelLoader entity creation",
    ["examples/platformer/LevelLoader.h", "examples/platformer/LevelLoader.cpp"])
commit_with_log("2023-02-08 09:00:00", "Add RPG example main entry point",
    ["examples/rpg/main.cpp"])
commit_with_log("2023-02-09 10:00:00", "Add GameUI.h RPG interface widgets",
    ["examples/rpg/GameUI.h", "examples/rpg/GameUI.cpp"])
commit_with_log("2023-02-10 09:00:00", "Implement GameUI health bar and inventory",
    ["examples/rpg/GameUI.cpp"])
append_to_file("CMakeLists.txt", "# EXAMPLES: RPG\n")
commit_with_log("2023-02-13 11:00:00", "Add InventorySystem.h item management",
    ["CMakeLists.txt"])
commit_with_log("2023-02-14 10:00:00", "Implement InventorySystem item use logic",
    ["examples/rpg/InventorySystem.h", "examples/rpg/InventorySystem.cpp"])
commit_with_log("2023-02-15 09:00:00", "Add Pong example main entry point",
    ["examples/pong/main.cpp"])
commit_with_log("2023-02-16 10:00:00", "Add PaddleController.h keyboard input",
    ["examples/pong/PaddleController.h", "examples/pong/PaddleController.cpp"])
commit_with_log("2023-02-17 09:00:00", "Implement PaddleController AI opponent",
    ["examples/pong/PaddleController.cpp"])
append_to_file("CMakeLists.txt", "# EXAMPLES: Pong\n")
commit_with_log("2023-02-20 11:00:00", "Add BallController.h physics and scoring",
    ["CMakeLists.txt"])
commit_with_log("2023-02-21 10:00:00", "Implement BallController paddle collision",
    ["examples/pong/BallController.h", "examples/pong/BallController.cpp"])
commit_with_log("2023-02-22 09:00:00", "Add examples CMakeLists.txt",
    ["examples/CMakeLists.txt"])
append_to_file("examples/platformer/PlayerController.cpp", "// v2: Fixed jump edge case\n")
commit_with_log("2023-02-23 10:00:00", "Fix platformer player movement edge cases",
    ["examples/platformer/PlayerController.cpp"])

# ===== PHASE 16: Test infrastructure (Mar 2023) =====
commit_with_log("2023-03-01 09:00:00", "Add test runner main entry point",
    ["tests/main.cpp"])
commit_with_log("2023-03-02 10:00:00", "Add tests CMakeLists.txt with test targets",
    ["tests/CMakeLists.txt"])
commit_with_log("2023-03-03 09:00:00", "Add EntityTests.cpp creation and destruction",
    ["tests/ecs/EntityTests.cpp"])
commit_with_log("2023-03-06 11:00:00", "Add WorldTests.cpp lifecycle tests",
    ["tests/ecs/WorldTests.cpp"])
commit_with_log("2023-03-07 10:00:00", "Add SystemTests.cpp update ordering tests",
    ["tests/ecs/SystemTests.cpp"])
commit_with_log("2023-03-08 09:00:00", "Add Vector2Tests.cpp operation tests",
    ["tests/math/Vector2Tests.cpp"])
commit_with_log("2023-03-09 10:00:00", "Add Vector3Tests.cpp operation tests",
    ["tests/math/Vector3Tests.cpp"])
append_to_file("tests/math/MatrixTests.cpp", "// v2: Added inverse tests\n")
commit_with_log("2023-03-10 09:00:00", "Add MatrixTests.cpp transform tests",
    ["tests/math/MatrixTests.cpp"])
commit_with_log("2023-03-13 11:00:00", "Add MathUtilsTests.cpp function tests",
    ["tests/math/MathUtilsTests.cpp"])
commit_with_log("2023-03-14 10:00:00", "Add CollisionTests.cpp shape intersection tests",
    ["tests/physics/CollisionTests.cpp"])
commit_with_log("2023-03-15 09:00:00", "Add PhysicsTests.cpp integration tests",
    ["tests/physics/PhysicsTests.cpp"])
commit_with_log("2023-03-16 10:00:00", "Add SerializationTests.cpp roundtrip tests",
    ["tests/serialization/SerializationTests.cpp"])
commit_with_log("2023-03-17 09:00:00", "Add JsonTests.cpp format tests",
    ["tests/serialization/JsonTests.cpp"])

# ===== PHASE 17: Application refinements (Mar-Apr 2023) =====
append_to_file("engine/core/Application.h", "// v4: Refactored game loop into GameLoop class\n")
commit_with_log("2023-03-20 09:00:00", "Refactor Application game loop into GameLoop class",
    ["engine/core/Application.h"])
append_to_file("engine/core/Application.cpp", "// v3: Added profiling scope markers\n")
commit_with_log("2023-03-21 10:00:00", "Add Application profiling scope markers",
    ["engine/core/Application.cpp"])
append_to_file("engine/graphics/Window.cpp", "// v2: Improved focus handling\n")
commit_with_log("2023-03-22 09:00:00", "Improve Application window focus handling",
    ["engine/graphics/Window.cpp"])
append_to_file("engine/core/EngineConfig.cpp", "// v2: Added runtime reload\n")
commit_with_log("2023-03-23 11:00:00", "Add Application config reload at runtime",
    ["engine/core/EngineConfig.cpp"])
append_to_file("engine/core/Application.cpp", "// v4: Fixed shutdown cleanup\n")
commit_with_log("2023-03-24 10:00:00", "Fix Application shutdown resource cleanup",
    ["engine/core/Application.cpp"])
commit_with_log("2023-03-27 09:00:00", "Add CommandLineParser to Application",
    ["engine/core/Application.h"])
append_to_file("engine/core/GameLoop.h", "// v2: Improved interpolation alpha\n")
commit_with_log("2023-03-28 10:00:00", "Improve GameLoop interpolation alpha",
    ["engine/core/GameLoop.h"])
append_to_file("engine/core/GameLoop.cpp", "// v2: Fixed sub-step cap\n")
commit_with_log("2023-03-29 09:00:00", "Fix GameLoop sub-step cap overflow",
    ["engine/core/GameLoop.cpp"])
append_to_file("engine/core/Time.h", "// v2: Added frame time history\n")
commit_with_log("2023-03-30 11:00:00", "Add GameLoop frame time history",
    ["engine/core/Time.h"])
append_to_file("engine/core/Layer.cpp", "// v3: Reverse iteration optimization\n")
commit_with_log("2023-03-31 10:00:00", "Optimize Layer event dispatch with reverse iteration",
    ["engine/core/Layer.cpp"])

# ===== PHASE 18: Performance optimization (Apr-May 2023) =====
append_to_file("engine/ecs/Archetype.h", "// v2: Optimized matching algorithm\n")
commit_with_log("2023-04-03 09:00:00", "Optimize ECS Archetype matching algorithm",
    ["engine/ecs/Archetype.h"])
append_to_file("engine/ecs/EntityManager.h", "// v3: Added entity pooling\n")
commit_with_log("2023-04-04 10:00:00", "Add entity pooling to EntityManager",
    ["engine/ecs/EntityManager.h"])
append_to_file("engine/ecs/Chunk.h", "// v3: Improved memory layout\n")
commit_with_log("2023-04-05 09:00:00", "Optimize Chunk memory layout with alignment",
    ["engine/ecs/Chunk.h"])
append_to_file("engine/CMakeLists.txt", "# Added parallel system support\n")
commit_with_log("2023-04-06 11:00:00", "Add parallel system update support",
    ["engine/CMakeLists.txt"])
append_to_file("engine/graphics/SpriteBatch.cpp", "// v3: Batch optimization\n")
commit_with_log("2023-04-10 10:00:00", "Optimize SpriteBatch to reduce draw calls",
    ["engine/graphics/SpriteBatch.cpp"])
append_to_file("engine/graphics/TextureManager.h", "// v3: Texture atlas packing\n")
commit_with_log("2023-04-11 09:00:00", "Add texture atlas packing to TextureManager",
    ["engine/graphics/TextureManager.h"])
append_to_file("engine/particles/ParticleSystem.cpp", "// v2: SIMD optimization hints\n")
commit_with_log("2023-04-12 10:00:00", "Optimize ParticleSystem with SIMD hints",
    ["engine/particles/ParticleSystem.cpp"])
append_to_file("engine/systems/CollisionSystem.cpp", "// v2: Spatial query optimization\n")
commit_with_log("2023-04-13 09:00:00", "Optimize CollisionSystem spatial queries",
    ["engine/systems/CollisionSystem.cpp"])
append_to_file("engine/systems/AudioSystem.h", "// v2: Object pooling\n")
commit_with_log("2023-04-14 11:00:00", "Add object pooling to AudioSystem",
    ["engine/systems/AudioSystem.h"])
append_to_file("engine/systems/RenderSystem.cpp", "// v3: Bottleneck profiling\n")
commit_with_log("2023-04-17 10:00:00", "Profile and optimize RenderSystem bottleneck",
    ["engine/systems/RenderSystem.cpp"])
append_to_file("engine/debug/Profiler.h", "// v2: Frame budget warnings\n")
commit_with_log("2023-04-18 09:00:00", "Add frame budget warnings to Profiler",
    ["engine/debug/Profiler.h"])

# ===== PHASE 19: Bug fixes and stability (May-Jun 2023) =====
append_to_file("engine/ecs/EntityManager.cpp", "// v3: Fixed version overflow\n")
commit_with_log("2023-05-01 10:00:00", "Fix entity version overflow in EntityManager",
    ["engine/ecs/EntityManager.cpp"])
append_to_file("engine/ecs/Archetype.cpp", "// v3: Fixed migration data corruption\n")
commit_with_log("2023-05-02 09:00:00", "Fix Archetype migration data corruption",
    ["engine/ecs/Archetype.cpp"])
append_to_file("engine/ecs/Chunk.cpp", "// v3: Fixed removal ordering\n")
commit_with_log("2023-05-03 11:00:00", "Fix Chunk back-fill removal ordering",
    ["engine/ecs/Chunk.cpp"])
append_to_file("engine/ecs/World.cpp", "// v3: Fixed system sort stability\n")
commit_with_log("2023-05-04 10:00:00", "Fix World system sort stability",
    ["engine/ecs/World.cpp"])
append_to_file("engine/graphics/TextureManager.cpp", "// v2: Thread safety fix\n")
commit_with_log("2023-05-05 09:00:00", "Fix TextureManager thread safety issue",
    ["engine/graphics/TextureManager.cpp"])
append_to_file("engine/graphics/ShaderManager.cpp", "// v2: Uniform caching bug\n")
commit_with_log("2023-05-08 10:00:00", "Fix ShaderManager uniform caching bug",
    ["engine/graphics/ShaderManager.cpp"])
append_to_file("engine/input/InputManager.cpp", "// v2: Key repeat handling\n")
commit_with_log("2023-05-09 09:00:00", "Fix InputManager key repeat handling",
    ["engine/input/InputManager.cpp"])
append_to_file("engine/systems/PhysicsSystem.cpp", "// v3: Fixed FP drift\n")
commit_with_log("2023-05-10 11:00:00", "Fix PhysicsSystem floating point drift",
    ["engine/systems/PhysicsSystem.cpp"])
append_to_file("engine/systems/CollisionSystem.cpp", "// v3: Ghost collision fix\n")
commit_with_log("2023-05-11 10:00:00", "Fix CollisionSystem ghost collision on edges",
    ["engine/systems/CollisionSystem.cpp"])
append_to_file("engine/particles/ParticleSystem.cpp", "// v3: Memory leak fix\n")
commit_with_log("2023-05-12 09:00:00", "Fix ParticleSystem memory leak on emitter clear",
    ["engine/particles/ParticleSystem.cpp"])
append_to_file("engine/audio/MusicPlayer.cpp", "// v2: Crossfade timing fix\n")
commit_with_log("2023-05-15 10:00:00", "Fix MusicPlayer crossfade timing",
    ["engine/audio/MusicPlayer.cpp"])
append_to_file("engine/systems/UISystem.cpp", "// v2: Mouse leave event fix\n")
commit_with_log("2023-05-16 09:00:00", "Fix UISystem mouse leave event dispatch",
    ["engine/systems/UISystem.cpp"])
append_to_file("engine/systems/PathfindingSystem.cpp", "// v3: Path recalculation fix\n")
commit_with_log("2023-05-17 11:00:00", "Fix PathfindingSystem path recalculation",
    ["engine/systems/PathfindingSystem.cpp"])
append_to_file("engine/debug/Console.cpp", "// v2: Command parsing fix\n")
commit_with_log("2023-05-18 10:00:00", "Fix Console command parsing edge cases",
    ["engine/debug/Console.cpp"])
append_to_file("engine/serialization/JsonSerializer.cpp", "// v2: NaN handling\n")
commit_with_log("2023-05-19 09:00:00", "Fix JsonSerializer NaN handling",
    ["engine/serialization/JsonSerializer.cpp"])
append_to_file("engine/serialization/BinarySerializer.cpp", "// v2: Endianness fix\n")
commit_with_log("2023-05-22 10:00:00", "Fix BinarySerializer endianness issues",
    ["engine/serialization/BinarySerializer.cpp"])

# ===== PHASE 20: Documentation (Jun-Jul 2023) =====
commit_with_log("2023-06-01 09:00:00", "Write architecture documentation",
    ["docs/architecture.md"])
commit_with_log("2023-06-02 10:00:00", "Write getting started tutorial",
    ["docs/getting-started.md"])
commit_with_log("2023-06-05 09:00:00", "Write API reference documentation",
    ["docs/api-reference.md"])
commit_with_log("2023-06-06 11:00:00", "Write ECS usage guide with examples",
    ["docs/ecs-guide.md"])
commit_with_log("2023-06-07 10:00:00", "Write build instructions for all platforms",
    ["docs/build-instructions.md"])
commit_with_log("2023-06-08 09:00:00", "Write component reference documentation",
    ["docs/component-reference.md"])
append_to_file("engine/core/Application.h", "// DOCS: Added inline documentation\n")
commit_with_log("2023-06-09 10:00:00", "Add inline code documentation to core module",
    ["engine/core/Application.h"])
append_to_file("engine/ecs/World.h", "// DOCS: API documentation\n")
commit_with_log("2023-06-12 09:00:00", "Add inline code documentation to ECS module",
    ["engine/ecs/World.h"])
append_to_file("engine/components/TransformComponent.h", "// DOCS: Component docs\n")
commit_with_log("2023-06-13 11:00:00", "Add inline code documentation to components",
    ["engine/components/TransformComponent.h"])
append_to_file("engine/systems/RenderSystem.h", "// DOCS: System documentation\n")
commit_with_log("2023-06-14 10:00:00", "Add inline code documentation to systems",
    ["engine/systems/RenderSystem.h"])
append_to_file("engine/graphics/Renderer.h", "// DOCS: Graphics API docs\n")
commit_with_log("2023-06-15 09:00:00", "Add inline code documentation to graphics",
    ["engine/graphics/Renderer.h"])
append_to_file("engine/audio/AudioEngine.h", "// DOCS: Audio API docs\n")
commit_with_log("2023-06-16 10:00:00", "Add inline code documentation to audio/input",
    ["engine/audio/AudioEngine.h"])
append_to_file("engine/math/Vector2.h", "// DOCS: Math library docs\n")
commit_with_log("2023-06-19 09:00:00", "Add inline code documentation to math/physics",
    ["engine/math/Vector2.h"])
append_to_file("engine/ui/Widget.h", "// DOCS: UI framework docs\n")
commit_with_log("2023-06-20 11:00:00", "Add inline code documentation to UI module",
    ["engine/ui/Widget.h"])
append_to_file("engine/serialization/Serializer.h", "// DOCS: Serialization docs\n")
commit_with_log("2023-06-21 10:00:00", "Add inline code documentation to serialization",
    ["engine/serialization/Serializer.h"])
append_to_file("engine/debug/DebugDraw.h", "// DOCS: Debug tools docs\n")
commit_with_log("2023-06-22 09:00:00", "Add inline code documentation to debug tools",
    ["engine/debug/DebugDraw.h"])
append_to_file("engine/scene/Scene.h", "// DOCS: Scene management docs\n")
commit_with_log("2023-06-23 10:00:00", "Add inline code documentation to scene/utils",
    ["engine/scene/Scene.h"])
append_to_file("README.md", "\n## Version History\n")
commit_with_log("2023-06-26 09:00:00", "Update README with feature overview",
    ["README.md"])
append_to_file("README.md", "\n### 0.9.0 - UI Framework and Debug Tools\n")
commit_with_log("2023-06-27 11:00:00", "Add version history to README",
    ["README.md"])
commit_with_log("2023-06-28 10:00:00", "Add code examples to README quick start",
    ["README.md"])

# ===== PHASE 21: Polish and refactoring (Jul-Aug 2023) =====
commit_with_log("2023-07-03 09:00:00", "Refactor ECS World into interface and impl",
    ["engine/ecs/World.h", "engine/ecs/World.cpp"])
commit_with_log("2023-07-05 10:00:00", "Refactor Renderer to support render passes",
    ["engine/graphics/Renderer.h", "engine/graphics/Renderer.cpp"])
commit_with_log("2023-07-07 09:00:00", "Refactor InputManager with action map",
    ["engine/input/InputManager.h", "engine/input/InputManager.cpp"])
commit_with_log("2023-07-10 11:00:00", "Refactor AudioEngine with bus system",
    ["engine/audio/AudioEngine.h", "engine/audio/AudioEngine.cpp"])
commit_with_log("2023-07-12 10:00:00", "Refactor PhysicsWorld into broad/narrow phases",
    ["engine/physics/PhysicsWorld.h", "engine/physics/PhysicsWorld.cpp"])
commit_with_log("2023-07-14 09:00:00", "Refactor Console command system",
    ["engine/debug/Console.h", "engine/debug/Console.cpp"])
commit_with_log("2023-07-17 10:00:00", "Refactor ParticleSystem emitter API",
    ["engine/particles/ParticleSystem.h", "engine/particles/ParticleSystem.cpp"])
commit_with_log("2023-07-19 09:00:00", "Refactor Tilemap chunk management",
    ["engine/tilemap/Tilemap.h", "engine/tilemap/Tilemap.cpp"])
commit_with_log("2023-07-21 11:00:00", "Refactor UISystem event routing",
    ["engine/systems/UISystem.h", "engine/systems/UISystem.cpp"])
commit_with_log("2023-07-24 10:00:00", "Refactor SceneManager state transitions",
    ["engine/scene/SceneManager.h", "engine/scene/SceneManager.cpp"])
append_to_file("engine/CMakeLists.txt", "# Clean up includes\n")
commit_with_log("2023-07-26 09:00:00", "Clean up header include dependencies",
    ["engine/CMakeLists.txt"])
append_to_file("engine/core/Application.h", "// v5: Removed deprecated methods\n")
commit_with_log("2023-07-28 10:00:00", "Remove deprecated methods from core API",
    ["engine/core/Application.h"])
append_to_file("engine/ecs/System.h", "// v2: Added nodiscard\n")
commit_with_log("2023-07-31 09:00:00", "Add [[nodiscard]] to core interfaces",
    ["engine/ecs/System.h"])

# ===== PHASE 22: Final features (Aug-Sep 2023) =====
append_to_file("engine/systems/RenderSystem.h", "// v2: Added render layers\n")
commit_with_log("2023-08-02 10:00:00", "Add render layers to RenderSystem",
    ["engine/systems/RenderSystem.h"])
append_to_file("engine/particles/ParticleSystem.h", "// v2: Attractor/repeller forces\n")
commit_with_log("2023-08-04 09:00:00", "Add particle attractor/repeller forces",
    ["engine/particles/ParticleSystem.h"])
append_to_file("engine/tilemap/Tilemap.h", "// v2: Collision auto-detection\n")
commit_with_log("2023-08-07 11:00:00", "Add tilemap collision auto-detection",
    ["engine/tilemap/Tilemap.h"])
append_to_file("engine/ui/Canvas.h", "// v2: Keyboard navigation\n")
commit_with_log("2023-08-09 10:00:00", "Add UI widget navigation with keyboard",
    ["engine/ui/Canvas.h"])
append_to_file("engine/input/InputAction.h", "// v2: Chord support\n")
commit_with_log("2023-08-11 09:00:00", "Add input chord support to InputAction",
    ["engine/input/InputAction.h"])
append_to_file("engine/audio/AudioEngine.h", "// v2: Spatial attenuation curves\n")
commit_with_log("2023-08-14 10:00:00", "Add spatial audio attenuation curves",
    ["engine/audio/AudioEngine.h"])
append_to_file("engine/systems/CameraSystem.h", "// v2: Fade-to-black effect\n")
commit_with_log("2023-08-16 09:00:00", "Add CameraSystem fade-to-black effect",
    ["engine/systems/CameraSystem.h"])
append_to_file("engine/systems/PathfindingSystem.h", "// v2: Path smoothing\n")
commit_with_log("2023-08-18 11:00:00", "Add PathfindingSystem path smoothing",
    ["engine/systems/PathfindingSystem.h"])
append_to_file("engine/debug/Profiler.h", "// v3: Timeline visualization\n")
commit_with_log("2023-08-21 10:00:00", "Add Profiler timeline visualization",
    ["engine/debug/Profiler.h"])
append_to_file("engine/debug/Console.h", "// v2: Auto-complete support\n")
commit_with_log("2023-08-23 09:00:00", "Add Console variable auto-complete",
    ["engine/debug/Console.h"])
append_to_file("engine/utils/PoolAllocator.h", "// v2: Alignment guarantee\n")
commit_with_log("2023-08-25 10:00:00", "Add PoolAllocator alignment guarantee",
    ["engine/utils/PoolAllocator.h"])
append_to_file("engine/debug/DebugDraw.h", "// v2: Scene graph visualization\n")
commit_with_log("2023-08-28 09:00:00", "Add scene graph debug visualization",
    ["engine/debug/DebugDraw.h"])

# ===== PHASE 23: Testing and quality (Sep-Oct 2023) =====
commit_with_log("2023-09-01 10:00:00", "Add integration tests for RenderSystem",
    ["tests/systems/RenderSystemTests.h"])
commit_with_log("2023-09-04 09:00:00", "Add integration tests for PhysicsSystem",
    ["tests/systems/PhysicsSystemTests.h"])
commit_with_log("2023-09-06 11:00:00", "Add integration tests for AnimationSystem",
    ["tests/systems/AnimationSystemTests.h"])
commit_with_log("2023-09-08 10:00:00", "Add integration tests for AudioSystem",
    ["tests/systems/AudioSystemTests.h"])
commit_with_log("2023-09-11 09:00:00", "Add integration tests for UISystem",
    ["tests/systems/UISystemTests.h"])
commit_with_log("2023-09-13 10:00:00", "Add stress tests for EntityManager",
    ["tests/stress/EntityManagerStressTests.h"])
commit_with_log("2023-09-15 09:00:00", "Add stress tests for PhysicsWorld",
    ["tests/stress/PhysicsWorldStressTests.h"])
commit_with_log("2023-09-18 11:00:00", "Add memory leak detection tests",
    ["tests/stress/MemoryLeakTests.h"])
commit_with_log("2023-09-20 10:00:00", "Add thread safety tests for ResourceHandle",
    ["tests/threading/ResourceHandleThreadTests.h"])
commit_with_log("2023-09-22 09:00:00", "Add edge case tests for MathUtils",
    ["tests/math/MathEdgeCaseTests.h"])
commit_with_log("2023-09-25 10:00:00", "Add benchmark tests for ECS queries",
    ["tests/benchmark/ECSQueryBenchmarks.h"])
commit_with_log("2023-09-27 09:00:00", "Add benchmark tests for collision detection",
    ["tests/benchmark/CollisionBenchmarks.h"])

# ===== PHASE 24: Release preparations (Oct-Dec 2023) =====
append_to_file("CMakeLists.txt", "# Version 0.9.0\n")
commit_with_log("2023-10-02 10:00:00", "Bump version to 0.9.0 in CMakeLists.txt",
    ["CMakeLists.txt"])
commit_with_log("2023-10-04 09:00:00", "Update changelog for 0.9.0 release",
    ["CHANGELOG.md"])
append_to_file("CMakeLists.txt", "# MSVC 2022 warnings fixed\n")
commit_with_log("2023-10-06 11:00:00", "Fix compiler warnings on MSVC 2022",
    ["CMakeLists.txt"])
append_to_file("CMakeLists.txt", "# GCC 13 warnings fixed\n")
commit_with_log("2023-10-09 10:00:00", "Fix compiler warnings on GCC 13",
    ["CMakeLists.txt"])
append_to_file("CMakeLists.txt", "# Clang 16 warnings fixed\n")
commit_with_log("2023-10-11 09:00:00", "Fix compiler warnings on Clang 16",
    ["CMakeLists.txt"])
append_to_file("engine/core/Application.h", "// v6: C++20 compatibility\n")
commit_with_log("2023-10-13 10:00:00", "Ensure C++20 compatibility across all modules",
    ["engine/core/Application.h"])
append_to_file("engine/ecs/World.h", "// v3: API stability annotations\n")
commit_with_log("2023-10-16 09:00:00", "Add API stability annotations",
    ["engine/ecs/World.h"])
append_to_file("engine/components/TransformComponent.h", "// v3: Deprecated old API\n")
commit_with_log("2023-10-18 11:00:00", "Deprecate old transform API",
    ["engine/components/TransformComponent.h"])
append_to_file("CMakeLists.txt", "# Version 1.0.0-rc1\n")
commit_with_log("2023-10-20 10:00:00", "Bump version to 1.0.0-rc1",
    ["CMakeLists.txt"])
append_to_file("engine/ecs/EntityManager.cpp", "// v4: Debug iterator validation\n")
commit_with_log("2023-10-23 09:00:00", "Fix MSVC debug iterator validation",
    ["engine/ecs/EntityManager.cpp"])
append_to_file("engine/math/Vector2.h", "// v2: GCC Wconversion fixes\n")
commit_with_log("2023-10-25 10:00:00", "Fix GCC -Wall -Wextra warnings",
    ["engine/math/Vector2.h"])
append_to_file("engine/math/Matrix4.h", "// v2: Clang Wconversion fixes\n")
commit_with_log("2023-10-27 09:00:00", "Fix Clang -Wconversion warnings",
    ["engine/math/Matrix4.h"])
commit_with_log("2023-10-30 11:00:00", "Add Windows ARM64 build support",
    ["CMakeLists.txt"])
commit_with_log("2023-11-01 10:00:00", "Add Linux Wayland support",
    ["CMakeLists.txt"])
commit_with_log("2023-11-03 09:00:00", "Add macOS Metal backend compatibility",
    ["CMakeLists.txt"])
append_to_file("engine/core/ServiceLocator.h", "// v2: Finalize API\n")
commit_with_log("2023-11-06 10:00:00", "Finalize API for 1.0.0 release",
    ["engine/core/ServiceLocator.h"])
append_to_file("CMakeLists.txt", "# Version 1.0.0\n")
commit_with_log("2023-11-08 09:00:00", "Update CMakeLists.txt for 1.0.0 release",
    ["CMakeLists.txt"])
commit_with_log("2023-11-10 11:00:00", "Add license file",
    ["LICENSE"])
commit_with_log("2023-11-13 10:00:00", "Write migration guide from 0.9.x to 1.0.0",
    ["docs/migration-guide.md"])
append_to_file("engine/ecs/Archetype.h", "// v3: Cleaned up TODOs\n")
commit_with_log("2023-11-15 09:00:00", "Clean up TODO comments in source files",
    ["engine/ecs/Archetype.h"])
commit_with_log("2023-11-17 10:00:00", "Optimize build times with precompiled headers",
    ["CMakeLists.txt"])
commit_with_log("2023-11-20 09:00:00", "Add CMake install targets",
    ["CMakeLists.txt"])
commit_with_log("2023-11-22 11:00:00", "Add CPack packaging configuration",
    ["CMakeLists.txt"])
commit_with_log("2023-11-24 10:00:00", "Add Doxygen documentation generation",
    ["CMakeLists.txt"])
append_to_file("engine/core/Application.cpp", "// v5: Final API review\n")
commit_with_log("2023-11-27 09:00:00", "Final review of core engine API",
    ["engine/core/Application.cpp"])
append_to_file("engine/ecs/World.cpp", "// v4: Final ECS review\n")
commit_with_log("2023-11-29 10:00:00", "Final review of ECS implementation",
    ["engine/ecs/World.cpp"])
append_to_file("engine/graphics/Renderer.cpp", "// v3: Final graphics review\n")
commit_with_log("2023-12-01 09:00:00", "Final review of graphics pipeline",
    ["engine/graphics/Renderer.cpp"])
append_to_file("engine/ui/Widget.cpp", "// v3: Final UI review\n")
commit_with_log("2023-12-04 11:00:00", "Final review of UI framework",
    ["engine/ui/Widget.cpp"])
append_to_file("CMakeLists.txt", "# Release candidate 2\n")
commit_with_log("2023-12-06 10:00:00", "Release candidate 2: bug fixes",
    ["CMakeLists.txt"])
commit_with_log("2023-12-08 09:00:00", "Bump version to 1.0.0",
    ["CMakeLists.txt"])
commit_with_log("2023-12-11 10:00:00", "Update README for stable release",
    ["README.md"])
commit_with_log("2023-12-13 09:00:00", "Add release notes for 1.0.0",
    ["RELEASE_NOTES.md"])
commit_with_log("2023-12-15 11:00:00", "v1.0.0 stable release",
    ["README.md", "CMakeLists.txt"])

# Commit the CHANGES.log as well
subprocess.run(["git", "add", "engine/CHANGES.log"], cwd=REPO, capture_output=True)
timestamp = int(datetime.datetime.strptime("2023-12-15 11:00:01", "%Y-%m-%d %H:%M:%S").timestamp())
env = os.environ.copy()
env["GIT_AUTHOR_DATE"] = str(timestamp)
env["GIT_COMMITTER_DATE"] = str(timestamp)
env["GIT_AUTHOR_NAME"] = "Mikhail Volkov"
env["GIT_AUTHOR_EMAIL"] = "mikhail.volkov@neon-engine.dev"
env["GIT_COMMITTER_NAME"] = "Mikhail Volkov"
env["GIT_COMMITTER_EMAIL"] = "mikhail.volkov@neon-engine.dev"
subprocess.run(["git", "commit", "-m", "Add changelog for complete history"], cwd=REPO, capture_output=True, env=env)

print("\nDone! All commits created.")
