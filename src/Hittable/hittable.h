#ifndef HITTABLE_H
#define HITTABLE_H

#include "../Helper/rtweekend.h"

#include "../Bounding_Volume_Hierarchies/aabb.h"

class material;

class hit_record {
    public:
        point3 p; // Hit-Point
        vec3 normal;
        shared_ptr<material> mat;
        double t; // Closest Hit-Distance
        double u, v;
        bool front_face;

        void set_face_normal(const ray& r, const vec3& outward_normal) {
            // Set hit record normal Vector
            // Pre-Condition: 'outward_normal' is normalized

            front_face = dot(r.direction(), outward_normal) < 0;
            normal = front_face ? outward_normal : -outward_normal;
        }

};

class hittable {
    public:
        virtual ~hittable() = default;
        
        virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const = 0;

        virtual aabb bounding_box() const = 0;

        virtual point3 center(double time) {return point3();} //Needed For Rotation and Scaling, default returns (0, 0, 0)

        virtual double pdf_value(const point3& origin, const vec3& direction) const {
            return 0.0;
        }

        virtual vec3 random(const point3& origin) const {
            return vec3(1, 0, 0);
        }
};

class translate : public hittable {
    public:
        translate(shared_ptr<hittable> object, const vec3& offset)
         : object(object), offset(offset) {
            bbox = object->bounding_box() + offset;
        }
        
        bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
            // Move ray backwards by offset
            ray offset_r(r.origin() - offset, r.direction(), r.time());

            //Determine if intersection exists
            if(!object->hit(offset_r, ray_t, rec))
                return false;

            //Move intersection point forward by the offset
            rec.p += offset;

            return true;
        }

        aabb bounding_box() const override {return bbox;}

        point3 center(double time) override {return object->center(time);}

    private:
        shared_ptr<hittable> object;
        vec3 offset;
        aabb bbox;
};

class rotate_y : public hittable {
    public:
        rotate_y(shared_ptr<hittable> object, double angle) : object(object) {
            auto radians = degrees_to_radians(angle);
            sin_theta = sin(radians);
            cos_theta = cos(radians);
            bbox = object->bounding_box();

            point3 min( infinity, infinity, infinity);
            point3 max(-infinity,-infinity,-infinity);

            for(int i = 0; i < 2; i++) {
                for(int j = 0; j < 2; j++) {
                    for(int k = 0; k < 2; k++) {
                        auto x = i*bbox.x.max + (1-i)*bbox.x.min;
                        auto y = j*bbox.y.max + (1-j)*bbox.y.min;
                        auto z = k*bbox.z.max + (1-k)*bbox.z.min;

                        auto newx =  cos_theta*x + sin_theta*z;
                        auto newz = -sin_theta*x + cos_theta*z;

                        vec3 tester(newx, y, newz);

                        for(int c = 0; c < 3; c++) {
                            min[c] = fmin(min[c], tester[c]);
                            max[c] = fmax(max[c], tester[c]);
                        }
                    }
                }
            }
            bbox = aabb(min, max);
        }

        bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
            // Change Ray from world space to object space
            //auto origin = r.origin(); // Tutorial
            auto origin = r.origin() - object->center(r.time()); // Fixed
            auto direction = r.direction();
            point3 Cpy = origin;

            origin[0] = cos_theta*Cpy[0] - sin_theta*Cpy[2];
            origin[2] = sin_theta*Cpy[0] + cos_theta*Cpy[2];

            origin += object->center(r.time()); // Fixed

            direction[0] = cos_theta*r.direction()[0] - sin_theta*r.direction()[2];
            direction[2] = sin_theta*r.direction()[0] + cos_theta*r.direction()[2];

            ray rotated_r(origin, direction, r.time());

            // Determine if Intersection in object space
            if(!object->hit(rotated_r, ray_t, rec))
                return false;
            
