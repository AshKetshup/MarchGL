# MarchGL
Iso-Surfaces using Marching Cubes in Modern OpenGL



- All 15 unique possible cases:

![Pasted image 20230120181503](C:\Users\nunom\Documents\MEGAsync\CIV\CIV-2\Pasted image 20230120181503.png)

![Pasted image 20230122033900](C:\Users\nunom\Documents\MEGAsync\CIV\CIV-2\Pasted image 20230122033900.png)

- Black: vertices numbers
- Red: edges numbers

## Example

- Case 1: (only one vertice is inside the shape)
	- Decimal
		- Vertice: 0
		- Edges: 048 (first table)
	- Binary
		- Vertice: 0000 0001
		- Edges: 0000 0000 1100
	  1. Calculate the middle points of each edge
	  2. Add does vertices to a list
	  3. "Organize" the vertices, in a way that the correct triangles are drawn (third table)



# Left to do

- Test for other functions
- Accelerate the process
  - One way might be to create threads that process each vortex separately, while keeping the correct order of the triangles vertices
- Make the "advanced cube marching"
  - Instead of using the edge middle point, calculate a more approximate point using the isovalues
