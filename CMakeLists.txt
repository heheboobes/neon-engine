cmake_minimum_required(VERSION 3.20)
project(NeonEngine VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SFML 2.6 COMPONENTS system window graphics audio network REQUIRED)

add_subdirectory(engine)

option(BUILD_EXAMPLES "Build example projects" ON)
option(BUILD_TESTS "Build unit tests" ON)

if(BUILD_EXAMPLES)
    enable_testing()
    add_subdirectory(examples)
endif()

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
# Phase 1: Core infrastructure
# Added Time utility
# Layer system integration
# Service locator pattern
# MATH: Vector2
# MATH: Matrix3/4
# MATH: Random + tests
# MATH: Optimizations complete
# ECS: Entity, Component, Mask
# ECS: World orchestrator
# COMPONENTS: Transform
# COMPONENTS: Audio, Particles
# COMPONENTS: Camera, Light
# COMPONENTS: RigidBody, Pathfinding
# SYSTEMS: Render
# SYSTEMS: Audio, Particles
# SYSTEMS: Camera, UI, Input
# GRAPHICS: Renderer
# GRAPHICS: Camera, View
