

# Changes

- GPU (Compute Shaders) now available 
- Limitations:
  - When voxel size < 0.1 it may not work (memory allocation problems)
- All Marching Cubes algorithm is inside "MarchGL/MarchGL/src/cubeMarch.cpp"
- Compute shader is inside "MarchGL/MarchGL/res/src" (file name: "computeShaderOriginal.cs")

# How to use

1. Apply desired settings in the "Render Settings" window
2. Introduce implicit formula in the "Implicit Functions" window, and then render 
   - f.e Sphere formula: x\*x + y\*y + z\*z - radius\*radius (replace radius for desired value)

# MarchGL - Algorithm

Iso-Surfaces using Marching Cubes in Modern OpenGL

- All 15 unique possible cases:

![cubes1](images/Pasted%20image%2020230120181503.png)


![cubes2](images/Pasted%20image%2020230122033900.png)

- Black: vertices numbers
- Red: edges numbers

## Example

- Case 1: (only vertice 0 is inside the shape)
	- Decimal
		- Vertice: 0
		- Edges: 048 (lookup in first table)
	- Binary
		- Vertice: 0000 0001
		- Edges: 0000 0000 1100
	  1. Calculate the points of each edge (Linear Interpolation)
	     - Formula: (1-t) * p1 + t * p2, t = -D1/(D2-D1); D -> density on the point
	  2. Add those vertices to a list
	  3. "Organize" the vertices, in a way that the correct triangles are drawn (lookup third table, using the vertice binary)


![Pasted image 20230120181503](images/Screenshot1.png)

![Pasted image 20230120181503](images/Screenshot2.png)
