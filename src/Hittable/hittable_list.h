#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include "../Helper/rtweekend.h"

#include "../Bounding_Volume_Hierarchies/bvh.h"
#include "hittable.h"

#include <vector>

class hittable_list : public hittable {
    public:
        std::vector<shared_ptr<hittable>> objects;

        hittable_list() {}
        hittable_list(shared_ptr<hittable> object) { add(object); }

        void clear() {
            objects.clear();
            cUpdate = false;
        }
        void add(shared_ptr<hittable> object) {
            objects.push_back(object);
            bbox = aabb(bbox, object->bounding_box());
            cUpdate = false;
        }

        bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
            hit_record temp_rec;
            bool hit_anything = false;
            auto closest_so_far = ray_t.max;

            for(const auto& object : objects) {
                if(object->hit(r,interval(ray_t.min, closest_so_far), temp_rec)) {
                    hit_anything = true;
                    closest_so_far = temp_rec.t;
                    rec = temp_rec;
                }
            }

            return hit_anything;
        }

        aabb bounding_box() const override {return bbox;}

        double pdf_value(const point3& origin, const vec3& direction) const override {
            auto weight = 1.0 / objects.size();
            auto sum = 0.0;

            for(const auto& object : objects)
                sum += weight * object->pdf_value(origin, direction);

            return sum;
        }

        vec3 random(const point3& origin) const override {
            auto int_size = int(objects.size());
            return objects[random_int(0, int_size-1)]->random(origin);
        }

        point3 center(double time) override {
            if(cUpdate)
                return cPoint;
            int div = objects.size();
            int x = 0;
            int y = 0;
            int z = 0;
            for(shared_ptr<hittable> object : objects) {
                x += object->center(time).x();
                y += object->center(time).y();
                z += object->center(time).z();
            }
            x /= div;
            y /= div;
            z /= div;
            cPoint = point3(x,y,z);
            cUpdate = true;
            return cPoint;
        }

    private:
        aabb bbox;
        point3 cPoint;
        bool cUpdate = false;
};

#endif