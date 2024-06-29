#include "../Helper/rtweekend.h"

#include <iostream>
#include <iomanip>
#include <math.h>
#include <stdlib.h>

double f(const vec3& d) {
    auto cosine_squared = d.z() * d.z();
    return cosine_squared;
}

double pdf(const vec3& d) {
    return 1 / (4*pi);
}

int main() {
    int N = 1'000'000;
    auto sum = 0.0;
    for(int i = 0; i < N; i++) {
        vec3 d = random_unit_vector();
        auto f_d = f(d);
        sum += f_d / pdf(d);
    }
    std::cout << std::fixed << std::setprecision(12);
    std::cout << "I = " << sum / N << '\n';
}