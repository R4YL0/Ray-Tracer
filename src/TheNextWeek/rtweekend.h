#ifndef RTWEEKEND_H
#define RTWEEKEND_H

#include <cmath>
#include <iostream>
#include <limits>
#include <memory>

// Multithreading
#include <fstream>
#include <thread>

// C++ Std Usings
using std::fabs;
using std::make_shared;
using std::shared_ptr;
using std::sqrt;
using std::ofstream;
using std::thread;

// Constants
const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

// Utility Functions
inline double degrees_to_radians(double degrees) {
    return degrees * pi / 180.0;
}

inline double random_double() {
    // [0,1)
    return rand() / (RAND_MAX + 1.0);
}

inline double random_double(double min, double max) {
    // [min, max)
    return min + (max-min)*random_double();
}

inline int random_int(int min, int max) {
    return int(random_double(min, max+1));
}

// Common Headers

#include "color.h"
#include "ray.h"
#include "vec3.h"
#include "interval.h"


#endif