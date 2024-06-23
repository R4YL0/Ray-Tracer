#ifndef SURFACE_H
#define SURFACE_H

#include "../Helper/rtweekend.h"

#include "hittable.h"
#include "hittable_list.h"

class quad : public hittable {
    public:
        quad(const point3& Q, const vec3& u, const vec3& v, shared_ptr<material> mat)
         : Q(Q), u(u), v(v), mat(mat) {
            auto n = cross(u, v);
            normal = unit_vector(n);
            D = dot(normal, Q);
            w = n/(n.length_squared());

            cPoint = (Q + u/2 + v/2);
            
            set_bounding_box();
        }

        virtual void set_bounding_box() {
            auto bbox_diagonal1 = aabb(Q, Q+u+v);
            auto bbox_diagonal2 = aabb(Q+u, Q+v);
            bbox = aabb(bbox_diagonal1, bbox_diagonal1);
        }

        aabb bounding_box() const override {return bbox;}

        bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
            auto denom = dot(normal, r.direction());

            // No hit if ray parallel
            if(fabs(denom) < 1e-8)
                return false;

            // False if t outside of ray interval
            auto t = (D - dot(normal, r.origin())) / denom;
            if(!ray_t.contains(t))
                return false;

            // Determine hit point lies within planar shape
            auto intersection = r.at(t);
            vec3 planar_hitpt_vector = intersection - Q;
            auto alpha = dot(w, cross(planar_hitpt_vector, v));
            auto beta = dot(w, cross(u, planar_hitpt_vector));

            if(!is_interior(alpha, beta, rec))
                return false;

            // Ray hits 2D shape
            rec.t = t;
            rec.p = intersection;
            rec.mat = mat;
            rec.set_face_normal(r, normal);

            return true;
        }

        virtual bool is_interior(double a, double b, hit_record& rec) const {
            // Can be adapted to fit any 2D-Shape
            interval unit_interval = interval(0, 1);

            // return false if outside quad, return true and set UV coordinates else
            if(!unit_interval.contains(a) || !unit_interval.contains(b))
                return false;

            rec.u = a;
            rec.v = b;
            return true;
        }

        point3 center(double time) const override {return cPoint;}

    private:
        point3 Q;
        vec3 u, v;
        vec3 w;
        shared_ptr<material> mat;
        aabb bbox;
        vec3 normal;
        double D;
        point3 cPoint;
};

class circle : public quad {
    public:
        circle(const point3& Q, const vec3& u, const vec3& v, double r, shared_ptr<material> mat) 
         : quad(Q, r*unit_vector(u), r*unit_vector(v), mat), r(r) {}

        virtual void set_bounding_box() override {
            auto bbox_diagonal1 = aabb(Q-u-v, Q+u+v);
            auto bbox_diagonal2 = aabb(Q+u-v, Q-u+v);
            bbox = aabb(bbox_diagonal1, bbox_diagonal1);
        }
        
        virtual bool is_interior(double a, double b, hit_record& rec) const override {
            // return false if outside circle, return true and set UV coordinates else
            if(sqrt(a*a + b*b) > r)
                return false;

            rec.u = a;
            rec.v = b;
            return true;
        }

        point3 center(double time) const override {return Q;}

    private:
        point3 Q; // Center
        vec3 u, v; // Scaled Base Vectors
        double r; // Radius
        vec3 w;
        shared_ptr<material> mat;
        aabb bbox;
        vec3 normal;
        double D;
};

class triangle : public quad {
    public:
        triangle(const point3& Q, const vec3& u, const vec3& v, shared_ptr<material> mat) 
         : quad(Q, u, v, mat) {
            // Q, Q+u, Q+v are edges of the Triangle
            auto uLen = u.length_squared();
            auto vLen = v.length_squared();
            auto uv = dot(u,v);

            if(uv != 0) {
                // if u,v perp
                s = u;
                t = v;
            }
            else if(uLen > vLen) {
                s = v-dot(u,v)/(uLen*vLen)*u;
                t = u;
            }
            else {
                s = u-dot(u,v)/(uLen*vLen)*v;
                t = v;
            }

            cPoint = (Q + u/3 + v/3);
            
         }

        virtual void set_bounding_box() override {
            //Determine Square enclosing Triangle
            auto bbox_diagonal1 = aabb(Q, Q+t+s);
            auto bbox_diagonal2 = aabb(Q+s, Q+t);
            bbox = aabb(bbox_diagonal1, bbox_diagonal1);
        }
        
        virtual bool is_interior(double a, double b, hit_record& rec) const override {
            // Can be adapted to fit any 2D-Shape
            interval unit_interval = interval(0, 1);

            // return false if outside quad, return true and set UV coordinates else
            if(!unit_interval.contains(a) || !unit_interval.contains(b) || a+b > 1)
                return false;

            rec.u = a;
            rec.v = b;
            return true;
        }

        point3 center(double time) const {return cPoint;}

    private:
        point3 Q;
        vec3 u, v;
        vec3 w;
        vec3 s, t; // Two perp vectors, needed for BBox
        shared_ptr<material> mat;
        aabb bbox;
        vec3 normal;
        double D;
        point3 cPoint;
};

inline shared_ptr<hittable_list> box(const point3& a, const point3& b, shared_ptr<material> mat) {
    // Returns 3D box (six sides) with two opposite vertices a & b

    auto sides = make_shared<hittable_list>();

    // Construct opposite vertices with minimum and maximum coordinates
    auto min = point3(fmin(a.x(), b.x()), fmin(a.y(), b.y()), fmin(a.z(), b.z()));
    auto max = point3(fmax(a.x(), b.x()), fmax(a.y(), b.y()), fmax(a.z(), b.z()));

    auto dx = vec3(max.x() - min.x(), 0, 0);
    auto dy = vec3(0, max.y() - min.y(), 0);
    auto dz = vec3(0, 0, max.z() - min.z());

    sides->add(make_shared<quad>(point3(min.x(), min.y(), max.z()),  dx,  dy, mat)); // front
    sides->add(make_shared<quad>(point3(max.x(), min.y(), max.z()), -dz,  dy, mat)); // right
    sides->add(make_shared<quad>(point3(max.x(), min.y(), min.z()), -dx,  dy, mat)); // back
    sides->add(make_shared<quad>(point3(min.x(), min.y(), min.z()),  dz,  dy, mat)); // left
    sides->add(make_shared<quad>(point3(min.x(), max.y(), max.z()),  dx, -dz, mat)); // top
    sides->add(make_shared<quad>(point3(min.x(), min.y(), min.z()),  dx,  dz, mat)); // bottom

    return sides;
}

#endif