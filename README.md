# SphericalWaves
## 3D discrete physics sim of radial waves on a spherical suface

![Screen shot](SphericalHarmonics1-anim.png)

## What does it do?
Simulates radial waves on a spherical surface.  The acceleration of each voxel is based on the radial displacement of its neighbors.

The challenge is linking the voxels together into a seamless mesh with some way to identify the neighbors of each.  After several different approaches, I went with a simpler but more computationally expensive method of using bounding boxes and intersections to generate the neighbors of each voxel once upon initialization.  (So you may notice a short delay when the program starts, since this is an O(n^2) operation, but it happens only once.)

**Press P** to introduce a disturbance on the surface.  Hold P to build up a bigger wave.  Think of it like pinching the surface of a water balloon and pulling it outward with your finger.  The camera is in free-look mode so you can click the mouse wheel and use cntrl and alt to pan, zoom, rotate, etc.

**Press D** to enter "diagnostic" mode if you want to see the neighbors of each voxel.  The sphere itself will be faded, one voxel will be white, and its neighbors gray.  Pressing C,V,B,or N will change the focus to different voxels in the sphere.  C goes back by 10, V by 1, B forward by 1, N forward by 10.  **Press F** to turn diagnostic mode off.  Useful for tweaking or verifying the neighbor-linking algorithm.

## Options to change
The global variable `unsigned int circSteps` at the top determines how many voxels comprise the surface of the sphere.  Specifically, it sets the number of cubes around the equator of the sphere.  The total number of cubes in the sphere is proportionate to and less than circSteps^2/2.  If the real-time animation is slow, reduce this value.  A value around 100 gives passable results on basic video cards -- experiment and go up from there.

The scale factor in the neighbor-finding algorithm, set here:
```
bbox1.min=sphereSurfaceCubes[diagidx].vectorPos-sizer/1.5;
bbox1.max=sphereSurfaceCubes[diagidx].vectorPos+sizer/1.5;
```
and here:
```
bbox2.min=sphereSurfaceCubes[j].vectorPos-sizer/1.5;
bbox2.max=sphereSurfaceCubes[j].vectorPos+sizer/1.5;
```

Use diagnotic mode to see the results of different figures for the final number.  Between 6 and 8 neighbors per voxel seems to yield good, consistent results for natural wave propagation through the surface, and the scale factor of 1.5 at the end of the above lines achieves this.  If the factor was 2.0, the bounding box would be the same size as the voxel cubes themeselves.  While the cubes do overlap to some degree, there is usually not enough overlap for good linkage over the whole sphere at `sizer/2.0`.

Right before the drawing loop, the key factors affecting acceleration are here:
```
//if ( abs(rdiff)<0.0001 ) rdiff=0.0;
sphereSurfaceCubes[i].rvel+=rdiff;
//sphereSurfaceCubes[i].rvel*=0.9999; //Damping
```
Uncomment the third line for damping: the oscillations will ultimately subside.  With this option, uncommenting the first line may be useful aesthetically to reduce the "shimmering" or "quivering" effect as the sphere quiets down by canceling the propagation of really small motions.

The middle line determines the acceleration.  A scale factor here makes a big difference in the propagation of the wave.  To dampen it, for example, try:
```
sphereSurfaceCubes[i].rvel+=rdiff/40.0;
```

## Why did I make this?
I was teaching my daughter about electron orbitals for her chemistry class.  While I'd ultimately like to demonstrate Spherical Harmonics on it (thus the name of the repo) just getting good wave function working was a significant enough first hurdle.

## Thanks to RayLib
Once again, for making the graphics coding easy.
