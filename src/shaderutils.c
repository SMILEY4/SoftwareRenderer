#include "shaderutils.h"


float randFloat() {
    return (float)rand() / (float)RAND_MAX;
}



// Normal Distribution Function (NDF)

float D_Blinn(float NdotH, float a) {
    return (1.0f / (PI*a*a)) * powf(NdotH, 2.0f/(a*a) - 2.0f);
}

float D_Beckmann(float NdotH, float a) {
    return (1.0f / (PI*a*a*NdotH*NdotH*NdotH*NdotH) ) * expf((NdotH*NdotH-1.0f)/(a*a*NdotH*NdotH));
}

float D_GGX(float NdotH, float a) {
    return (a*a) / (PI * pow2(NdotH*NdotH*(a*a-1.0f)+1.0f) );
}

float D_GGXaniso(float NdotH, float XdotH, float YdotH, float ax, float ay) {
    const float d = (pow2(XdotH)/pow2(ax)) + (pow2(YdotH)/pow2(ay)) + NdotH*NdotH;
    return (1.0f / (PI*ax*ay)) * (1.0f / (d*d));
}





// Geometric Shadowing

float G_Implicit(float NdotL, float NdotV) {
    return NdotL*NdotV;
}

float G_Neumann(float NdotL, float NdotV) {
    return (NdotL*NdotV) / max(NdotL,NdotV);
}

float G_CookTorrance(float NdotL, float NdotV, float NdotH, float VdotH) {
    return min(1.0f, min((2.0f*NdotH*NdotV)/VdotH, (2.0f*NdotH*NdotL)/VdotH));
}

float G_Kelemen(float NdotL, float NdotV, float VdotH) {
    return (NdotL*NdotV)/(VdotH*VdotH);
}

float Gs_Beckmann(float NdotV, float a) {
    const float c = NdotV / (a*sqrtf(1.0f-NdotV*NdotV));
    if(c < 1.6f) {
        return (3.535f*c + 2.181f*c*c)/(1.0f+2.276f*c+2.577f*c*c);
    } else {
        return 1.0f;
    }
}

float Gs_GGX(float NdotV, float a) {
    return (2.0f*NdotV) / ( NdotV + sqrtf(a*a+(1.0f-a*a)*NdotV*NdotV) );
}

float Gs_SchlickBeckmann(float NdotV, float a) {
    const float k = a*sqrtf(2.0f/PI);
    return NdotV * ( NdotV*(1.0f-k)+k);
}

float Gs_SchlickGGX(float NdotV, float a) {
    const float k = a/2.0f;
    return NdotV * ( NdotV*(1.0f-k)+k);
}

float G_Smith(float NdotL, float NdotV, float a) {
    return Gs_SchlickBeckmann(NdotL,a)*Gs_SchlickBeckmann(NdotV,a);
}




// Fresnel

float F_Schlick(float f0, float u) {
    return f0 + (1.0f-f0) * pow5(1.0f-u);
}

float F_Schlick2(float F0, float F90, float u) {
    return F0 + (F90 - F0) * pow5(1.0f - u);
}

float F_CookTorrance(float F0, float u) {
    const float sqrtF0 = sqrtf(F0);
    const float n = (1.0f+sqrtF0)/(1.0f-sqrtF0);
    const float c = u;
    const float g = sqrtf(n*n+c*c-1.0f);
    return 0.5f * pow2((g-c)/(g+c)) * ( 1.0f + pow2( ((g+c)*c-1.0f) / ((g-c)*c+1.0f) ) );
}


// Diffuse BRDF Functions

float Diffuse_OrenNayar(float roughness, float NdotV, float NdotL, float VdotH) {
    const float a = roughness*roughness;
    const float s = a; // (1.29f + 0.5f * a);
    const float s2 = s*s;
    const float VdotL = 2.0f * VdotH *VdotH;
    const float Cosri = VdotL - NdotV*NdotL;
    const float c1 = 1.0f - 0.5f * s2 / (s2+0.33f);
    const float c2 = 0.45f * s2 / (s2+0.09f) * Cosri * (Cosri >= 0.0f ? max(NdotL,NdotV) : 1.0f);
    return max(0.0f, 1.0f/PI * (c1+c2) * (1.0f+roughness*0.5f));
}

