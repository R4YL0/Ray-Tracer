#ifndef CAMERA_H
#define CAMERA_H

#include <string>
#include <stdio.h>
#include <mutex>

#include "Helper/rtweekend.h"
//#include "External/glfw3.h"

#include "Hittable/hittable.h"
#include "Materials/material.h"
#include "Helper/pdf.h"
#include "Post-processing/filter.h"

class camera {
    public:
    // Camera Resolution
    double  aspect_ratio      = 1.0;
    int     image_width       = 100;
    int     samples_per_pixel = 10; // Anti-Aliasing Samples
    int     max_depth         = 10; // Ray Bounces Limit
    std::shared_ptr<texture> backgroundTex;

    // Camera Position
    double vfov     = 90; // Vertical FoV
    point3 lookfrom = point3(0,0,0);
    point3 lookat   = point3(0,0,-1);
    vec3   vup      = vec3(0,1,0); // Camera-relative "up" direction

    // Depth of field
    double defocus_angle = 0;
    double focus_dist = 10;

    enum rayTracingType {ambientOcclusion, shadowRays, reflectionsOnly, globalIllumination};

    std::string output = "render.ppm";
    
    void render(const hittable& world, const hittable& lights, int threads = 1, bool denoise = false) {
        initialize();

        double* img = (double*) malloc(sizeof(double)*image_width*image_height*3);
        double* dNois;
        // Divide the Work
        if(threads == 1) {
            drawPixels(world, lights, img);
        } else {
            thread t[threads];
            for(int i = 0; i < threads; i++) {
                std::clog << "Starting Thread " << i << ":\n";
                t[i] = thread(&camera::drawPixels, this, std::cref(world), std::cref(lights), std::ref(img), i, threads);
            }
            for(int i = 0; i < threads; i++) {
                t[i].join();
            }
        }

        //Post Processing:
        std::clog << "Starting Post-Processing:\n";
        //Denoising
        if(denoise) {
            dNois = (double*) malloc(sizeof(double)*image_width*image_height*3);
            dNois = filter(img, image_width, image_height);
        } else {
            dNois = img;
        }

        // Output File        
        ofstream renderedFile;
        renderedFile.open("../Rendered_Images/"+output);
        
        std::clog << "Writing output file...\n";
        renderedFile << "P3\n" << image_width << ' ' << image_height << "\n255\n";
        for(int i = 0; i < image_height; i++) {
            int row = i*image_width*3;
            for(int j = 0; j < image_width; j++) {
                int col = j*3;
                renderedFile << dNois[row + col] << ' ' << dNois[row + col + 1] << ' ' << dNois[row + col + 2] << '\n';
            }
        }
        
        std::clog << "\rDone. \n";
        return;
    }

    private:
    int     image_height;
    double  pixel_samples_scale;
    int     sqrt_spp;
    double  recip_sqrt_spp;
    point3  center;                 // Camera center
    point3  pixel00_loc;
    vec3    pixel_delta_u;          // Pixel Right offset
    vec3    pixel_delta_v;          // Pixel Bottom offset
    vec3    u, v, w;                // Camera frame basis vectors
    vec3    defocus_disk_u;
    vec3    defocus_disk_v;

    std::mutex counterLock;
    std::mutex finishLock;
    int counter = 0;

    void initialize() {
        image_height = int(image_width / aspect_ratio);
        image_height = (image_height<1) ? 1 : image_height;

        sqrt_spp = int(sqrt(samples_per_pixel));
        pixel_samples_scale = 1.0 / (sqrt_spp * sqrt_spp);
        recip_sqrt_spp = 1.0 / sqrt_spp;

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

        // Process Line Counter
        counter = image_height;

        // If not set, initialize backgroundTex to solid_color
        if(backgroundTex == nullptr)
            backgroundTex = make_shared<solid_color>(color());
    }

    ray get_ray(int i , int j, int s_i, int s_j) const {
        // Camera Ray from Defocus Disk to random Pixel Location in [i, j] for stratified sample square s_i, s_j
        auto offset = sample_square_stratified(s_i, s_j);
        auto pixel_sample = pixel00_loc
                          + ((i + offset.x()) * pixel_delta_u)
                          + ((j + offset.y()) * pixel_delta_v);

        auto ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
        auto ray_direction = pixel_sample - ray_origin;
        auto ray_time = random_double();

        return ray(ray_origin, ray_direction, ray_time);
    }

