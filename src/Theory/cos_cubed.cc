#include "../Helper/rtweekend.h"

#include <iostream>
#include <iomanip>
#include <math.h>

double f(double r2) {
    // auto x = cos(2*pi*r1)*2*sqrt(r2*(1-r2));
    // auto y = sin(2*pi*r1)*2*sqrt(r2*(1-r2));
    auto z = 1 - r2;
    double cos_theta = z;
    return cos_theta*cos_theta*cos_theta;
}

double pdf() {
    return 1.0 / (2.0 * pi);
}

int main() {
    int N = 1'000'000;
    auto sum = 0.0;
    for(int i = 0; i < N; i++) {
        auto r2 = f(random_double());
        sum += f(r2) / pdf();
    }
    std::cout << std::fixed << std::setprecision(12);
    std::cout << "PI/2 = " << pi / 2.0 << '\n';
    std::cout << "Estimate = " << sum / N << '\n';
}