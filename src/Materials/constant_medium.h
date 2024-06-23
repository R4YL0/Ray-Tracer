#ifndef CONSTANT_MEDIUM_H
#define CONSTANT_MEDIUM_H

#include "../Helper/rtweekend.h"

#include "../Hittable/hittable.h"
#include "material.h"
#include "texture.h"

class constant_medium : public hittable {
    public:
        constant_medium(shared_ptr<hittable> boundary, double density, shared_ptr<texture> tex)
         : boundary(boundary), neg_inv_density(-1/density), phase_function(make_shared<isotropic>(tex)) {}

        constant_medium(shared_ptr<hittable> boundary, double density, const color& albedo)
         : boundary(boundary), neg_inv_density(-1/density), phase_function(make_shared<isotropic>(albedo)) {}

        bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
            // Occasional samples when debugging, set enableDebug true
            const bool enableDebug = false;
            const bool debugging = enableDebug && random_double() < 0.00001;

            // Only allows for Convex Shapes, i.e. entering it once
            // TO DO: Analyze the Program, understand it and allow for non-convex shapes (re-entering)
            hit_record rec1, rec2;

            double leftBorder = -infinity;

            auto ray_length = r.direction().length();
            double distance_inside_boundary = 0.0;
            auto hit_distance = neg_inv_density * log(random_double());

            while(true) {
                // Multiple Runs in case Shape is non-convex
                if(!boundary->hit(r, interval(leftBorder, infinity), rec1)) // Border-Entry: Constant_Medium
                    return false;
                
                if(!boundary->hit(r, interval(rec1.t+0.0001, infinity), rec2)) // Border-Exit: Constant_Medium
                    return false;

                leftBorder = rec2.t + 0.0001;

                if(debugging) std::clog << "\nt_min=" << rec1.t << ", t_max=" << rec2.t << '\n';

                if(rec1.t < ray_t.min) rec1.t = ray_t.min; // Take max(ray origin, Border-Entry)
                if(rec2.t > ray_t.max) rec2.t = ray_t.max; // Take min(ray limit, Border-Exit)

                if(rec1.t >= rec2.t) // Edge cases: rec1.t > ray_t.max || rec2.t < ray_t.min
                    continue;

                if(rec1.t < 0)
                    rec1.t = 0;

                distance_inside_boundary = (rec2.t - rec1.t) * ray_length;

                if(hit_distance > distance_inside_boundary) //Ray passes through Boundary
                    continue;
                break;
            }

            rec.t = rec1.t + hit_distance / ray_length;
            rec.p = r.at(rec.t);

            if(debugging) {
                std::clog << "hit_distance = " << hit_distance << '\n'
                          << "rec.t = " << rec.t << '\n'
                          << "rec.p = " << rec.p << '\n';
            }
            
            rec.normal = vec3(1, 0, 0); // arbitrary
            rec.front_face = true;      // arbitrary
            rec.mat = phase_function;

            return true;
        }

        aabb bounding_box() const override {return boundary->bounding_box();}

    private:
        shared_ptr<hittable> boundary;
        double neg_inv_density;
        shared_ptr<material> phase_function;
};

#endif