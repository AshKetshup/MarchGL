#version 450





layout(std430, binding = 0) buffer Input
{
    vec4 voxels [] ; //point 0 of the VOXEL
}
;

layout(std430, binding = 4) buffer Output
{
    vec4 edges [125][12] ;
}
;

layout(std430, binding = 1) buffer Output2
{
    int edge_case[] ;
}
;


layout(std430, binding = 2) buffer Input2
{
    int edgeTable[256] ;
}
;

layout(std430, binding = 3) buffer Input3
{
    int triTable[256][16] ;
}
;


layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;


vec4 getIntersVertice(vec4 p1, vec4 p2, float D1, float D2)
{
	float dist = 0.5f;

    if (abs(D1) < 0.00001)
        return p1;

    if (abs(D2) < 0.00001)
        return p2;

    if (abs(D1 - D2) < 0.00001)
        return p1;

    float t = -D1 / (D2 - D1);

    //vec4 tmp = (1 - t) * p1 + t * p2;
    //return (p1 + p2) / 2.0f;
    return (1 - t) * p1 + t * p2;
}

void main()
{

   


    float dist = 0.5f;
    uint index = gl_GlobalInvocationID.x;
    //int bin = 0b00000000;
	int bin[] = { 0,0,0,0,0,0,0,0 }; 

    //8 points of the vertex
    vec4 p[8]; 
    p[0] = voxels[index];
    p[1] = p[0] + vec4(dist, 0.0f, 0.0f, 1.0f);
    p[2] = p[0] + vec4(dist, dist, 0.0f, 1.0f);
    p[3] = p[0] + vec4(0.0f, dist, 0.0f, 1.0f);
    p[4] = p[0] + vec4(0.0f, 0.0f, dist, 1.0f);
    p[5] = p[0] + vec4(dist, 0.0f, dist, 1.0f);
    p[6] = p[0] + vec4(dist, dist, dist, 1.0f);
    p[7] = p[0] + vec4(0.0f, dist, dist, 1.0f);

	//densities
	float d[8];
	for(int i = 0; i < 8; i++)
	{
        d[i] = p[i].x * p[i].x + p[i].y * p[i].y + p[i].z * p[i].z - 1;
        if (d[i] < 0.0f) //check if is inside the sphere
        {
			bin[7-i] = 1;
        }
    }


	//convert to int
	int bin_int = 0;
	int allBin[] = { 128, 64, 32, 16, 8, 4, 2, 1 };

    //0011 0010
	for(int i=7; i >= 0; i--)
	{
		bin_int += bin[i] * allBin[i];
	}

    if(bin_int == 0)
    {
        edge_case[index] = -1;
    }
    else
    {
        int edgeFlag = edgeTable[bin_int];

        vec4 edgeVertices[12];
        for(int i = 0; i < 12; i++)
        {
            edgeVertices[i] = vec4(0.0f, 0.0f, 0.0f, 0.0f);
        }
        if ((edgeFlag & 1) != 0) edgeVertices[0] = getIntersVertice(p[0], p[1], d[0], d[1]); //edge 0
        if ((edgeFlag & 2) != 0) edgeVertices[1] = getIntersVertice(p[1], p[2], d[1], d[2]); //edge 1
        if ((edgeFlag & 4) != 0) edgeVertices[2] = getIntersVertice(p[2], p[3], d[2], d[3]); //edge 2
        if ((edgeFlag & 8) != 0) edgeVertices[3] = getIntersVertice(p[3], p[0], d[3], d[0]); //edge 3
        if ((edgeFlag & 16) != 0) edgeVertices[4] = getIntersVertice(p[4], p[5], d[4], d[5]); //edge 4
        if ((edgeFlag & 32) != 0) edgeVertices[5] = getIntersVertice(p[5], p[6], d[5], d[6]); //edge 5
        if ((edgeFlag & 64) != 0) edgeVertices[6] = getIntersVertice(p[6], p[7], d[6], d[7]); //edge 6
        if ((edgeFlag & 128) != 0) edgeVertices[7] = getIntersVertice(p[7], p[4], d[7], d[4]); //edge 7
        if ((edgeFlag & 256) != 0) edgeVertices[8] = getIntersVertice(p[0], p[4], d[0], d[4]); //edge 8
        if ((edgeFlag & 512) != 0) edgeVertices[9] = getIntersVertice(p[1], p[5], d[1], d[5]); //edge 9
        if ((edgeFlag & 1024) != 0) edgeVertices[10] = getIntersVertice(p[2], p[6], d[2], d[6]); //edge 10
        if ((edgeFlag & 2048) != 0) edgeVertices[11] = getIntersVertice(p[3], p[7], d[3], d[7]); //edge 11


        edge_case[index] = bin_int;
        for (int i = 0; i < 12; i++)
        {
            edges[index][i] = edgeVertices[i];
        }
       
    }

    
    

    
}