
#include "postProcess.h"
#include "bitmap.h"
#include "geometry.h"
#include "camera.h"
#include <math.h>
#include <windows.h>
#include <time.h>







float randFloat() {
    return (float)rand() / (float)RAND_MAX;
}



void ppAmbientOcclusion(bitmap_t *img) {

    srand(824759);

    // generate kernel
    const unsigned int KERNEL_SIZE = 20;
    vec_t kernel[KERNEL_SIZE];





    unsigned int width = img->width;
    unsigned int height = img->height;

    float radius = 5.0f;

    for(int x=0; x<width; x++) {
        for(int y=0; y<height; y++) {
            pixel_t *pixel = bmGetPixelAt(img, x, y);
            if(1.0f-pixel->depth < 0.00001 ) { continue; }

            for(int i=0; i<KERNEL_SIZE; i++) {
                kernel[i] = (vec_t){
                        randFloat() * 2.0f - 1.0f,
                        randFloat() * 2.0f - 1.0f,
                        randFloat() * 2.0f - 1.0f,
                        0.0f
                };
                vecNormalize(&kernel[i], &kernel[i]);
                vecScale(&kernel[i], &kernel[i], randFloat()*0.5f + 0.5f);
            }

            float pxDepth = pixel->depth;
            float scale = radius / (pxDepth);

            float ao = 0.0;   // 1 = occluded, 0 = free

            for(int i=0; i<KERNEL_SIZE; i++) {
                vec_t k = kernel[i];
                vec_t offset; vecScale(&offset, &k, scale);

                int sampleX = (int)(x + offset.x);
                int sampleY = (int)(y + offset.y);

                if(sampleX == x && sampleY == y) {
                    ao += 0.0f;
                    continue;
                }

                pixel_t *pxSample = bmGetPixelAt(img, sampleX, sampleY);
                if(!pxSample) {
                    ao += 0.0f;
                    continue;
                }

                float sampleDepth = pxSample->depth;
                float deltaDepth = pxDepth - sampleDepth;

                if(deltaDepth > 0) {
                    ao += 1.0f;
                }

            }

            ao = ao / (float)KERNEL_SIZE;
            ao = 1.0f - ao*ao;

            pixel->color = (color_t){pixel->color.r*ao, pixel->color.g*ao, pixel->color.b*ao, 1.0f};
            //pixel->color = (color_t){ao, ao, ao, 1.0f};

        }


    }


}