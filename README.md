<div align="center">
  <h1 align="center">CPU-based-Ray-Tracing</h1>
</div>
<br/>

A raytracer that utilizes only the CPU to render simple 3D models and environments in both sequential and multithreading modes.


## Features

- **Parallelised:** Runs in a multi-threaded environment (if enabled) for a smoother real time rendering
- **Interactive:** Use arrow keys (or WASD keys) to move around, and J&L keys to adjust brightness (Gamma correction)
- **Realistic:** Simulates lightning concepts such as Phong Reflection Model and Schlick’s approximation


## Demo 
Sequential mode            |  Multi-threaded mode
:-------------------------:|:-------------------------:
![](https://github.com/vinamraparakh/CPU-based-Ray-Tracing/blob/main/sequential.gif)  |  ![](https://github.com/vinamraparakh/CPU-based-Ray-Tracing/blob/main/mt.gif)


## Tech Stack

- [OpenGL](https://www.opengl.org/) – framework
- [C++](https://en.cppreference.com/w/) – language


## Inspiration and References

- [Parallelizing Ray Tracing](https://mikeadev.net/2019/11/parallelizing-ray-tracing/) - [Michael Adaixo](https://github.com/mikea15)
- [Tiny Renderer](https://github.com/ssloy/tinyrenderer) - [ssloy](https://github.com/ssloy)
