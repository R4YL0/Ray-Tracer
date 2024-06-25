#ifndef SCENES_H
#define SCENES_H

#include "Helper\rtweekend.h"

#include "Bounding_Volume_Hierarchies/bvh.h"
#include "camera.h"
#include "Materials/constant_medium.h"
#include "Hittable/hittable.h"
#include "Hittable/hittable_list.h"
#include "Materials/material.h"
#include "Hittable/sphere.h"
#include "Hittable/surface.h"
#include "Materials/texture.h"

void scene1(hittable_list& world, camera& cam, int image_width = 400, int samples_per_pixel = 100, int max_depth = 50) {
    // Materials Scene
    // Materials    
    auto material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.0));
    auto material_center = make_shared<lambertian>(color(0.1, 0.2, 0.5));
    auto material_left   = make_shared<dielectric>(1.5);
    auto material_bubble = make_shared<dielectric>(1.00 / 1.50);
    auto material_right  = make_shared<metal>(color(0.8, 0.6, 0.2), 1.0);
    
    // Objects
    world.add(make_shared<sphere>(point3( 0.0, -100.5, -1.0), 100.0, material_ground));
    world.add(make_shared<sphere>(point3( 0.0,    0.0, -1.2),   0.5, material_center));
    world.add(make_shared<sphere>(point3(-1.0,    0.0, -1.0),   0.5, material_left));
    world.add(make_shared<sphere>(point3(-1.0,   0.0,  -1.0),   0.4, material_bubble));
    world.add(make_shared<sphere>(point3( 1.0,    0.0, -1.0),   0.5, material_right));
    
    // Camera Resolution
    cam.aspect_ratio        = 16.0 / 9.0;
    cam.image_width         = image_width;
    cam.samples_per_pixel   = samples_per_pixel;
    cam.max_depth           = max_depth;
    cam.background        = color(0.70, 0.80, 1.00);

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
void scene2(hittable_list& world, camera& cam, int image_width = 400, int samples_per_pixel = 100, int max_depth = 50) {
    // Light-bleeding Scene
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
    cam.image_width         = image_width;
    cam.samples_per_pixel   = samples_per_pixel;
    cam.max_depth           = max_depth;
    cam.background          = color(0.70, 0.80, 1.00);

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

void scene3(hittable_list& world, camera& cam, int image_width = 1200, int samples_per_pixel = 500, int max_depth = 50) {
    // InAWeekend Scene
    // Materials
    auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    auto material1 = make_shared<dielectric>(1.5);
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
                    sphere_material = make_shared<dielectric>(1.5);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    // Camera Resolution
    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = image_width;
    cam.samples_per_pixel = samples_per_pixel; // 9 for Lite
    cam.max_depth         = max_depth; // 20 for Lite
    cam.background        = color(0.70, 0.80, 1.00);

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

void scene4(hittable_list& world, camera& cam, int image_width = 400, int samples_per_pixel = 100, int max_depth = 50) {
    // Moving Scene and BVH
    // Materials
    auto checker   = make_shared<checker_texture>(0.32, color(.2, .3, .1), color(.9, .9, .9));
    auto material1 = make_shared<dielectric>(1.5);
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
                    sphere_material = make_shared<dielectric>(1.5);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    //Add BVH
    world = hittable_list(make_shared<bvh_node>(world));

    // Camera Resolution
    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = image_width;
    cam.samples_per_pixel = samples_per_pixel;
    cam.max_depth         = max_depth;
    cam.background        = color(0.70, 0.80, 1.00);

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

void scene5(hittable_list& world, camera& cam, int image_width = 400, int samples_per_pixel = 100, int max_depth = 50) {
    // Checkerboard Scene
    // Materials
    auto checker = make_shared<checker_texture>(0.32, color(.2, .3, .1), color(.9, .9, .9));

    // Objects
    world.add(make_shared<sphere>(point3(0, -10, 0), 10, make_shared<lambertian>(checker)));
    world.add(make_shared<sphere>(point3(0,  10, 0), 10, make_shared<lambertian>(checker)));

    // Camera Resolution
    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = image_width;
    cam.samples_per_pixel = samples_per_pixel;
    cam.max_depth         = max_depth;
    cam.background        = color(0.70, 0.80, 1.00);

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

void scene6(hittable_list& world, camera& cam, int image_width = 400, int samples_per_pixel = 100, int max_depth = 50) {
    // Textures Scene
    // Textures
    auto earth_texture = make_shared<image_texture>("earthmap.jpg");

    // Materials
    auto earth_surface = make_shared<lambertian>(earth_texture);
    
    // Objects
    shared_ptr<hittable> globe = make_shared<sphere>(point3(0,0,50), 2, earth_surface);
    globe = make_shared<rotate_x>(globe, 45);
    globe = make_shared<rotate_y>(globe, 45);
    globe = make_shared<rotate_z>(globe, 45);
    world.add(globe);

    // Camera Resolution
    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = image_width;
    cam.samples_per_pixel = samples_per_pixel;
    cam.max_depth         = max_depth;
    cam.background        = color(0.70, 0.80, 1.00);

    // Camera Position
    cam.vfov     = 20;
    cam.lookfrom = point3(0, 0, 62);
    cam.lookat   = point3(0, 0, 50);
    cam.vup      = vec3(0, 1, 0);

    // Depth of Field
    cam.defocus_angle = 0.0;

    // FileName
    cam.output = "scene6.ppm";
    
    return;
}

void scene7(hittable_list& world, camera& cam, int image_width = 400, int samples_per_pixel = 100, int max_depth = 50) {
    // Perlin-Noise Scene
    // Materials
    auto pertext = make_shared<noise_texture>(4);

    // Objects
    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(pertext)));
    world.add(make_shared<sphere>(point3(0, 2, 0), 2, make_shared<lambertian>(pertext)));

    // Camera Resolution
    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = image_width;
    cam.samples_per_pixel = samples_per_pixel;
    cam.max_depth         = max_depth;
    cam.background        = color(0.70, 0.80, 1.00);

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

void scene8(hittable_list& world, camera& cam, int image_width = 400, int samples_per_pixel = 100, int max_depth = 50) {
    // Quad Scene
    // Materials
    auto left_red     = make_shared<lambertian>(color(1.0, 0.2, 0.2));
    auto back_green   = make_shared<lambertian>(color(0.2, 1.0, 0.2));
    auto right_blue   = make_shared<lambertian>(color(0.2, 0.2, 1.0));
    auto upper_orange = make_shared<lambertian>(color(1.0, 0.5, 0.0));
    auto lower_teal   = make_shared<lambertian>(color(0.2, 0.8, 0.8));

    //Objects: Quads
    world.add(make_shared<quad>(point3(-3,-2, 5), vec3(0, 0,-4), vec3(0, 4, 0), left_red));
    world.add(make_shared<quad>(point3(-2,-2, 0), vec3(4, 0, 0), vec3(0, 4, 0), back_green));
    world.add(make_shared<quad>(point3( 3,-2, 1), vec3(0, 0, 4), vec3(0, 4, 0), right_blue));
    world.add(make_shared<quad>(point3(-2, 3, 1), vec3(4, 0, 0), vec3(0, 0, 4), upper_orange));
    world.add(make_shared<quad>(point3(-2,-3, 5), vec3(4, 0, 0), vec3(0, 0,-4), lower_teal));

    // Camera Resolution
    cam.aspect_ratio      = 1.0;
    cam.image_width       = image_width;
    cam.samples_per_pixel = samples_per_pixel;
    cam.max_depth         = max_depth;
    cam.background        = color(0.70, 0.80, 1.00);

    // Camera Position
    cam.vfov     = 80;
    cam.lookfrom = point3(0, 0, 9);
    cam.lookat   = point3(0, 0, 0);
    cam.vup      = vec3(0, 1, 0);

    // Depth of Field
    cam.defocus_angle = 0.0;

    // FileName
    cam.output = "scene8.ppm";
    
    return;
}

void scene9(hittable_list& world, camera& cam, int image_width = 400, int samples_per_pixel = 100, int max_depth = 50) {
    // Diffuse-Light Scene
    // Materials
    auto pertext = make_shared<noise_texture>(4);
    auto difflight = make_shared<diffuse_light>(color(4, 4, 4));

    //Objects
    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(pertext)));
    world.add(make_shared<sphere>(point3(0, 2, 0), 2, make_shared<lambertian>(pertext)));

    world.add(make_shared<sphere>(point3(0, 7, 0), 2, difflight));
    world.add(make_shared<quad>(point3(3, 1, -2), vec3(2, 0, 0), vec3(0, 2, 0), difflight));

    // Camera Resolution
    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = image_width;
    cam.samples_per_pixel = samples_per_pixel;
    cam.max_depth         = max_depth;
    cam.background        = color(0.0, 0.0, 0.0);

    // Camera Position
    cam.vfov     = 20;
    cam.lookfrom = point3(26, 3, 6);
    cam.lookat   = point3(0, 2, 0);
    cam.vup      = vec3(0, 1, 0);

    // Depth of Field
    cam.defocus_angle = 0.0;

    // FileName
    cam.output = "scene9.ppm";
    
    return;
}

void scene10(hittable_list& world, camera& cam, int image_width = 600, int samples_per_pixel = 200, int max_depth = 50) {
    // Cornell Box Scene
    // Materials
    auto red   = make_shared<lambertian>(color(.65, .05, .05));
    auto white = make_shared<lambertian>(color(.73, .73, .73));
    auto green = make_shared<lambertian>(color(.12, .45, .15));
    auto light = make_shared<diffuse_light>(color(15, 15, 15));

    //Objects
    world.add(make_shared<quad>(point3(555, 0, 0), vec3(0, 555, 0), vec3(0, 0, 555), green));
    world.add(make_shared<quad>(point3(0, 0, 0), vec3(0, 555, 0), vec3(0, 0, 555), red));
    world.add(make_shared<quad>(point3(0, 0, 0), vec3(555, 0, 0), vec3(0, 0, 555), white));
    world.add(make_shared<quad>(point3(555, 555, 555), vec3(-555, 0, 0), vec3(0, 0, -555), white));
    world.add(make_shared<quad>(point3(0, 0, 555), vec3(555, 0, 0), vec3(0, 555, 0), white));

    shared_ptr<hittable> box1 = box(point3(0,0,0), point3(165, 330, 165), white);
    box1 = make_shared<rotate_y>(box1, 15);
    box1 = make_shared<translate>(box1, vec3(265, 0, 295));
    world.add(box1);
    shared_ptr<hittable> box2 = box(point3(0,0,0), point3(165, 165, 165), white);
    box2 = make_shared<rotate_y>(box2, -18);
    box2 = make_shared<translate>(box2, vec3(130, 0, 65));
    world.add(box2);

    world.add(make_shared<quad>(point3(343, 554, 332), vec3(-130, 0, 0), vec3(0, 0, -105), light));

    // Camera Resolution
    cam.aspect_ratio      = 1.0;
    cam.image_width       = image_width;
    cam.samples_per_pixel = samples_per_pixel;
    cam.max_depth         = max_depth;
    cam.background        = color(0.0, 0.0, 0.0);

    // Camera Position
    cam.vfov     = 40;
    cam.lookfrom = point3(278, 278, -800);
    cam.lookat   = point3(278, 278, 0);
    cam.vup      = vec3(0, 1, 0);

    // Depth of Field
    cam.defocus_angle = 0.0;

    // FileName
    cam.output = "scene10.ppm";
    
    return;
}

void scene11(hittable_list& world, camera& cam, int image_width = 600, int samples_per_pixel = 200, int max_depth = 50) {
    // Cornell Box w/ Constant_Medium
    // Materials
    auto red   = make_shared<lambertian>(color(.65, .05, .05));
    auto white = make_shared<lambertian>(color(.73, .73, .73));
    auto green = make_shared<lambertian>(color(.12, .45, .15));
    auto light = make_shared<diffuse_light>(color(7, 7, 7));

    //Objects
    world.add(make_shared<quad>(point3(555, 0, 0), vec3(0, 555, 0), vec3(0, 0, 555), green));
    world.add(make_shared<quad>(point3(0, 0, 0), vec3(0, 555, 0), vec3(0, 0, 555), red));
    world.add(make_shared<quad>(point3(0, 555, 0), vec3(555, 0, 0), vec3(0, 0, 555), white));
    world.add(make_shared<quad>(point3(0, 0, 0), vec3(555, 0, 0), vec3(0, 0, 555), white));
    world.add(make_shared<quad>(point3(0, 0, 555), vec3(555, 0, 0), vec3(0, 555, 0), white));

    shared_ptr<hittable> box1 = box(point3(0,0,0), point3(165, 330, 165), white);
    box1 = make_shared<rotate_y>(box1, 15);
    box1 = make_shared<translate>(box1, vec3(265, 0, 295));
    world.add(make_shared<constant_medium>(box1, 0.01, color(0, 0, 0)));

    shared_ptr<hittable> box2 = box(point3(0,0,0), point3(165, 165, 165), white);
    box2 = make_shared<rotate_y>(box2, -18);
    box2 = make_shared<translate>(box2, vec3(130, 0, 65));
    world.add(make_shared<constant_medium>(box2, 0.01, color(1, 1, 1)));

    world.add(make_shared<quad>(point3(343, 554, 332), vec3(-130, 0, 0), vec3(0, 0, -105), light));

    // Camera Resolution
    cam.aspect_ratio      = 1.0;
    cam.image_width       = image_width;
    cam.samples_per_pixel = samples_per_pixel;
    cam.max_depth         = max_depth;
    cam.background        = color(0.0, 0.0, 0.0);

    // Camera Position
    cam.vfov     = 40;
    cam.lookfrom = point3(278, 278, -800);
    cam.lookat   = point3(278, 278, 0);
    cam.vup      = vec3(0, 1, 0);

    // Depth of Field
    cam.defocus_angle = 0.0;

    // FileName
    cam.output = "scene11.ppm";
    
    return;
}

void scene12(hittable_list& world, camera& cam, int image_width = 400, int samples_per_pixel = 250, int max_depth = 4) {
    // TheNextWeek Scene
    // Materials
    auto ground = make_shared<lambertian>(color(0.48, 0.83, 0.53));
    auto sphere_material = make_shared<lambertian>(color(0.7, 0.3, 0.1));
    auto emat = make_shared<lambertian>(make_shared<image_texture>("earthmap.jpg"));
    auto pertext = make_shared<noise_texture>(0.2);
    auto white = make_shared<lambertian>(color(.73, .73, .73));
    auto light = make_shared<diffuse_light>(color(7, 7, 7));

    //Objects
    hittable_list boxes1;
    int boxes_per_side = 20;
    for(int i = 0; i < boxes_per_side; i++) {
        for(int j = 0; j < boxes_per_side; j++) {
            auto w = 100.0;
            auto x0 = -1000.0 + i*w;
            auto z0 = -1000.0 + j*w;
            auto y0 = 0.0;
            auto x1 = x0 + w;
            auto y1 = random_double(1,101);
            auto z1 = z0 + w;

            boxes1.add(box(point3(x0, y0, z0), point3(x1, y1, z1), ground));
        }
    }
    world.add(make_shared<bvh_node>(boxes1));

    auto center1 = point3(400, 400, 200);
    auto center2 = center1 + vec3(30, 0, 0);
    world.add(make_shared<sphere>(center1, center2, 50, sphere_material));

    world.add(make_shared<sphere>(point3(260, 150, 45), 50, make_shared<dielectric>(1.5)));
    world.add(make_shared<sphere>(
        point3(0, 150, 145), 50, make_shared<metal>(color(0.8, 0.8, 0.9), 1.0)
    ));

    auto boundary = make_shared<sphere>(point3(360, 150, 145), 70, make_shared<dielectric>(1.5));
    world.add(boundary);
    world.add(make_shared<constant_medium>(boundary, 0.2, color(0.2, 0.4, 0.9)));
    boundary = make_shared<sphere>(point3(0, 0, 0), 5000, make_shared<dielectric>(1.5));
    world.add(make_shared<constant_medium>(boundary, .0001, color(1, 1, 1)));

    world.add(make_shared<sphere>(point3(400, 200, 400), 100, emat));

    world.add(make_shared<sphere>(point3(220, 280, 300), 80, make_shared<lambertian>(pertext)));

    hittable_list boxes2;
    int ns = 1000;
    for(int j = 0; j < ns; j++) {
        boxes2.add(make_shared<sphere>(point3::random(0,165), 10, white));
    }

    world.add(make_shared<translate>(
        make_shared<rotate_y>(
            make_shared<bvh_node>(boxes2), 15),
            vec3(-100, 270, 395)
        )
    );
    world.add(make_shared<quad>(point3(123, 554, 147), vec3(300, 0, 0), vec3(0, 0, 265), light));

    // Camera Resolution
    cam.aspect_ratio      = 1.0;
    cam.image_width       = image_width;
    cam.samples_per_pixel = samples_per_pixel;
    cam.max_depth         = max_depth;
    cam.background        = color(0.0, 0.0, 0.0);

    // Camera Position
    cam.vfov     = 40;
    cam.lookfrom = point3(478, 278, -600);
    cam.lookat   = point3(278, 278, 0);
    cam.vup      = vec3(0, 1, 0);

    // Depth of Field
    cam.defocus_angle = 0.0;

    // FileName
    cam.output = "scene12.ppm";
    
    return;
}

#endif