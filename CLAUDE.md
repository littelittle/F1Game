# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build System

This is a C++ Formula 1 racing simulation using OpenGL with CMake build system and vcpkg for dependency management.

### Build Commands

**Configure with CMake:**
```bash
cmake -B build -DCMAKE_TOOLCHAIN_FILE=$env{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake
```

**Build the project:**
```bash
cmake --build build --config Debug
```

**Run the application:**
```bash
./build/Debug/F1.exe
```

**Clean build:**
```bash
rm -rf build && mkdir build
```

## Architecture Overview

**Core Components:**
- `Car` class (include/car.h:13) - Handles F1 car physics, rendering, and controls
- `Circuit` class (include/circuit.h:8) - Renders the race track
- `Wheel` class (include/wheel.h) - Individual wheel mechanics
- `Shader` class (include/shader.h) - OpenGL shader management

**Key Features:**
- Real-time car physics with acceleration, braking, and steering
- 3D rendering using OpenGL 3.3 core profile
- OBJ model loading for car assets
- Camera controls with WASD movement
- GLFW for window management and input
- GLEW for OpenGL extension loading
- GLM for mathematics operations

**File Structure:**
- `src/` - Implementation files
- `include/` - Header files
- `assets/` - 3D models and textures
- `build/` - Build output directory

**Dependencies:**
- GLEW (OpenGL Extension Wrangler)
- GLFW (Window and input management)
- GLM (OpenGL Mathematics)
- OpenGL 3.3+

## Development Notes

- Uses C++20 standard
- CMake copies assets to build directory automatically
- No unit tests currently implemented
- Focus on real-time simulation and rendering
- Keyboard controls: WASD (camera), Arrow keys (car controls)