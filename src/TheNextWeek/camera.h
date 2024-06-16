#ifndef CAMERA_H
#define CAMERA_H

#include <string>
#include <stdio.h>

#include "rtweekend.h"

#include "hittable.h"
#include "material.h"

class camera {
    public:
    // Camera Resolution
    double  aspect_ratio      = 1.0;
    int     image_width       = 100;
    int     samples_per_pixel = 10; // Anti-Aliasing Samples
    int     max_depth         = 10; // Ray Bounces Limit

    // Camera Position
    double vfov     = 90; // Vertical FoV
    point3 lookfrom = point3(0,0,0);
    point3 lookat   = point3(0,0,-1);
    vec3   vup      = vec3(0,1,0); // Camera-relative "up" direction

    // Depth of field
    double defocus_angle = 0;
    double focus_dist = 10;

    std::string output = "render.ppm";

    void render(const hittable& world) {
        initialize();

        //Select Output File
        ofstream renderedFile;
        renderedFile.open(output);

        renderedFile << "P3\n" << image_width << ' ' << image_height << "\n255\n";

        for(int j = 0; j < image_height; j++) {
            std::clog << "\rScanlines remaining: " << (image_height - j) << '\n' << std::flush;
            for(int i = 0; i < image_width; i++) {
                color pixel_color(0,0,0);
                for(int sample = 0; sample < samples_per_pixel; sample++) {
                    ray r = get_ray(i, j);
                    pixel_color += ray_color(r, max_depth, world);
                }
                write_color(renderedFile, pixel_samples_scale * pixel_color);
            }
        }
        renderedFile.close();
        std::clog << "\r Done. \n";
    }

    void render(const hittable& world, int threads) {
        initialize();


        // Divide the Work
        std::clog << "Starting Threads\n";
        thread t[threads];
        for(int i = 0; i < threads; i++) {
            t[i] = thread(&camera::renderMT, this, std::cref(world), i, threads);
        }
        for(int i = 0; i < threads; i++) {
            t[i].join();
        } 

        // Read The Files
        std::ifstream files[threads];
        std::string fileName[threads];
        for(int i = 0; i < threads; i++) {
            fileName[i] = "render" + std::to_string(i) + ".txt";
            files[i].open(fileName[i]);
            if(!files[i].is_open()) {
                std::cout << "Error Reading File: render" + std::to_string(i) + ".exe";
                return;
            }
        }

        // Output File        
        ofstream renderedFile;
        renderedFile.open(output);
        
        renderedFile << "P3\n" << image_width << ' ' << image_height << "\n255\n";
        std::string line;
        for(int i = 0; i < image_height; i++) {
            for(int j = 0; j < image_width; j++) {
                int curr = i % threads;
                std::getline(files[curr],line);
                renderedFile << line << '\n';
            }
        }

        // Delete temporary files
        for(int i = 0; i < threads; i++) {
            files[i].close();
            std::remove(fileName[i].c_str());
        }
        renderedFile.close();
        
        std::clog << "\rDone. \n";
        return;
    }

    private:
    int     image_height;
    double  pixel_samples_scale;
    point3  center;                 // Camera center
    point3  pixel00_loc;
    vec3    pixel_delta_u;          // Pixel Right offset
    vec3    pixel_delta_v;          // Pixel Bottom offset
    vec3    u, v, w;                // Camera frame basis vectors
    vec3    defocus_disk_u;
    vec3    defocus_disk_v;

    void initialize() {
        image_height = int(image_width / aspect_ratio);
        image_height = (image_height<1) ? 1 : image_height;

        pixel_samples_scale = 1.0 / samples_per_pixel;

        center = lookfrom;

        // Viewport Dimensions
        auto theta = degrees_to_radians(vfov);
        auto h = tan(theta/2);
        auto viewport_height = 2 * h * focus_dist;
        auto viewport_width = viewport_height * (double(image_width)/image_height);

        // Unit Basis Vectors for Camera Coordinate Frame
        w = unit_vector(lookfrom - lookat);
        u = unit_vector(cross(vup, w));
        v = cross(w, u);

        // Vectors: Horizontal and Vertical Viewport Edges
        vec3 viewport_u = viewport_width * u;
        vec3 viewport_v = viewport_height * -v;

        // Vectors: Horizontal and Vertical Pixel-Pixel-Delta
        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;

        // Location: Upper Left Pixel
        auto viewport_upper_left = center - (focus_dist * w) - viewport_u/2 - viewport_v/2;
        pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

        // Camera Defocus Disk Basis Vectors
        auto defocus_radius = focus_dist * tan(degrees_to_radians(defocus_angle / 2));
        defocus_disk_u = u * defocus_radius;
        defocus_disk_v = v * defocus_radius;
    }

    ray get_ray(int i , int j) const {
        // Camera Ray from Defocus Disk to random Pixel Location in [i, j]
        auto offset = sample_square();
        auto pixel_sample = pixel00_loc
                          + ((i + offset.x()) * pixel_delta_u)
                          + ((j + offset.y()) * pixel_delta_v);

        auto ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
        auto ray_direction = pixel_sample - ray_origin;
        auto ray_time = random_double();

        return ray(ray_origin, ray_direction, ray_time);
    }

    vec3 sample_square() const {
        // Return Vector to random point in [-0.5, -0.5]-[+0.5, +0.5] unit square
        return vec3(random_double() - 0.5, random_double() - 0.5, 0);
    }

    point3 defocus_disk_sample() const {
        // Return random point in camera defocus disk
        auto p = random_in_unit_disk();
        return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
    }

    color ray_color(const ray& r, int depth, const hittable& world) const {
        if (depth <= 0)
            return color(0,0,0);
        
        hit_record rec;

        // Ray Bouncing
        if(world.hit(r, interval(0.001, infinity), rec)) {
            ray scattered;
            color attenuation;
            if (rec.mat->scatter(r, rec, attenuation, scattered))
                return attenuation * ray_color(scattered, depth-1, world);
            return color(0, 0, 0);
            
        }

        // Background Illumination
        vec3 unit_direction = unit_vector(r.direction());
        auto a = 0.5 * (unit_direction.y() + 1.0);
        return (1.0-a)*color(1.0,1.0,1.0) + a*color(0.5,0.7,1.0);
    }

    void renderMT(const hittable& world, int curr, int threads) {
        ofstream file;
        file.open("render" + std::to_string(curr) + ".txt");

        for(int j = curr; j < image_height; j+= threads) {
            for(int i = 0; i < image_width; i++) {
                color pixel_color(0,0,0);
                for(int sample = 0; sample < samples_per_pixel; sample++) {
                    ray r = get_ray(i, j);
                    pixel_color += ray_color(r, max_depth, world);
                }
                write_color(file, pixel_samples_scale * pixel_color);
            }
        }
        file.close();
        std::clog << "Thread " << curr << " finished\n" << std::flush;
        return;
    }

};

#endif