            // Change intersection point from object space to world space
            //auto p = rec.p; // Tutorial
            auto p = rec.p - object->center(r.time()); // Fixed
            Cpy = p;

            p[0] = cos_theta*Cpy[0] + sin_theta*Cpy[2];
            p[2] = -sin_theta*Cpy[0] + cos_theta*Cpy[2];

            p += object->center(r.time()); // Fixed

            // Change Normal from object space to world space
            auto normal = rec.normal;
            normal[0] = cos_theta*rec.normal[0] + sin_theta*rec.normal[2];
            normal[2] = -sin_theta*rec.normal[0] + cos_theta*rec.normal[2];

            rec.p = p;
            rec.normal = normal;

            return true;
        }

        aabb bounding_box() const override {return bbox;}

        point3 center(double time) override {return object->center(time);}

    private:
        shared_ptr<hittable> object;
        double sin_theta;
        double cos_theta;
        aabb bbox;
};

class rotate_x : public hittable {
    public:
        rotate_x(shared_ptr<hittable> object, double angle) : object(object) {
            auto radians = degrees_to_radians(angle);
            sin_theta = sin(radians);
            cos_theta = cos(radians);
            bbox = object->bounding_box();

            point3 min( infinity, infinity, infinity);
            point3 max(-infinity,-infinity,-infinity);

            for(int i = 0; i < 2; i++) {
                for(int j = 0; j < 2; j++) {
                    for(int k = 0; k < 2; k++) {
                        auto x = i*bbox.x.max + (1-i)*bbox.x.min;
                        auto y = j*bbox.y.max + (1-j)*bbox.y.min;
                        auto z = k*bbox.z.max + (1-k)*bbox.z.min;

                        auto newy = cos_theta*y - sin_theta*z;
                        auto newz = sin_theta*y + cos_theta*z;

                        vec3 tester(x, newy, newz);

                        for(int c = 0; c < 3; c++) {
                            min[c] = fmin(min[c], tester[c]);
                            max[c] = fmax(max[c], tester[c]);
                        }
                    }
                }
            }
            bbox = aabb(min, max);
        }

        bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
            // Change Ray from world space to object space
            auto origin = r.origin() - object->center(r.time());;
            auto direction = r.direction();
            point3 Cpy = origin;

            origin[2] = cos_theta*Cpy[2] - sin_theta*Cpy[1];
            origin[1] = sin_theta*Cpy[2] + cos_theta*Cpy[1];

            origin += object->center(r.time());

            direction[2] = cos_theta*r.direction()[2] - sin_theta*r.direction()[1];
            direction[1] = sin_theta*r.direction()[2] + cos_theta*r.direction()[1];

            ray rotated_r(origin, direction, r.time());

            // Determine if Intersection in object space
            if(!object->hit(rotated_r, ray_t, rec))
                return false;
            
            // Change intersection point from object space to world space
            auto p = rec.p - object->center(r.time());
            Cpy = p;

            p[2] = cos_theta*Cpy[2] + sin_theta*Cpy[1];
            p[1] = -sin_theta*Cpy[2] + cos_theta*Cpy[1];

            p += object->center(r.time());

            // Change Normal from object space to world space
            auto normal = rec.normal;

            normal[2] = cos_theta*rec.normal[2] + sin_theta*rec.normal[1];
            normal[1] = -sin_theta*rec.normal[2] + cos_theta*rec.normal[1];

            rec.p = p;
            rec.normal = normal;

            return true;
        }

        aabb bounding_box() const override {return bbox;}

        point3 center(double time) override {return object->center(time);}

    private:
        shared_ptr<hittable> object;
        double sin_theta;
        double cos_theta;
        aabb bbox;
};

