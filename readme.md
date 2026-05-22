"""# 🎆 Particle System (OpenGL / OpenCL Interoperability)

100% GPU-bound 3D particle system simulation built from scratch in C++. This project demonstrates graphics pipeline concepts and GPGPU computing by using **OpenGL** for rendering and **OpenCL** for physics computation, sharing memory buffers directly in VRAM to eliminate CPU-GPU bottlenecking.

---

## 🚀 Key Features

- **Zero-Copy Architecture:** OpenGL Vertex Buffer Objects (VBOs) are mapped directly into the OpenCL context. Data never leaves the GPU during the simulation loop.
- **Dynamic Physics Engine:** Real-time calculation of gravity fields, particle lifetimes, and turbulence.
- **Color Modes:** On-the-fly hardware color interpolation based on distance to the gravitational center and particle lifespan or velocity.
- **Interactive Multi-Mode Sandbox:** Live controls for camera movement, field modifications, shape regeneration, and particle stream emission.

---

## 🛠️ Controls & Commands

When you launch the application, the following controls are available to interact with the simulation sandbox:

### 📹 Camera & Navigation
* `W` / `A` / `S` / `D` : Move camera horizontally (Forward / Left / Backward / Right)
* `Up Arrow` / `Down Arrow` : Move camera vertically (Ascend / Descend)

### 🧪 Simulation Controls
* `Mouse Movement` : The center of gravity automatically tracks your screen cursor
* `Spacebar` : Toggle Gravity Center `[ON / OFF]`
* `Mouse Scroll` : Dynamically adjust gravity strength
* `T` (Hold) : Inject fluid-like micro-turbulences into the system
* `E` (Hold) : Trigger continuous real-time particle emission

### 🎨 Rendering & Geometry
* `C` : Cycle between Color Modes (`Distance-based` ➔ `Lifetime-based` ➔ `Velocity Ramping`)
* `L` : Toggle Particle Lifespan `[Active / Immortal Mode]`
* `R` : Reset and cycle initial geometries `[Cube Grid ➔ Sphere Cloud]`
* `ESC` : Safely free GPU resources and close the application

---

## 🏗️ Architectural Overview

The primary engineering goal of this project is the mitigation of the PCIe bus transfer bottleneck. Instead of calculating positions on the CPU and uploading them every frame, the lifecycle of a frame flows as follows:

1. **Acquire:** OpenCL locks ownership of the OpenGL VBO (`clEnqueueAcquireGLObjects`).
2. **Compute:** The OpenCL physics kernel updates particle positions, velocities, and color states directly inside VRAM.
3. **Release:** OpenCL releases the VBO back to OpenGL (`clEnqueueReleaseGLObjects`).
4. **Render:** OpenGL draws the updated vertex array using hardware-accelerated point sprites (`glDrawArrays`).

---

## 📦 Requirements & Installation

### Dependencies
- **OpenGL 4.3+** Core Profile
- **OpenCL 2.0+** (with `cl_khr_gl_sharing` extension enabled by your hardware vendor)
- **GLFW3** & **GLAD** (included/linked)
- **X11 / GLX** (for Linux contexts)

### Compilation
Build the system using the provided Makefile:"""