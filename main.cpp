#include <iostream>
#include <cmath>
#include "raylib.h"
#include "RaylibOpOverloads.hpp"
#include <vector>


using namespace std;

unsigned int circSteps=240; //Determines fineness of voxels.  There are circSteps cubes over theta=[0,2PI) around the equator of the sphere, and circSteps/2 cubes over phi=[0,PI)

//typedef Vector3 Polar3;
struct Polar3 {
    float r;
    float th;
    float phi;
};

struct SCube {
    Polar3 polarPos;
    Vector3 vectorPos;
    unsigned int idx;
    vector<unsigned int> neighbors;
    float rvel;
};

Vector3 SphericalToCartesian(Polar3 p) {
    Vector3 result;
    result.y=p.r*cos(p.phi);
    result.x=p.r*sin(p.phi)*cos(p.th);
    result.z=p.r*sin(p.phi)*sin(p.th);
return result;
}

Polar3 CartesianToSpherical(Vector3 v) {
    Polar3 result;
    result.r=Vector3Length(v);
    result.phi = acos(v.y/result.r);
    result.th = atan2(v.z,v.x);
return result;
}

int main()
{
    vector<SCube> sphereSurfaceCubes;
    SCube c;
    vector<unsigned int> nbs;

    //Initialize vector to contain a sphere, the surface of which is small cubes
    //Theta goes from 0 to 2PI in the x-z plane.  Phi goes from 0 to PI in the X-Y plane.

    float sphereSpacing=2*PI/circSteps;
    float sphereRadius=100.0;
    unsigned int counter=0;
    //Do North and South poles by hand
    c.polarPos=(Polar3){sphereRadius,0,0};
    c.vectorPos=SphericalToCartesian(c.polarPos);
    c.rvel=0.0;
    c.idx=0;
    sphereSurfaceCubes.push_back(c);
    for (unsigned int k=0;k<circSteps/2;k++ ) {
        //cout<<"For k="<<k<<": "<<circSteps*sin(2*PI*k/circSteps)<<endl;
        for (unsigned int j=0;j<circSteps*sin(2*PI*k/circSteps);j++) {
            c.polarPos=(Polar3) {sphereRadius,j*sphereSpacing/sin(2*PI*k/circSteps),k*sphereSpacing};
            c.vectorPos=SphericalToCartesian(c.polarPos);
            c.idx=counter;
            c.rvel=0.0;
            sphereSurfaceCubes.push_back(c);
        }
    }
    //Do South pole
    c.polarPos=(Polar3){sphereRadius,0,90};
    c.vectorPos=SphericalToCartesian(c.polarPos);
    c.rvel=0.0;
    sphereSurfaceCubes.push_back(c);

    cout<<"Number of Surface Cubes="<<sphereSurfaceCubes.size()<<endl;

    float rdiff=0.0;
    vector<SCube> hits;
    bool diag=false;
    unsigned int diagidx=0;

    BoundingBox bbox1,bbox2;
    Vector3 sizer=(Vector3) {sphereRadius*sphereSpacing,sphereRadius*sphereSpacing,sphereRadius*sphereSpacing};

    unsigned int startidx,endidx,loopstocheck;
    loopstocheck=5;

    for (diagidx=0;diagidx<sphereSurfaceCubes.size();diagidx++) {
        bbox1.min=sphereSurfaceCubes[diagidx].vectorPos-sizer/1.5;
        bbox1.max=sphereSurfaceCubes[diagidx].vectorPos+sizer/1.5;
        sphereSurfaceCubes[diagidx].neighbors.clear();
        //Expedite neighbor-finding compared to comparing very cube with every other, by only checking 5 theta loops higher and lower
        if (diagidx>loopstocheck*circSteps) startidx=diagidx-loopstocheck*circSteps; else startidx=0;
        if (diagidx+loopstocheck*circSteps>sphereSurfaceCubes.size()) endidx=sphereSurfaceCubes.size(); else endidx=diagidx+loopstocheck*circSteps;
        for (unsigned int j=startidx;j<endidx;j++) {
        //for (unsigned int j=0;j<sphereSurfaceCubes.size();j++) {
            bbox2.min=sphereSurfaceCubes[j].vectorPos-sizer/1.5;
            bbox2.max=sphereSurfaceCubes[j].vectorPos+sizer/1.5;
            if (j!=diagidx) {
                if (CheckCollisionBoxes(bbox1,bbox2)) {
                    sphereSurfaceCubes[diagidx].neighbors.push_back(j);
                }
            }
        }
        if (sphereSurfaceCubes[diagidx].neighbors.size()==0) cout<<"Cube # "<<diagidx<<" has "<<sphereSurfaceCubes[diagidx].neighbors.size()<<"neighbors"<<endl;
    }
    diagidx=0;

    //Initialize RayLib after the above to avoid a big lag on first frame
    const int screenWidth = 1200;
    const int screenHeight = 900;

    InitWindow(screenWidth, screenHeight, "");
    SetTargetFPS(30);

    Camera3D camera = { 0 };
    camera.position = (Vector3){ 0.0f, 0.0f, 300.0f };  // Camera position
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };      // Camera looking at point
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 45.0f;                                // Camera field-of-view Y
    camera.type = CAMERA_PERSPECTIVE;                   // Camera mode type
    SetCameraMode(camera, CAMERA_FREE);