class rotate_z : public hittable {
    public:
        rotate_z(shared_ptr<hittable> object, double angle) : object(object) {
            auto radians = degrees_to_radians(angle);
            sin_theta = sin(radians);
            cos_theta = cos(radians);
            bbox = object->bounding_box();

            point3 min( infinity, infinity, infinity);
            point3 max(-infinity,-infinity,-infinity);

            for(int i = 0; i < 2; i++) {
                for(int j = 0; j < 2; j++) {
                    for(int k = 0; k < 2; k++) {
                        auto x = i*bbox.x.max + (1-i)*bbox.x.min;
                        auto y = j*bbox.y.max + (1-j)*bbox.y.min;
                        auto z = k*bbox.z.max + (1-k)*bbox.z.min;

                        auto newx = cos_theta*x - sin_theta*y;
                        auto newy = sin_theta*x + cos_theta*y;

                        vec3 tester(newx, newy, z);

                        for(int c = 0; c < 3; c++) {
                            min[c] = fmin(min[c], tester[c]);
                            max[c] = fmax(max[c], tester[c]);
                        }
                    }
                }
            }
            bbox = aabb(min, max);
        }

        bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
            // Change Ray from world space to object space
            auto origin = r.origin() - object->center(r.time());
            auto direction = r.direction();
            point3 Cpy = origin;

            origin[1] = cos_theta*Cpy[1] - sin_theta*Cpy[0];
            origin[0] = sin_theta*Cpy[1] + cos_theta*Cpy[0];

            origin += object->center(r.time());

            direction[1] = cos_theta*r.direction()[1] - sin_theta*r.direction()[0];
            direction[0] = sin_theta*r.direction()[1] + cos_theta*r.direction()[0];

            ray rotated_r(origin, direction, r.time());

            // Determine if Intersection in object space
            if(!object->hit(rotated_r, ray_t, rec))
                return false;
            
            // Change intersection point from object space to world space
            auto p = rec.p - object->center(r.time());
            Cpy = p;

            p[1] = cos_theta*Cpy[1] + sin_theta*Cpy[0];
            p[0] = -sin_theta*Cpy[1] + cos_theta*Cpy[0];

            p += object->center(r.time());

            // Change Normal from object space to world space
            auto normal = rec.normal;
            normal[1] = cos_theta*rec.normal[1] + sin_theta*rec.normal[0];
            normal[0] = -sin_theta*rec.normal[1] + cos_theta*rec.normal[0];

            rec.p = p;
            rec.normal = normal;

            return true;
        }

        aabb bounding_box() const override {return bbox;}

        point3 center(double time) override {return object->center(time);}

    private:
        shared_ptr<hittable> object;
        double sin_theta;
        double cos_theta;
        aabb bbox;
};

// TODO: Scaling

class scale : public hittable {
    public:
        scale(shared_ptr<hittable> object, const vec3& scaling)
        : object(object), xScale(scaling.x()), yScale(scaling.y()), zScale(scaling.z()) {
            //Scale the bbox from the center point outward
            xScaled = object->bounding_box().x;
            yScaled = object->bounding_box().y;
            zScaled = object->bounding_box().z;

            int mid = xScaled.max/2 + xScaled.min/2;
            int lower = mid - xScale*xScaled.min;
            int upper = mid + xScale*xScaled.max;
            xScaled = interval(lower, upper);

            mid = yScaled.max/2 + yScaled.min/2;
            lower = mid - yScale*yScaled.min;
            upper = mid + yScale*yScaled.max;
            yScaled = interval(lower, upper);

            mid = zScaled.max/2 + zScaled.min/2;
            lower = mid - zScale*zScaled.min;
            upper = mid + zScale*zScaled.max;
            zScaled = interval(lower, upper);

            bbox = aabb(xScaled, yScaled, zScaled);
        }

        bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
            //Scale object
            return false;
        }

        aabb bounding_box() const override {return bbox;}
    private:
        shared_ptr<hittable> object;
        double xScale;
        double yScale;
        double zScale;
        aabb bbox;
        interval xScaled;
        interval yScaled;
        interval zScaled;
};

#endif