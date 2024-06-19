#include "rtweekend.h"

#include "bvh.h"
#include "camera.h"
#include "hittable.h"
#include "hittable_list.h"
#include "material.h"
#include "sphere.h"
#include "texture.h"

#include <chrono>
using namespace std::chrono;

void performance(camera& cam, const hittable_list world) {
    //1 Sample
    cam.samples_per_pixel = 1;
    auto begin = high_resolution_clock::now();
    cam.render(world);
    auto end = high_resolution_clock::now();
    auto duration1 = duration_cast<microseconds>(end - begin);

    //10 Sample
    cam.samples_per_pixel = 10;
    begin = high_resolution_clock::now();
    cam.render(world);
    end = high_resolution_clock::now();
    auto duration2 = duration_cast<microseconds>(end - begin);

    //100 Sample
    cam.samples_per_pixel = 100;
    begin = high_resolution_clock::now();
    cam.render(world);
    end = high_resolution_clock::now();
    auto duration3 = duration_cast<microseconds>(end - begin);
    
    
    std::clog << "1 Sample: " << duration1.count() << "\n10 Samples: " << duration2.count() << "\n100 Samples: " << duration3.count() << '\n';
}

void scene1(hittable_list& world, camera& cam) {
    // Materials    
    auto material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.0));
    auto material_center = make_shared<lambertian>(color(0.1, 0.2, 0.5));
    auto material_left   = make_shared<dialectric>(1.5);
    auto material_bubble = make_shared<dialectric>(1.00 / 1.50);
    auto material_right  = make_shared<metal>(color(0.8, 0.6, 0.2), 1.0);
    
    // Objects
    world.add(make_shared<sphere>(point3( 0.0, -100.5, -1.0), 100.0, material_ground));
    world.add(make_shared<sphere>(point3( 0.0,    0.0, -1.2),   0.5, material_center));
    world.add(make_shared<sphere>(point3(-1.0,    0.0, -1.0),   0.5, material_left));
    world.add(make_shared<sphere>(point3(-1.0,   0.0,  -1.0),   0.4, material_bubble));
    world.add(make_shared<sphere>(point3( 1.0,    0.0, -1.0),   0.5, material_right));
    
    // Camera Resolution
    cam.aspect_ratio        = 16.0 / 9.0;
    cam.image_width         = 400;
    cam.samples_per_pixel   = 100;
    cam.max_depth           = 50;

    // Camera Position
    cam.vfov     = 90;
    cam.lookfrom = point3(-2, 2, 1);
    cam.lookat   = point3(0, 0, -1);
    cam.vup      = vec3(0, 1, 0);

    // Depth of Field
    cam.defocus_angle = 0.0;
    cam.focus_dist    = 1.0;

    // Output
    cam.output = "scene1.ppm";
    
    return;
}
void scene2(hittable_list& world, camera& cam) {
    // Variables
    auto R = cos(pi/4);

    // Materials
    auto material_left = make_shared<lambertian>(color(0,0,1));
    auto material_right = make_shared<lambertian>(color(1,0,0));

    // Objects
    world.add(make_shared<sphere>(point3(-R, 0, -1), R, material_left));
    world.add(make_shared<sphere>(point3(R, 0, -1), R, material_right));

    // Camera Resolution
    cam.aspect_ratio        = 16.0 / 9.0;
    cam.image_width         = 400;
    cam.samples_per_pixel   = 100;
    cam.max_depth           = 50;

    // Camera Position
    cam.vfov     = 90;
    cam.lookfrom = point3(-2, 2, 1);
    cam.lookat   = point3(0, 0, -1);
    cam.vup      = vec3(0, 1, 0);

    // Depth of Field
    cam.defocus_angle = 0.0;
    cam.focus_dist    = 10.0;

    // Output
    cam.output = "scene2.ppm";
    
    return;
}

