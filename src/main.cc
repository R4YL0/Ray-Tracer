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
    hittable_list lights;
    
    //Camera
    camera cam;

    //Scene
    scene11(world, lights, cam, 800, 200, 50);

    //Renderer w/ Performance measurement
    auto begin = high_resolution_clock::now();
    cam.render(world, lights, 8);
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<seconds>(end - begin);

    std::clog << "Render Time: " << duration.count() << " seconds.\n";

    return 0;
}