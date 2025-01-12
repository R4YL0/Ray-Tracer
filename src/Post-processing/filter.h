#ifndef FILTER_H
#define FILTER_H

#include <string>
#include <stdio.h>
#include <mutex>

#include "../Helper/rtweekend.h"

double kernel[3][3] = {{-1/9, -1/9, 1-1/9}, {-2-1/9, 8/9, 2-1/9}, {-1-1/9, -1/9, 2-1/9}};

double* out;

double* filter(double* img, int imgWidth, int imgHeight, double* filter = kernel[0], int filterDim = 3) {
        double weightCntr = 0.34;
        double weightEdge = 0.33;
        double weightCrnr = 0.33;

        //Apply Filter Matrix with Convolution (?)
        //double kernel[3][3] = {{-1/9, -1/9, 1-1/9}, {-2-1/9, 8/9, 2-1/9}, {-1-1/9, -1/9, 2-1/9}}

        out[imgWidth*imgHeight*3];
        
        for(int i = 0; i < imgHeight; i++) {
            for(int j = 0; j < imgWidth; j++) {
                double ratio = 0;
                int pos = imgWidth*3*i + 3*j;
                vec3 colorIn(&img[pos]);
                for(int w = -4; w < 5; w++) {
                    if(i+w == 0 || i+w == imgWidth)
                        continue;
                    for(int h = -4; h < 5; h++) {
                        if(j+h == 0 || j+h == imgHeight) {
                            continue;
                        }
                        vec3 colorNB(&img[imgWidth*3*(i+w)+3*(j+h)]);
                        // Dynamic Weights:
                        //vec3 diff = colorIn-colorNB;
                        //double weight = 1 - diff.x()*diff.x()*diff.y()*diff.y()*diff.z()*diff.z()/255;
                        // Static Weights:
                        double weight = w == 0 ? (h == 0 ? weightCntr : weightEdge) : (h == 0 ? weightEdge : weightCrnr);
                        
                        std::clog << "weight is \n" << weight;
                        for(int k = 0; k < 3; k++) {
                            out[pos + k] += weight*colorNB[k];
                        }
                        ratio += weight;
                    }
                }
                for(int k = 0; k < 3; k++) {
                    out[pos + k] /= ratio;
                }
            }
        }
        return out;
    }

    #endif