void scene3(hittable_list& world, camera& cam) {
    // Materials
    auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    auto material1 = make_shared<dialectric>(1.5);
    auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
    auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);

    // Objects
    world.add(make_shared<sphere>(point3( 0, -1000, 0), 1000, ground_material));
    world.add(make_shared<sphere>(point3( 0,     1, 0),  1.0, material1));
    world.add(make_shared<sphere>(point3(-4,     1, 0),  1.0, material2));
    world.add(make_shared<sphere>(point3( 4,     1, 0),  1.0, material3));

    for(int a = -11; a < 11; a++) {
        for(int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            point3 center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());

            if((center - point3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<material> sphere_material;

                if(choose_mat < 0.8) {
                    //Diffuse
                    auto albedo = color::random() * color::random();
                    sphere_material = make_shared<lambertian>(albedo);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else if(choose_mat < 0.95) {
                    //Metal
                    auto albedo = color::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else {
                    sphere_material = make_shared<dialectric>(1.5);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    // Camera Resolution
    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 1200;
    cam.samples_per_pixel = 500;
    cam.max_depth         = 50;

    // Camera Position
    cam.vfov     = 20;
    cam.lookfrom = point3(13, 2, 3);
    cam.lookat   = point3(0, 0, 0);
    cam.vup      = vec3(0, 1, 0);

    // Depth of Field
    cam.defocus_angle = 0.6;
    cam.focus_dist    = 10.0;
    
    // Output
    cam.output = "scene3.ppm";
    
    return;
}
void scene3_lite(hittable_list& world, camera& cam) {
    // Materials
    auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    auto material1 = make_shared<dialectric>(1.5);
    auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
    auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);

    // Objects
    world.add(make_shared<sphere>(point3( 0, -1000, 0), 1000, ground_material));
    world.add(make_shared<sphere>(point3( 0,     1, 0),  1.0, material1));
    world.add(make_shared<sphere>(point3(-4,     1, 0),  1.0, material2));
    world.add(make_shared<sphere>(point3( 4,     1, 0),  1.0, material3));

    for(int a = -11; a < 11; a++) {
        for(int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            point3 center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());

            if((center - point3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<material> sphere_material;

                if(choose_mat < 0.8) {
                    //Diffuse
                    auto albedo = color::random() * color::random();
                    sphere_material = make_shared<lambertian>(albedo);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else if(choose_mat < 0.95) {
                    //Metal
                    auto albedo = color::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else {
                    sphere_material = make_shared<dialectric>(1.5);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    // Camera Resolution
    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 1200;
    cam.samples_per_pixel = 9;
    cam.max_depth         = 20;

    // Camera Position
    cam.vfov     = 20;
    cam.lookfrom = point3(13, 2, 3);
    cam.lookat   = point3(0, 0, 0);
    cam.vup      = vec3(0, 1, 0);

    // Depth of Field
    cam.defocus_angle = 0.6;
    cam.focus_dist    = 10.0;
    
    // Output
    cam.output = "scene3_lite.ppm";
    
    return;
}

void scene4(hittable_list& world, camera& cam) {
    // Materials
    auto checker   = make_shared<checker_texture>(0.32, color(.2, .3, .1), color(.9, .9, .9));
    auto material1 = make_shared<dialectric>(1.5);
    auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
    auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);

    // Objects
    world.add(make_shared<sphere>(point3( 0, -1000, 0), 1000, make_shared<lambertian>(checker)));
    world.add(make_shared<sphere>(point3( 0,     1, 0),  1.0, material1));
    world.add(make_shared<sphere>(point3(-4,     1, 0),  1.0, material2));
    world.add(make_shared<sphere>(point3( 4,     1, 0),  1.0, material3));

    for(int a = -11; a < 11; a++) {
        for(int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            point3 center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());

            if((center - point3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<material> sphere_material;

                if(choose_mat < 0.8) {
                    //Diffuse
                    auto albedo = color::random() * color::random();
                    sphere_material = make_shared<lambertian>(albedo);
                    auto center2 = center + vec3(0, random_double(0, .5), 0);
                    world.add(make_shared<sphere>(center, center2, 0.2, sphere_material));
                } else if(choose_mat < 0.95) {
                    //Metal
                    auto albedo = color::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else {
                    sphere_material = make_shared<dialectric>(1.5);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    //Add BVH
    world = hittable_list(make_shared<bvh_node>(world));

    // Camera Resolution
    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth         = 50;

    // Camera Position
    cam.vfov     = 20;
    cam.lookfrom = point3(13, 2, 3);
    cam.lookat   = point3(0, 0, 0);
    cam.vup      = vec3(0, 1, 0);

    // Depth of Field
    cam.defocus_angle = 0.6;
    cam.focus_dist    = 10.0;
    
    // Output
    cam.output = "scene4.ppm";

    return;
}

void scene5(hittable_list& world, camera& cam) {
    // Materials
    auto checker = make_shared<checker_texture>(0.32, color(.2, .3, .1), color(.9, .9, .9));

    // Objects
    world.add(make_shared<sphere>(point3(0, -10, 0), 10, make_shared<lambertian>(checker)));
    world.add(make_shared<sphere>(point3(0,  10, 0), 10, make_shared<lambertian>(checker)));

    // Camera Resolution
    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth         = 50;

    // Camera Position
    cam.vfov     = 20;
    cam.lookfrom = point3(13, 2, 3);
    cam.lookat   = point3(0, 0, 0);
    cam.vup      = vec3(0, 1, 0);

    // Depth of Field
    cam.defocus_angle = 0.0;

    // FileName
    cam.output = "scene5.ppm";
    
    return;
}

void scene6(hittable_list& world, camera& cam) {
    // Textures
    auto earth_texture = make_shared<image_texture>("earthmap.jpg");

    // Materials
    auto earth_surface = make_shared<lambertian>(earth_texture);
    
    // Objects
    auto globe = make_shared<sphere>(point3(0,0,0), 2, earth_surface);
    world.add(globe);

    // Camera Resolution
    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth         = 50;

    // Camera Position
    cam.vfov     = 20;
    cam.lookfrom = point3(0, 0, 12);
    cam.lookat   = point3(0, 0, 0);
    cam.vup      = vec3(0, 1, 0);

    // Depth of Field
    cam.defocus_angle = 0.0;

    // FileName
    cam.output = "scene6.ppm";
    
    return;
}

void scene7(hittable_list& world, camera& cam) {
    // Materials
    auto pertext = make_shared<noise_texture>(4);

    // Objects
    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(pertext)));
    world.add(make_shared<sphere>(point3(0, 2, 0), 2, make_shared<lambertian>(pertext)));

    // Camera Resolution
    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth         = 50;

    // Camera Position
    cam.vfov     = 20;
    cam.lookfrom = point3(13, 2, 3);
    cam.lookat   = point3(0, 0, 0);
    cam.vup      = vec3(0, 1, 0);

    // Depth of Field
    cam.defocus_angle = 0.0;

    // FileName
    cam.output = "scene7.ppm";
    
    return;
}

int main() {
    //World
    hittable_list world;
    
    //Camera
    camera cam;

    //Scene
    scene7(world, cam);

    //Renderer w/ Performance measurement
    auto begin = high_resolution_clock::now();
    cam.render(world, 8);
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - begin);

    std::clog << "Render Time: " << (duration.count()/1000000) << " seconds.\n";

    return 0;
}