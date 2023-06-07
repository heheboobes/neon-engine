# Component Reference

## Transform

```cpp
struct Transform {
    vec3 position = {0, 0, 0};
    quat rotation = quat::identity();
    vec3 scale    = {1, 1, 1};
};
```

The fundamental spatial component. Every renderable entity requires a `Transform`. Hierarchical transforms are handled by `Child` and `Parent` components.

```cpp
entity.set(Transform{
    .position = {10, 0, 5},
    .rotation = quat::from_euler({0, glm::radians(45), 0}),
    .scale    = {2, 2, 2}
});
```

## MeshRenderer

```cpp
struct MeshRenderer {
    Handle<Mesh> mesh;
    Handle<Material> material;
    uint32_t layer = 0;  // render layer mask bit
    bool cast_shadows = true;
};
```

Attach to an entity with `Transform` to render a mesh. The material handle references a material asset loaded by the `AssetManager`.

## Camera

```cpp
struct Camera {
    float fov_y     = 60.0f;  // vertical field of view in degrees
    float near_clip = 0.1f;
    float far_clip  = 1000.0f;
    bool  primary   = false;  // only one primary camera renders
};
```

Defines a view into the scene. Set `primary = true` on exactly one camera per frame.

## RigidBody

```cpp
struct RigidBody {
    float mass       = 1.0f;
    float friction   = 0.5f;
    float restitution = 0.3f;
    bool  is_kinematic = false;
};
```

Enables physics simulation on an entity. Requires `Transform`. The physics system integrates velocity and applies forces each tick.

## PointLight

```cpp
struct PointLight {
    vec3  color      = {1, 1, 1};
    float intensity  = 1.0f;
    float radius     = 10.0f;
};
```

Emits light from the entity's position. Up to 256 point lights are supported per frame. Use with care in performance-critical scenes.

## AudioSource

```cpp
struct AudioSource {
    Handle<AudioClip> clip;
    float volume  = 1.0f;
    float pitch   = 1.0f;
    bool  looping = false;
    bool  playing = false;
};
```

Plays a 3D-positioned audio clip. Requires `Transform` for spatial audio. Call `world.send<PlayAudio>(entity)` to trigger playback.

## ScriptComponent

```cpp
struct ScriptComponent {
    std::string script_path;
    sol::protected_function update_fn; // Lua function
};
```

Holds a reference to a Lua script that runs every frame. Use with the scripting system to define game-specific behavior.

## NetworkReplicated

```cpp
struct NetworkReplicated {
    uint32_t owner_id;
    bool     interpolate = true;
    bool     authoritative = false;
};
```

Marks an entity for network replication. Only entities with this component are synchronized between client and server.
