#include "Helper/rtweekend.h"

#include "Bounding_Volume_Hierarchies/bvh.h"
#include "camera.h"
#include "Hittable/hittable_list.h"
#include "scenes.h"

#include <chrono>
#include <windows.h>
using namespace std::chrono;

int main() {
    //Prevent Sleep
    SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_AWAYMODE_REQUIRED);

    //World
    hittable_list world;
    hittable_list lights;
    
    //Camera
    camera cam;

    //Scene
    scene12(world, lights, cam, 1600, 200, 200);

    //Renderer w/ Performance measurement
    auto begin = high_resolution_clock::now();
    cam.render(world, lights, 8, false);
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<seconds>(end - begin);

    std::clog << "Render Time: " << duration.count() << " seconds.\n";

    //Allow Sleep
    SetThreadExecutionState(ES_CONTINUOUS);

    return 0;
}