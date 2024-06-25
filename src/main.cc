#include "Helper/rtweekend.h"

#include "Bounding_Volume_Hierarchies/bvh.h"
#include "camera.h"
#include "Hittable/hittable_list.h"
#include "scenes.h"

#include <chrono>
using namespace std::chrono;

int main() {
    //World
    hittable_list world;
    
    //Camera
    camera cam;

    //Scene
    scene6(world, cam);

    //Renderer w/ Performance measurement
    auto begin = high_resolution_clock::now();
    cam.render(world, 8);
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - begin);

    std::clog << "Render Time: " << (duration.count()/1000000) << " seconds.\n";

    return 0;
}