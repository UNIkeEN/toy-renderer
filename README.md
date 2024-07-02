# Toy Renderer

A tiny, cross-platform rendering engine using OpenGL, born from code practice.

## Features

* [x] Supports multiple models with different file types: OBJ, PLY.
* [x] Supports different shaders and camera types.
* [x] GUI with [imgui](https://github.com/ocornut/imgui), supporting keyboard and mouse control.

## Usage

### Requirements

* A C++17 capable compiler.
* [CMake](https://cmake.org/) v3.21 or higher.

### Clone

This repository contains submodules, so please clone it with:

```sh
# SSH
git clone git@github.com:UNIkeEN/toy-renderer.git --recursive
cd toy-renderer

# HTTPS
git clone https://github.com/UNIkeEN/toy-renderer.git --recursive
cd toy-renderer
```

### Build

If you are using Windows with the latest version of Visual Studio, simply run `build.bat`.

For other platform, use CMake to build the project:

```sh
# Change directory to toy-renderer
mkdir build && cd build
cmake .. && make -j
```
