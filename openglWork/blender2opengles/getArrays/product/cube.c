// This is a .c file for the model: cube

#include "cube.h"

const int cubeVertices = 36;

const float cubePositions[108] = 
{
1, -1, -1, 
1, 1, -0.999999, 
0.999999, 1, 1, 
1, -1, -1, 
0.999999, 1, 1, 
1, -1, 1, 
1, 1, -0.999999, 
1, -1, -1, 
-1, -1, -1, 
1, 1, -0.999999, 
-1, -1, -1, 
-1, 1, -1, 
1, -1, 1, 
0.999999, 1, 1, 
-1, 1, 1, 
1, -1, 1, 
-1, 1, 1, 
-1, -1, 1, 
1, -1, -1, 
1, -1, 1, 
-1, -1, 1, 
1, -1, -1, 
-1, -1, 1, 
-1, -1, -1, 
-1, -1, 1, 
-1, 1, 1, 
-1, 1, -1, 
-1, -1, 1, 
-1, 1, -1, 
-1, -1, -1, 
1, 1, -0.999999, 
-1, 1, -1, 
-1, 1, 1, 
1, 1, -0.999999, 
-1, 1, 1, 
0.999999, 1, 1, 
};

const float cubeTexels[72] = 
{
0.375624, 0.500625, 
0.375625, 0.251875, 
0.624374, 0.251874, 
0.375624, 0.500625, 
0.624374, 0.251874, 
0.624375, 0.500624, 
0.126874, 0.998126, 
0.126874, 0.749375, 
0.375625, 0.749375, 
0.126874, 0.998126, 
0.375625, 0.749375, 
0.375625, 0.998126, 
0.873126, 0.749375, 
0.873126, 0.998126, 
0.624375, 0.998126, 
0.873126, 0.749375, 
0.624375, 0.998126, 
0.624375, 0.749375, 
0.375624, 0.500625, 
0.624375, 0.500624, 
0.624375, 0.749375, 
0.375624, 0.500625, 
0.624375, 0.749375, 
0.375625, 0.749375, 
0.624375, 0.749375, 
0.624375, 0.998126, 
0.375625, 0.998126, 
0.624375, 0.749375, 
0.375625, 0.998126, 
0.375625, 0.749375, 
0.375625, 0.251875, 
0.375624, 0.003126, 
0.624373, 0.003126, 
0.375625, 0.251875, 
0.624373, 0.003126, 
0.624374, 0.251874, 
};

const float cubeNormals[108] = 
{
1, 0, 1e-06, 
1, 0, 1e-06, 
1, 0, 1e-06, 
1, 1e-06, 0, 
1, 1e-06, 0, 
1, 1e-06, 0, 
0, 1e-06, -1, 
0, 1e-06, -1, 
0, 1e-06, -1, 
1e-06, 0, -1, 
1e-06, 0, -1, 
1e-06, 0, -1, 
-0, -1e-06, 1, 
-0, -1e-06, 1, 
-0, -1e-06, 1, 
0, -0, 1, 
0, -0, 1, 
0, -0, 1, 
0, -1, -0, 
0, -1, -0, 
0, -1, -0, 
0, -1, -0, 
0, -1, -0, 
0, -1, -0, 
-1, 0, 0, 
-1, 0, 0, 
-1, 0, 0, 
-1, 0, 0, 
-1, 0, 0, 
-1, 0, 0, 
-0, 1, 0, 
-0, 1, 0, 
-0, 1, 0, 
-0, 1, 0, 
-0, 1, 0, 
-0, 1, 0, 
};

const int cubeMaterials = 6;

const int cubeFirsts[6] = 
{
0,
6,
12,
18,
24,
30,
};

const int cubeCounts[6] = 
{
6,
6,
6,
6,
6,
6,
};

const float cubeDiffuses[6][3] = 
{
0.8, 0, 0.8, 
0.8, 0, 0, 
0, 0, 0.4, 
0, 0.4, 0.4, 
0, 0, 0, 
0, 0, 0, 
};

const float cubeSpeculars[6][3] = 
{
0, 0, 0, 
0, 0, 0, 
1, 1, 1, 
1, 1, 1, 
0, 1, 0, 
1, 1, 0, 
};