    vec3 sample_square_stratified(int s_i, int s_j) const {
        // Return Vector to random point in square sub-pixel of indices s_i, s_j for idealized unit square [-.5, -.5] to [.5, .5]
        auto px = ((s_i + random_double()) * recip_sqrt_spp) - 0.5;
        auto py = ((s_j + random_double()) * recip_sqrt_spp) - 0.5;

        return vec3(px, py, 0);
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

    // Path Tracing, brings Global Illumination
    color ray_color(const ray& r, int depth, const hittable& world, const hittable& lights) const {
        if (depth <= 0)
            return color(0,0,0);
        
        hit_record rec;

        // hit function is Ray-Triangle Intersection Test, possibly HW accellerated
        if(!world.hit(r, interval(0.001, infinity), rec)) {
            double u, v;
            vec3 p = unit_vector(r.direction());
            auto theta = acos(-p.y());
            auto phi = atan2(-p.z(), p.x()) + pi;

            u = phi / (2*pi);
            v = theta / pi;
            return backgroundTex->value(u,v,p);
        }

        // Ray Bouncing
        scatter_record srec;
        color color_from_emission = rec.mat->emitted(r, rec, rec.u, rec.v, rec.p);

        if(!rec.mat->scatter(r, rec, srec))
            return color_from_emission;

        // Specular reflection, no PDF
        if(srec.skip_pdf) {
            return srec.attenuation * ray_color(srec.skip_pdf_ray, depth - 1, world, lights);
        }

        auto light_ptr = make_shared<hittable_pdf>(lights, rec.p);
        mixture_pdf p(light_ptr, srec.pdf_ptr);

        ray scattered = ray(rec.p, p.generate(), r.time());
        auto pdf_val = p.value(scattered.direction());

        double scattering_pdf = rec.mat->scattering_pdf(r, rec, scattered);

        color sample_color = ray_color(scattered, depth - 1, world, lights);
        color color_from_scatter = (srec.attenuation * scattering_pdf * sample_color) / pdf_val; //Possible Rounding Errors
        
        //Limit color_from_scatter to Range [0,1] and set NaN to 0.0, making direct-lit surfaces brighter. No difference found in images
        /*
        for(int i = 0; i < 3; i++) {
            if(color_from_scatter[i] != color_from_scatter[i] || color_from_scatter[i] < 0.0) {
                color_from_scatter[0] = 0.0;
                color_from_scatter[1] = 0.0;
                color_from_scatter[2] = 0.0;
                break; 
            } else if (color_from_scatter[i] > 1.0) {
                double scale = fmax(color_from_scatter[0], fmax(color_from_scatter[1], color_from_scatter[2]));
                color_from_scatter[0] /= scale;
                color_from_scatter[1] /= scale;
                color_from_scatter[2] /= scale;
                break;
            }
        }
        */

        color colorSum = color_from_scatter + color_from_emission;

        for(int i = 0; i < 3; i++) {
            if(colorSum[i] != colorSum[i] || colorSum[i] < 0.0) {
                colorSum[0] = 0.0;
                colorSum[1] = 0.0;
                colorSum[2] = 0.0;
                break; 
            } else if (colorSum[i] > 1.0) {
                double scale = fmax(colorSum[0], fmax(colorSum[1], colorSum[2]));
                colorSum[0] /= scale;
                colorSum[1] /= scale;
                colorSum[2] /= scale;
                break;
            }
        }
        return colorSum;

    }

    // TODO: Add Functions for Ambient Occlusion, Shadow Rays

    void drawPixels(const hittable& world, const hittable& lights, double* array, int curr = 1, int threads = 1) {
        if(threads != 1) {
            for(int j = curr; j < image_height; j+= threads) {
                for(int i = 0; i < image_width; i++) {
                    color pixel_color(0,0,0);
                    for(int s_j = 0; s_j < sqrt_spp; s_j++) {
                        for(int s_i = 0; s_i < sqrt_spp; s_i++) {
                            ray r = get_ray(i, j, s_i, s_j);
                            pixel_color += ray_color(r, max_depth, world, lights);
                        }
                    }
                    write_color(&array[image_width*j*3 + i*3], pixel_samples_scale * pixel_color);
                }
                std::lock_guard<std::mutex> lock(counterLock);
                std::clog << "\rScanlines remaining: " << --counter << '\n' << std::flush;
            }
            std::lock_guard<std::mutex> lock(finishLock);
            std::clog << "Thread " << curr << " finished\n" << std::flush;
            return;
        } else {
            for(int j = curr; j < image_height; j+= 1) {
                for(int i = 0; i < image_width; i++) {
                    color pixel_color(0,0,0);
                    for(int s_j = 0; s_j < sqrt_spp; s_j++) {
                        for(int s_i = 0; s_i < sqrt_spp; s_i++) {
                            ray r = get_ray(i, j, s_i, s_j);
                            pixel_color += ray_color(r, max_depth, world, lights);
                        }
                    }
                    write_color(&array[image_width*j*3 + i*3], pixel_samples_scale * pixel_color);
                }
                std::clog << "\rScanlines remaining: " << --counter << '\n' << std::flush;
            }
            return;
        }
    }


};

#endif