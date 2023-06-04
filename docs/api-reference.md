# API Reference

## `ecs::World`

| Method | Description |
|---|---|
| `create(Components&&...) -> Entity` | Creates an entity with the given components |
| `destroy(Entity)` | Removes an entity on the next frame |
| `add<T>(Entity, T)` | Adds a component to an existing entity |
| `remove<T>(Entity)` | Removes a component from an entity |
| `get<T>(Entity) -> T*` | Returns pointer to component or nullptr |
| `tick(float dt)` | Runs all registered systems once |
| `query(QueryDesc) -> Query` | Builds a query over matching archetypes |
| `register_system(System)` | Registers a system for the tick loop |

## `ecs::Query`

| Method | Description |
|---|---|
| `each(Fn)` | Iterates every matching entity |
| `par_each(Fn, JobScheduler&)` | Parallel iteration over archetype chunks |
| `count() -> size_t` | Number of matching entities |
| `single() -> Entity` | Asserts exactly one match and returns it |

## `render::RenderDevice`

| Method | Description |
|---|---|
| `create_mesh(MeshDesc) -> Handle<Mesh>` | Uploads mesh data to GPU |
| `create_texture(TextureDesc) -> Handle<Texture>` | Uploads texture to GPU |
| `draw_frame(Camera&, RenderGraph&)` | Executes the render graph for one frame |

## `asset::AssetManager`

| Method | Description |
|---|---|
| `load<T>(path) -> Handle<T>` | Loads an asset (mesh, texture, material) |
| `unload(Handle<T>)` | Decrements refcount, unloads when zero |
| `reload(Handle<T>)` | Hot-reloads an asset from disk |

## `input::InputSystem`

| Method | Description |
|---|---|
| `is_down(Key) -> bool` | Is the key currently held |
| `just_pressed(Key) -> bool` | Was the key pressed this frame |
| `mouse_position() -> vec2` | Current mouse position in screen coords |
| `bind(Key, Action)` | Binds a key to a named action |

## `network::NetworkManager`

| Method | Description |
|---|---|
| `host(port) -> bool` | Starts a listen server |
| `connect(address, port) -> bool` | Connects to a remote host |
| `send(Entity, Reliability)` | Replicates entity state to peers |
| `register_rpc(string, Fn)` | Registers a remote procedure call handler |