float Diffuse_Lambert(float NdotV) {
    return max(NdotV, 0.0f);
}






// calculate shading

float calcSpecular(float cNdotH, float NdotL, float NdotV, float f0, float roughness, vec_t *V, vec_t *N) {

    vec_t M;
    M.x = mix(N->x, -V->x, roughness);
    M.y = mix(N->y, -V->y, roughness);
    M.z = mix(N->z, -V->z, roughness);
    vecNormalize(&M, &M);
    const float cVdotM = max(0.0f, vecDot(V,&M));

    // FRESNEL
    const float F = fmaxf(F_Schlick(f0, cVdotM), 0.0f);

    // DISTRIBUTION
    const float D = max(D_GGX(cNdotH, roughness*roughness), 0.0f);

    // GEOMETRIC
    const float G = max(G_Smith(NdotL, NdotV, roughness*roughness), 0.0f);

    // FINAL
    return G*F*D;
}




void shadeDefaultLight(vec_t *outColor, vec_t *N, vec_t *V, vec_t *L, vec_t *LightColor, vec_t *BaseColor, vec_t *RimColor, vec_t *SSColor, float metalness, float roughness, float rimPower) {

    // VALUES
    vec_t H = (vec_t){V->x+L->x, V->y+L->y, V->z+L->z, 0.0f};
    vecNormalize(&H, &H);

    const float NdotL = vecDot(N, L);
    const float NdotV = vecDot(N, V);
    const float NdotH = vecDot(N, &H);
    const float VdotH = vecDot(V, &H);
    const float LdotH = vecDot(L, &H);

    const float cNdotL = max(0.0f, NdotL);
    const float cNdotV = max(0.0f, NdotV);
    const float cNdotH = max(0.0f, NdotH);
    const float cVdotH = max(0.0f, VdotH);
    const float cLdotH = max(0.0f, LdotH);

    float f0 = mix(0.02f, 0.6f, metalness);

    vec_t Color = *BaseColor;

    // rim color
    Color.x = mix(Color.x, RimColor->x, powf(1.0f-cNdotV,rimPower));
    Color.y = mix(Color.y, RimColor->y, powf(1.0f-cNdotV,rimPower));
    Color.z = mix(Color.z, RimColor->z, powf(1.0f-cNdotV,rimPower));


    // SPECULAR
    const float spec = max(calcSpecular(cNdotH, NdotL, NdotV, f0, roughness, V, N), 0.0f) * (1.0f-roughness);

    vec_t Specular = (vec_t){spec, spec, spec, 0.0f};

    vecScale(&Specular, &Specular, cNdotL);
    vecMul(&Specular, &Specular, LightColor);

    vecScale(&Specular, &Specular, mix(1.0f-powf(roughness,0.7f), 1.0f, metalness)); //  -> more saturated rough surfaces

    Specular.x = Specular.x * mix(1.0f, max(0.001f, Color.x), metalness);
    Specular.y = Specular.y * mix(1.0f, max(0.001f, Color.y), metalness);
    Specular.z = Specular.z * mix(1.0f, max(0.001f, Color.z), metalness);


    // DIFFUSE
    const float diff = max(Diffuse_OrenNayar(roughness, NdotV, NdotL, VdotH), 0.0f);

    vec_t Diffuse = (vec_t){diff,diff,diff, 0.0f};

    vecScale(&Diffuse, &Diffuse, (1.0f-f0));
    vecScale(&Diffuse, &Diffuse, (1.0f-metalness));
    vecScale(&Diffuse, &Diffuse, cNdotL);

    vecMul(&Diffuse, &Diffuse, &Color);
    vecMul(&Diffuse, &Diffuse, LightColor);

    // FINAL
    vecAdd(outColor, &Diffuse, &Specular);

}



void shadeEnvironment(vec_t *outColor, bitmap_t *envmap, vec_t *baseColor, vec_t *N) {
    pixel_t *pixel = bmGetPixelLongLat(envmap, N->x, -N->y, N->z, 1);
    if(pixel) {
        outColor->x = pixel->r * baseColor->x;
        outColor->y = pixel->g * baseColor->y;
        outColor->z = pixel->b * baseColor->z;
    }
}
