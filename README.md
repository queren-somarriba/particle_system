# Particle System

A GPU-based 3D particle system built in C++ using OpenGL + OpenCL with GL/CL interoperability, real-time physics simulation, and point-sprite rendering.

## Dependencies

- OpenGL 4.3+
- OpenCL 2.0+ (with `cl_khr_gl_sharing` extension)
- GLFW
- GLAD
- GLX (Linux/X11)

## Build

```bash
make
```

## Usage

```bash
./particle_system <particle_count>
```

The particle count must be between `1` and `10,000,000`.

```bash
./particle_system 1000000
```

## Controls

| Key / Action | Effect |
|---|---|
| `W` / `A` / `S` / `D` | Move camera (horizontal plane) |
| `↑` / `↓` | Move camera up / down |
| `Mouse` | Move the gravity center (in dynamic gravity mode)|
| `Scroll` | Adjust gravity strength (G between 0 and 20) |
| `Space` | Toggle gravity on / off |
| `G` | Toggle dynamic gravity mode |
| `T` (hold) | Inject turbulence |
| `E` (hold) | Continuously emit particles |
| `C` | Cycle color mode (distance / lifetime → velocity) |
| `L` | Toggle particle lifespan |
| `R` | Reset shape (Cube → Sphere) |
| `Escape` | Quit |

## Features

- Full GPU physics simulation via an OpenCL kernel
- OpenCL / OpenGL interoperability: the VBO is shared with no CPU copy
- Gravity toward a dynamic center that follows the mouse cursor
- Turbulence via 3D value noise
- Two reset shapes: cube or sphere
- Configurable particle lifespan (mortality toggle)
- Two color modes: distance to center and life time or particle speed
- Real-time FPS display in the window title

## GPU Architecture

The OpenGL VBO is created on the CPU then shared with OpenCL via `clCreateFromGLBuffer`. Each frame proceeds as follows:

1. `glFinish()` — OpenGL synchronization
2. `clEnqueueAcquireGLObjects` — OpenCL acquires the VBO
3. `clEnqueueNDRangeKernel` — kernel updates positions and colors
4. `clEnqueueReleaseGLObjects` — OpenCL releases the VBO
5. `clFinish()` — OpenCL synchronization
6. `glDrawArrays(GL_POINTS, ...)` — rendering

## OpenCL Kernel (`simulation.cl`)

Each work item corresponds to one particle. The kernel handles:

- Velocity and position update (Euler integration)
- Gravitational attraction toward `gCenter`
- Additive turbulence via `valueNoise3D`
- Shape reset on cube or sphere (hash-based RNG)
- Lifespan management and particle re-emission
- Color computation (distance or speed mode)

## Structure

```
.
├── src/
│   ├── main.cpp          # Entry point, GL + CL init, main loop
│   ├── renderer.cpp      # Render loop, uniforms, state management
│   ├── cl.cpp            # OpenCL init, interop, kernel compilation
│   └── callback.cpp      # Keyboard, mouse, scroll callbacks
├── shaders/
│   ├── Particle.vs       # Vertex shader (MVP transform)
│   └── Particle.fs       # Fragment shader (per-point color)
├── kernels/
│   └── simulation.cl     # OpenCL kernel: physics, turbulence, reset
└── include/
    └── particle_system.hpp
```

## Notes

- Requires a GPU supporting `cl_khr_gl_sharing` and an active GLX context
- Tested on Linux/X11 only (GLX context is explicitly required)
- Beyond ~5M particles, performance is highly GPU-dependent
- 60 FPS smoothly maintained at 1,000,000 particles.
- 30 FPS sustained at 3,000,000 particles.