while (!WindowShouldClose())    // Detect window close button or ESC key
    {
    //Update
    UpdateCamera(&camera);
    if (IsKeyDown(KEY_P)) {
            sphereSurfaceCubes[4000].polarPos.r=sphereRadius+30; //Pluck one up
            sphereSurfaceCubes[4000].vectorPos=SphericalToCartesian(sphereSurfaceCubes[4000].polarPos);
    }

    if (IsKeyDown(KEY_D)) diag=true;
    if (IsKeyDown(KEY_F)) diag=false;

    if (diag) {
        if (IsKeyDown(KEY_V)) {
            if (diagidx!=0) diagidx--;
        }
        if (IsKeyDown(KEY_B)) {
            diagidx++;
            if (diagidx>sphereSurfaceCubes.size()-1) diagidx=sphereSurfaceCubes.size()-1;
        }
        if (IsKeyDown(KEY_C)) {
            if (diagidx>10) diagidx=-10;
        }
        if (IsKeyDown(KEY_N)) {
            diagidx+=10;
            if (diagidx>sphereSurfaceCubes.size()-1) diagidx=sphereSurfaceCubes.size()-1;
        }
        cout<<"Showing cube #"<<diagidx<<" neighbors: "<<sphereSurfaceCubes[diagidx].neighbors.size()<<endl;
    }

    for (unsigned int i=0;i<sphereSurfaceCubes.size();i++) {
        nbs=sphereSurfaceCubes[i].neighbors;
        rdiff=0.0;
        for (unsigned int n=0;n<nbs.size();n++) {
            rdiff+=sphereSurfaceCubes[nbs[n]].polarPos.r-sphereSurfaceCubes[i].polarPos.r;
        }
        rdiff/=(float)nbs.size();
        //if ( abs(rdiff)<0.0001 ) rdiff=0.0;
        sphereSurfaceCubes[i].rvel+=rdiff;
        //sphereSurfaceCubes[i].rvel*=0.9999; //Damping
    }

    for (unsigned int i=0;i<sphereSurfaceCubes.size();i++) {
        sphereSurfaceCubes[i].polarPos.r+=sphereSurfaceCubes[i].rvel;
        sphereSurfaceCubes[i].vectorPos=SphericalToCartesian(sphereSurfaceCubes[i].polarPos);
    }


    //Draw
    BeginDrawing();
    ClearBackground(BLACK);

    BeginMode3D(camera);

    DrawCube({0,0,00},1,1,400,ORANGE);
    DrawCube({0,0,00},400,1,1,GREEN);
    DrawCube({0,0,00},1,400,1,PINK);

    DrawGrid(10,1);
    DrawCube({0,0,0},2,2,2,BROWN);

    for (unsigned int i=0;i<sphereSurfaceCubes.size();i++) {
        DrawCube(sphereSurfaceCubes[i].vectorPos,sphereRadius*sphereSpacing,sphereRadius*sphereSpacing,sphereRadius*sphereSpacing,Fade(RED,diag?0.4:0.8));
        DrawCubeWires(sphereSurfaceCubes[i].vectorPos,sphereRadius*sphereSpacing,sphereRadius*sphereSpacing,sphereRadius*sphereSpacing,BLACK);
    }

    if (diag) {
        DrawCube(sphereSurfaceCubes[diagidx].vectorPos,sphereRadius*sphereSpacing,sphereRadius*sphereSpacing,sphereRadius*sphereSpacing,WHITE);
        for (unsigned int k=0;k<sphereSurfaceCubes[diagidx].neighbors.size();k++) {
            DrawCube(sphereSurfaceCubes[sphereSurfaceCubes[diagidx].neighbors[k]].vectorPos,sphereRadius*sphereSpacing,sphereRadius*sphereSpacing,sphereRadius*sphereSpacing,Fade(LIME,0.5));
        }
    }

    EndMode3D();

    //DrawFPS(5,5);
    EndDrawing();
    }


    CloseWindow();
    return 0;
}

/*
    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) ) {
        ray=GetMouseRay(GetMousePosition(),camera);
        Vector3 upperleft,lowerright,v;
        Vector3 sizer=(Vector3) {sphereRadius*sphereSpacing/2,sphereRadius*sphereSpacing/2,sphereRadius*sphereSpacing/2};
        BoundingBox bbox;
        hits.clear();
        for (unsigned int i=0; i<sphereSurfaceCubes.size(); i++ ) {
            v=SphericalToCartesian(sphereSurfaceCubes[i].polarPos);
            upperleft=v-sizer;
            lowerright=v+sizer;
            bbox.min=upperleft;
            bbox.max=lowerright;
            if (CheckCollisionRayBox(ray,bbox) ) {
                drawhits=true;
                hits.push_back(sphereSurfaceCubes[i]);
                for (unsigned int n=0;n<sphereSurfaceCubes[i].neighbors.size();n++) {
                    hits.push_back(sphereSurfaceCubes[sphereSurfaceCubes[i].neighbors[n]]);
                }
            }
        }
    }
*/
