# Build Instructions

## Common Setup

```bash
git clone --recursive https://github.com/xylotech/engine.git
cd engine
```

### CMake Options

| Option | Default | Description |
|---|---|---|
| `XYL_BUILD_EDITOR` | `OFF` | Build the editor application |
| `XYL_BUILD_TOOLS` | `ON` | Build CLI tools (asset baker, scene dumper) |
| `XYL_BUILD_TESTS` | `ON` | Build unit and integration tests |
| `XYL_ENABLE_PROFILING` | `OFF` | Enable Tracy profiler instrumentation |
| `XYL_ENABLE_ASSERTS` | `ON` | Enable runtime assertion checks |
| `XYL_VALIDATION_LAYERS` | `OFF` | Enable Vulkan validation layers |

## Windows (MSVC 2022)

```bash
cmake -B build -G "Visual Studio 17 2022" -DXYL_BUILD_EDITOR=ON
cmake --build build --config Release
```

Open `build/XylotechEngine.sln` in Visual Studio for development.

## Linux (GCC/Clang)

```bash
sudo apt install libx11-dev libxrandr-dev libxcursor-dev libvulkan-dev
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DXYL_BUILD_EDITOR=ON
cmake --build build
```

## macOS (Clang)

```bash
brew install cmake ninja molten-vk
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release \
    -DXYL_BUILD_EDITOR=ON \
    -DXYL_VULKAN_BACKEND=MoltenVK
cmake --build build
```

## Running Tests

```bash
cmake -B build -DXYL_BUILD_TESTS=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## Build Artifacts

After a successful build, outputs are placed in `build/bin/`:

- `XylotechEditor.exe` — Editor application
- `XylotechPlayer.exe` — Standalone runtime
- `assetbaker.exe` — Asset pipeline tool
- `libengine.a` / `engine.lib` — Static library for linking
