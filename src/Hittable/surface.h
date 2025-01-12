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

            area = n.length();

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

        double pdf_value(const point3& origin, const vec3& direction) const override {
            hit_record rec;
            if(!this->hit(ray(origin, direction), interval(0.001, infinity), rec))
                return 0.0;
            
            auto distance_squared = rec.t * rec.t * direction.length_squared();
            auto cosine = fabs(dot(direction, rec.normal) / direction.length());

            return distance_squared / (cosine * area);
        }

        vec3 random(const point3& origin) const override {
            //Return Random Point on Quad
            auto p = Q + (random_double() * u) + (random_double() * v);
            return p - origin;
        }

        point3 center(double time) override {return cPoint;}

    private:
        point3 Q;
        vec3 u, v;
        vec3 w;
        shared_ptr<material> mat;
        aabb bbox;
        vec3 normal;
        double D;
        double area;
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

        //TODO: Add pdf_value and random

        point3 center(double time) override {return Q;}

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

class triangle : public hittable {
    public:
        triangle(const point3* verts, const vec3 *norms, shared_ptr<material> mats)
        : mat(mats) {
            vertex[0] = verts[0];
            vertex[1] = verts[1];
            vertex[2] = verts[2];
            normal[0] = norms[0];
            normal[1] = norms[1];
            normal[2] = norms[2];
            // Q, Q+u, Q+v are edges of the Triangle
            auto uLen = vertex[1].length_squared();
            auto vLen = vertex[2].length_squared();
            auto uv = dot(vertex[1],vertex[2]);

            if(uv != 0) {
                // if u,v perp
                s = vertex[1];
                t = vertex[2];
            }
            else if(uLen > vLen) {
                s = vertex[2]-dot(vertex[1],vertex[2])/(uLen*vLen)*vertex[1];
                t = vertex[1];
            }
            else {
                s = vertex[1]-dot(vertex[1],vertex[2])/(uLen*vLen)*vertex[2];
                t = vertex[2];
            }

            area = s.length()*t.length()/2;
            //alternatively (?) area = cross(vertex[1]-vertex[0], vertex[2]-vertex[0]).length()/2;
            
            cPoint = (vertex[0] + vertex[1]/3 + vertex[2]/3);
            
         }

        virtual void set_bounding_box() {
            //Determine Square enclosing Triangle
            auto bbox_diagonal1 = aabb(vertex[0], vertex[0]+t+s);
            auto bbox_diagonal2 = aabb(vertex[0]+s, vertex[0]+t);
            bbox = aabb(bbox_diagonal1, bbox_diagonal1);
        }
        
        virtual bool is_interior(double a, double b, hit_record& rec) const {
            // Can be adapted to fit any 2D-Shape
            interval unit_interval = interval(0, 1);

            // return false if outside quad, return true and set UV coordinates else
            if(!unit_interval.contains(a) || !unit_interval.contains(b) || a+b > 1)
                return false;

            rec.u = a;
            rec.v = b;
            return true;
        }
        
        //TODO: Add pdf_value and random

        double pdf_value(const point3& origin, const vec3& direction) const override {
            hit_record rec;
            if(!this->hit(ray(origin, direction), interval(0.001, infinity), rec))
                return 0.0;
            
            auto distance_squared = rec.t * rec.t * direction.length_squared();
            auto cosine = fabs(dot(direction, rec.normal) / direction.length());

            return distance_squared / (cosine * area);
        }

        vec3 random(const point3& origin) const override {
            //Return Random Point on Triangle
            double s1 = random_double();
            auto p = vertex[0] + (s1 * vertex[1]) + ((1-s1)*random_double() * vertex[2]);
            return p - origin;
        }

        point3 center(double time) const {return cPoint;}

    private:
        point3 vertex[3];
        vec3 normal[3]; // Useful for Rasterization (?), makes polygon appear as if it has curves
        vec3 s, t; // Two perp vectors, needed for BBox
        shared_ptr<material> mat;
        aabb bbox;
        double D;
        double area;
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