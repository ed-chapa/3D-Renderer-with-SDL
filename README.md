# 🧱 Simple 3D Renderer in C

A lightweight 3D software renderer built from scratch using **C** and **SDL**, designed as a learning project to explore the fundamentals of graphics programming and rendering pipelines.

> 🧑‍🏫 This project was developed while following the **"3D Computer Graphics Programming"** course by [Gustavo Pezzi (Pikuma)](https://pikuma.com), and uses models and an array library provided as part of the course materials.

## 🚀 Features

- Custom 3D rasterizer with support for:
  - Wireframe rendering (with and without vertices)
  - Flat shading
  - Textured model rendering
- Manual implementation of vector and matrix math operations
- Real-time rendering mode switching via keyboard input

## 🛠️ Technologies

- **Language**: C
- **Graphics**: SDL2
- **External Assets**: Provided by Gustavo Pezzi (Pikuma)

## 🧩 Rendering Modes

Once the program is running, press keys **1–5** to switch between rendering modes:

| Key | Mode Description              |
|-----|-------------------------------|
| 1   | Wireframe with vertices       |
| 2   | Wireframe only                |
| 3   | Flat shading                  |
| 4   | Shading + wireframe           |
| 5   | Textured model                |

## 📦 Build Instructions

Make sure SDL2 is installed on your system. Then run:

```bash
make
make run
