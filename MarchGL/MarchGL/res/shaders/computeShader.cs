#version 450




struct TRIANGLES
{
    vec4 p[12];
};





layout(std430, binding = 1) buffer Input2
{
    int edgeTable[256] ;
}
;


layout(std430, binding = 2) buffer Input3
{
    int triTable[256]
    [16];
}
;


layout(std140, binding = 3) buffer Output
{
    TRIANGLES allTriangles[40][40][60]; //change after !!!!!!!!!!!!!!!!!!
}
;

layout(std140, binding = 4) buffer OutputNormals
{
    TRIANGLES allNormals [40]
    [40]
    [60]; //change after !!!!!!!!!!!!!!!!!!
}
;



//float dist = 1.0f;
//float radius = 1.0f;
//int obj = 0;

//uniform float radius;
uniform float dist;
//uniform int obj; //0-sphere 1-torus

//lim of the x, y, and z axis
uniform float x_size;
uniform float y_size;
uniform float z_size;

layout(local_size_x = 10, local_size_y = 10, local_size_z = 10) in;


vec4 getIntersVertice(vec4 p1, vec4 p2, float D1, float D2)
{

    if (abs(D1) < 0.00001)
        return p1;

    if (abs(D2) < 0.00001)
        return p2;

    if (abs(D1 - D2) < 0.00001)
        return p1;
    

    float t = -D1 / (D2 - D1);

    //vec4 tmp = (1 - t) * p1 + t * p2;
    //return (p1 + p2) / 2.0f;
    //return p2;
    
    return (1 - t) * p1 + t * p2;
}

float getDensity(float x, float y, float z)
{
    //if (obj == 0) return p.x * p.x + p.y * p.y + p.z * p.z - radius * radius;
    //else return ((sqrt(p.x * p.x + p.y * p.y) - radius) * (sqrt(p.x * p.x + p.y * p.y) - radius) + p.z * p.z - (radius / 2) * (radius / 2));
    return x*x+y*y+z*z-1;
}

void main()
{

   

    uint index_x = gl_GlobalInvocationID.x;
    uint index_y = gl_GlobalInvocationID.y;
    uint index_z = gl_GlobalInvocationID.z;

    //int bin = 0b00000000;
    int bin[] = { 0, 0, 0, 0, 0, 0, 0, 0 };

    //8 points of the vertex
    vec4 p[8];
    p[0] = vec4(index_x * dist - x_size, index_y * dist - y_size, index_z * dist - z_size, 1.0f);
    //p[0] = voxels[index_x][index_y][index_z];
    p[1] = p[0] + vec4(dist, 0.0f, 0.0f, 0.0f);
    p[2] = p[0] + vec4(dist, dist, 0.0f, 0.0f);
    p[3] = p[0] + vec4(0.0f, dist, 0.0f, 0.0f);
    p[4] = p[0] + vec4(0.0f, 0.0f, dist, 0.0f);
    p[5] = p[0] + vec4(dist, 0.0f, dist, 0.0f);
    p[6] = p[0] + vec4(dist, dist, dist, 0.0f);
    p[7] = p[0] + vec4(0.0f, dist, dist, 0.0f);

 

    //densities
    float d[8];
    for (int i = 0; i < 8; i++)
    {
        //d[i] = p[i].x * p[i].x + p[i].y * p[i].y + p[i].z * p[i].z - radius*radius;
        d[i] = getDensity(p[i].x, p[i].y, p[i].z);
        if (d[i] < 0.0f) //check if is inside the sphere
        {
            bin[7 - i] = 1;
        }
    }


    //convert to int
    int bin_int = 0;
    int allBin[] = { 128, 64, 32, 16, 8, 4, 2, 1 };

    //0011 0010
    for (int i = 7; i >= 0; i--)
    {
        bin_int += bin[i] * allBin[i];
    }


    int edgeFlag = edgeTable[bin_int];
    vec4 edgeVertices[12];
    /*
    for (int i = 0; i < 12; i++)
    {
        //edgeVertices[i] = vec4(0.0f, 0.0f, 0.0f, 0.0f);
        allTriangles[index_x][index_y][index_z].p[i] = vec4(0.0f, 0.0f, 0.0f, 0.0f);
    }
    */
   
    if (edgeFlag != 0)
    {
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

        //index if the work groups were 1d
        //uint def_index = index_x * (gl_NumWorkGroups.y * gl_NumWorkGroups.z) + index_y * gl_NumWorkGroups.z + index_z;

        //def_index = (index_z - 1) * gl_NumWorkGroups.x * gl_NumWorkGroups.y+ (index_x - 1) * gl_NumWorkGroups.y+ index_y;
        
        for (int n = 0; triTable[bin_int][n] != -1; n += 3)
        {
            allTriangles[index_x][index_y][index_z].p[n] = edgeVertices[triTable[bin_int][n]];
            allTriangles[index_x][index_y][index_z].p[n + 1] = edgeVertices[triTable[bin_int][n + 1]];
            allTriangles[index_x][index_y][index_z].p[n + 2] = edgeVertices[triTable[bin_int][n + 2]];


            //normalization
            vec3 a = vec3(allTriangles[index_x][index_y][index_z].p[n].x, allTriangles[index_x][index_y][index_z].p[n].y, allTriangles[index_x][index_y][index_z].p[n].z);
            vec3 b = vec3(allTriangles[index_x][index_y][index_z].p[n + 1].x, allTriangles[index_x][index_y][index_z].p[n + 1].y, allTriangles[index_x][index_y][index_z].p[n + 1].z);
            vec3 c = vec3(allTriangles[index_x][index_y][index_z].p[n + 2].x, allTriangles[index_x][index_y][index_z].p[n + 2].y, allTriangles[index_x][index_y][index_z].p[n + 2].z);


            vec3 normal = -normalize(cross(b-a,c-a));
            
            allNormals[index_x][index_y][index_z].p[n] = vec4(normal, 0.0f);
            allNormals[index_x][index_y][index_z].p[n + 1] = vec4(normal, 0.0f);
            allNormals[index_x][index_y][index_z].p[n + 2] = vec4(normal, 0.0f);


        }
        
    }









}























































































