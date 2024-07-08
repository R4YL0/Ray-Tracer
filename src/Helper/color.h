#ifndef COLOR_H
#define COLOR_H

#include "rtweekend.h"

#include "interval.h"
#include "vec3.h"

using color = vec3;

inline double linear_to_gamma(double linear_component) {
    if (linear_component > 0)
        return sqrt(linear_component);
    return 0;
}

void write_color(std::ostream& out, const color & pixel_color) {
    auto r = pixel_color.x();
    auto g = pixel_color.y();
    auto b = pixel_color.z();

    // Lazy Bug-fix: replace NaN with zero
    /*
    if(r != r) r = 0.0;
    if(g != g) g = 0.0;
    if(b != b) b = 0.0;
    */

    // Linear -> Gamma Transform for gamma 2
    r = linear_to_gamma(r);
    g = linear_to_gamma(g);
    b = linear_to_gamma(b);

    // Translate [0,1] -> [0,255]
    static const interval intensity(0.000, 0.999);
    int rbyte = int(256 * intensity.clamp(r));
    int gbyte = int(256 * intensity.clamp(g));
    int bbyte = int(256 * intensity.clamp(b));

    // Write pixel color components
    out << rbyte << ' ' << gbyte << ' ' << bbyte << '\n';
}

#endif