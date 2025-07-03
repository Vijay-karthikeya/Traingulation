# Triangulation-Algorithms in Computational geometry
A C++ OpenGL application that demonstrates **Ear Clipping Triangulation** to solve the **Art Gallery Problem**.
This tool takes a simple polygon as input, decomposes it into triangles using the Ear Clipping algorithm, and visualizes the triangulation process in real-time using GLFW and OpenGL.

Problem statement:
How many guards are sufficient to observe every point inside an art gallery, modeled as a simple polygon?
Triangulation answers this by decomposing the polygon into triangles, which makes visibility analysis and guard placement easier.

Input Format

- The input is read from a file named `data.txt`.
- The format is as follows:
- <number of polygons> <number of vertices in polygon 1> x1,y1 x2,y2 ... <number of vertices in polygon 2> x1,y1 ... ``` - Each polygon must: - Be simple (no self-intersections) - Have vertices listed in **counter-clockwise (CCW)** order - Contain at least 3 vertices

Download Opengl and GLFW, and make sure to link the libraries.
