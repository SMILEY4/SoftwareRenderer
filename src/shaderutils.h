#ifndef SOFTWARERENDERER_SHADERUTILS_H
#define SOFTWARERENDERER_SHADERUTILS_H

#include "geometry.h"
#include "bitmap.h"
#include <math.h>
#include <windows.h>



#define PI (float)M_PI
#define INV_INFINITE 0.00000001f
#define pow2(x) ((x)*(x))
#define pow5(x) ((x)*(x)*(x)*(x)*(x))
#define mix(x,y,a) ((x)*(1.0f-(a))+(y)*(a))
#define limit(x) ( (x) < 0 ? 0 : (x > 1 ? 1 : x))




float randFloat();


// Normal Distribution Function (NDF)

float D_Blinn(float NdotH, float a);
float D_Beckmann(float NdotH, float a);
float D_GGX(float NdotH, float a);
float D_GGXaniso(float NdotH, float XdotH, float YdotH, float ax, float ay);




// Geometric Shadowing

float G_Implicit(float NdotL, float NdotV);
float G_Neumann(float NdotL, float NdotV);
float G_CookTorrance(float NdotL, float NdotV, float NdotH, float VdotH);
float G_Kelemen(float NdotL, float NdotV, float VdotH);
float Gs_Beckmann(float NdotV, float a);
float Gs_GGX(float NdotV, float a);
float Gs_SchlickBeckmann(float NdotV, float a);
float Gs_SchlickGGX(float NdotV, float a);
float G_Smith(float NdotL, float NdotV, float a);




// Fresnel

float F_Schlick(float f0, float u);
float F_Schlick2(float F0, float F90, float u);
float F_CookTorrance(float F0, float u);




// Diffuse BRDF Functions

float Diffuse_OrenNayar(float roughness, float NdotV, float NdotL, float VdotH);
float Diffuse_Lambert(float NdotV);




// calculate shading

float calcSpecular(float cNdotH, float NdotL, float NdotV, float f0, float roughness, vec_t *V, vec_t *N);
void shadeDefaultLight(vec_t *outColor, vec_t *N, vec_t *V, vec_t *L, vec_t *LightColor, vec_t *BaseColor, vec_t *RimColor, vec_t *SSColor, float metalness, float roughness, float rimPower);
void shadeEnvironment(vec_t *outColor, bitmap_t *envmap, vec_t *baseColor, vec_t *N);


#endif
