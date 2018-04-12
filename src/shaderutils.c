#include "shaderutils.h"
#include "geometry.h"
#include "bitmap.h"
#include <stdio.h>

vec_t poissonDisk[64];
matrix_t TNB;

void initShaderUtils() {
    poissonDisk[0]  = (vec_t){-0.613392f,  0.617481f,  0.0f,  0.0f};
    poissonDisk[1]  = (vec_t){ 0.170019f, -0.040254f,  0.0f,  0.0f};
    poissonDisk[2]  = (vec_t){-0.299417f,  0.791925f,  0.0f,  0.0f};
    poissonDisk[3]  = (vec_t){ 0.645680f,  0.493210f,  0.0f,  0.0f};
    poissonDisk[4]  = (vec_t){-0.651784f,  0.717887f,  0.0f,  0.0f};
    poissonDisk[5]  = (vec_t){ 0.421003f,  0.027070f,  0.0f,  0.0f};
    poissonDisk[6]  = (vec_t){-0.817194f, -0.271096f,  0.0f,  0.0f};
    poissonDisk[7]  = (vec_t){-0.705374f, -0.668203f,  0.0f,  0.0f};
    poissonDisk[8]  = (vec_t){ 0.977050f, -0.108615f,  0.0f,  0.0f};
    poissonDisk[9]  = (vec_t){ 0.063326f,  0.142369f,  0.0f,  0.0f};
    poissonDisk[10] = (vec_t){ 0.203528f,  0.214331f,  0.0f,  0.0f};
    poissonDisk[11] = (vec_t){-0.667531f,  0.326090f,  0.0f,  0.0f};
    poissonDisk[12] = (vec_t){-0.098422f, -0.295755f,  0.0f,  0.0f};
    poissonDisk[13] = (vec_t){-0.885922f,  0.215369f,  0.0f,  0.0f};
    poissonDisk[14] = (vec_t){ 0.566637f,  0.605213f,  0.0f,  0.0f};
    poissonDisk[15] = (vec_t){ 0.039766f, -0.396100f,  0.0f,  0.0f};
    poissonDisk[16] = (vec_t){ 0.751946f,  0.453352f,  0.0f,  0.0f};
    poissonDisk[17] = (vec_t){ 0.078707f, -0.715323f,  0.0f,  0.0f};
    poissonDisk[18] = (vec_t){-0.075838f, -0.529344f,  0.0f,  0.0f};
    poissonDisk[19] = (vec_t){ 0.724479f, -0.580798f,  0.0f,  0.0f};
    poissonDisk[20] = (vec_t){ 0.222999f, -0.215125f,  0.0f,  0.0f};
    poissonDisk[21] = (vec_t){-0.467574f, -0.405438f,  0.0f,  0.0f};
    poissonDisk[22] = (vec_t){-0.248268f, -0.814753f,  0.0f,  0.0f};
    poissonDisk[23] = (vec_t){ 0.354411f, -0.887570f,  0.0f,  0.0f};
    poissonDisk[24] = (vec_t){ 0.175817f,  0.382366f,  0.0f,  0.0f};
    poissonDisk[25] = (vec_t){ 0.487472f, -0.063082f,  0.0f,  0.0f};
    poissonDisk[26] = (vec_t){-0.084078f,  0.898312f,  0.0f,  0.0f};
    poissonDisk[27] = (vec_t){ 0.488876f, -0.783441f,  0.0f,  0.0f};
    poissonDisk[28] = (vec_t){ 0.470016f,  0.217933f,  0.0f,  0.0f};
    poissonDisk[29] = (vec_t){-0.696890f, -0.549791f,  0.0f,  0.0f};
    poissonDisk[30] = (vec_t){-0.149693f,  0.605762f,  0.0f,  0.0f};
    poissonDisk[31] = (vec_t){ 0.034211f,  0.979980f,  0.0f,  0.0f};
    poissonDisk[32] = (vec_t){ 0.503098f, -0.308878f,  0.0f,  0.0f};
    poissonDisk[33] = (vec_t){-0.016205f, -0.872921f,  0.0f,  0.0f};
    poissonDisk[34] = (vec_t){ 0.385784f, -0.393902f,  0.0f,  0.0f};
    poissonDisk[35] = (vec_t){-0.146886f, -0.859249f,  0.0f,  0.0f};
    poissonDisk[36] = (vec_t){ 0.643361f,  0.164098f,  0.0f,  0.0f};
    poissonDisk[37] = (vec_t){ 0.634388f, -0.049471f,  0.0f,  0.0f};
    poissonDisk[38] = (vec_t){-0.688894f,  0.007843f,  0.0f,  0.0f};
    poissonDisk[39] = (vec_t){ 0.464034f, -0.188818f,  0.0f,  0.0f};
    poissonDisk[40] = (vec_t){-0.440840f,  0.137486f,  0.0f,  0.0f};
    poissonDisk[41] = (vec_t){ 0.364483f,  0.511704f,  0.0f,  0.0f};
    poissonDisk[42] = (vec_t){ 0.034028f,  0.325968f,  0.0f,  0.0f};
    poissonDisk[43] = (vec_t){ 0.099094f, -0.308023f,  0.0f,  0.0f};
    poissonDisk[44] = (vec_t){ 0.693960f, -0.366253f,  0.0f,  0.0f};
    poissonDisk[45] = (vec_t){ 0.678884f, -0.204688f,  0.0f,  0.0f};
    poissonDisk[46] = (vec_t){ 0.001801f,  0.780328f,  0.0f,  0.0f};
    poissonDisk[47] = (vec_t){ 0.145177f, -0.898984f,  0.0f,  0.0f};
    poissonDisk[48] = (vec_t){ 0.062655f, -0.611866f,  0.0f,  0.0f};
    poissonDisk[49] = (vec_t){ 0.315226f, -0.604297f,  0.0f,  0.0f};
    poissonDisk[50] = (vec_t){-0.780145f,  0.486251f,  0.0f,  0.0f};
    poissonDisk[51] = (vec_t){-0.371868f,  0.882138f,  0.0f,  0.0f};
    poissonDisk[52] = (vec_t){ 0.200476f,  0.494430f,  0.0f,  0.0f};
    poissonDisk[53] = (vec_t){-0.494552f, -0.711051f,  0.0f,  0.0f};
    poissonDisk[54] = (vec_t){ 0.612476f,  0.705252f,  0.0f,  0.0f};
    poissonDisk[55] = (vec_t){-0.578845f, -0.768792f,  0.0f,  0.0f};
    poissonDisk[56] = (vec_t){-0.772454f, -0.090976f,  0.0f,  0.0f};
    poissonDisk[57] = (vec_t){ 0.504440f,  0.372295f,  0.0f,  0.0f};
    poissonDisk[58] = (vec_t){ 0.155736f,  0.065157f,  0.0f,  0.0f};
    poissonDisk[59] = (vec_t){ 0.391522f,  0.849605f,  0.0f,  0.0f};
    poissonDisk[60] = (vec_t){-0.620106f, -0.328104f,  0.0f,  0.0f};
    poissonDisk[61] = (vec_t){ 0.789239f, -0.419965f,  0.0f,  0.0f};
    poissonDisk[62] = (vec_t){-0.545396f,  0.538133f,  0.0f,  0.0f};
    poissonDisk[63] = (vec_t){-0.178564f, -0.596057f,  0.0f,  0.0f};
}




float randFloat() {
    return (float)rand() / (float)RAND_MAX;
}




float calcShadow(vec_t *N, vec_t *L, vec_t *shadowPos, bitmap_t *shadowmap, float zNear, float zFar) {

    float visibility = 0.0f;
    float cosTheta = vecDot(N, L);


    if(cosTheta < 0) {
        visibility = 0.0f;

    } else {
        float bias = max(1.2f * (1.0f - cosTheta), 0.05f);
        float radius = 1.5f;

        int nOccluded = 0;
        int nSamples = 0;

        for(int i=0; i<20; i++) {

            vec_t samplePos;
            int pdIndex = (int)(randFloat()*63);
            samplePos.x = shadowPos->x + poissonDisk[pdIndex].x*radius;
            samplePos.y = shadowPos->y + poissonDisk[pdIndex].y*radius;
            pixel_t *shadowPX = bmGetPixelAt(shadowmap, (unsigned int)samplePos.x, (unsigned int)samplePos.y, 1);

            float zShadow = reverseDepth(shadowPos->z, zNear, zFar);
            float zPixel = reverseDepth(shadowPX->r, zNear, zFar);

            if(zPixel < zShadow-bias) {
                nOccluded++;
            }

            nSamples++;

            // early out
            if( i==6) {
                if(nOccluded >= 6) {
                    break;
                }
                if(nOccluded == 0) {
                    break;
                }
            }

        }

        visibility = 1.0f - ((float)nOccluded / (float)nSamples);

    }

    return visibility;
}




void calcNormal(vec_t *Nout, vec_t *Nvert, vec_t *Nmap) {

    vec_t Nm = (vec_t){Nmap->r*2.0f-1.0f, Nmap->g*2.0f-1.0f, Nmap->b*2.0f-1.0f, 0.0f};
    Nm.x *= -1; // why ?

    vec_t Nv  = *Nvert;
    vec_t Nv2 = {Nv.x+4, Nv.y+4, Nv.z+4};
    vec_t Tv;   vecCross(&Tv, &Nv, &Nv2);
    vec_t Bv;   vecCross(&Bv, &Nv, &Tv);

    Nv.w = 0.0; vecNormalize(&Nv, &Nv);
    Tv.w = 0.0; vecNormalize(&Tv, &Tv);
    Bv.w = 0.0; vecNormalize(&Bv, &Bv);

    matrix_t TBN;
    TBN.m[0][0] = Tv.x;    TBN.m[0][1] = Bv.x;    TBN.m[0][2] = Nv.x;    TBN.m[0][3] = 0.0f;
    TBN.m[1][0] = Tv.y;    TBN.m[1][1] = Bv.y;    TBN.m[1][2] = Nv.y;    TBN.m[1][3] = 0.0f;
    TBN.m[2][0] = Tv.z;    TBN.m[2][1] = Bv.z;    TBN.m[2][2] = Nv.z;    TBN.m[2][3] = 0.0f;
    TBN.m[3][0] = 0.0f;    TBN.m[3][1] = 0.0f;    TBN.m[3][2] = 0.0f;    TBN.m[3][3] = 1.0f;

    matTransform(Nout, &Nm, &TBN);
    vecNormalize(Nout, Nout);

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




void shadeDefaultLight(vec_t *outColor, vec_t *N, vec_t *V, vec_t *L, vec_t *LightColor, vec_t *BaseColor, float metalness, float roughness) {

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


    // SPECULAR
    const float spec = max(calcSpecular(cNdotH, NdotL, NdotV, f0, roughness, V, N), 0.0f) * (1.0f-roughness);

    vec_t Specular = (vec_t){spec, spec, spec, 0.0f};

    vecScale(&Specular, &Specular, cNdotL);
    vecMul(&Specular, &Specular, LightColor);

    Specular.x = Specular.x * mix(1.0f, max(0.001f, BaseColor->x), metalness);
    Specular.y = Specular.y * mix(1.0f, max(0.001f, BaseColor->y), metalness);
    Specular.z = Specular.z * mix(1.0f, max(0.001f, BaseColor->z), metalness);

    // DIFFUSE
    const float diff = max(Diffuse_OrenNayar(roughness, NdotV, NdotL, VdotH), 0.0f);

    vec_t Diffuse = (vec_t){diff,diff,diff, 0.0f};

    vecScale(&Diffuse, &Diffuse, (1.0f-f0));
    vecScale(&Diffuse, &Diffuse, (1.0f-metalness));
    vecScale(&Diffuse, &Diffuse, cNdotL);

    vecMul(&Diffuse, &Diffuse, BaseColor);
    vecMul(&Diffuse, &Diffuse, LightColor);

    // FINAL
    vecAdd(outColor, &Diffuse, &Specular);

}



void calcReflection(vec_t *outColor, vec_t *R, bitmap_t *img, float roughness, float metalness, vec_t *baseColor, vec_t *N, vec_t *V) {

    const int N_SAMPLES = 32;

    const float imgWidth = img->width;
    const float radius = (imgWidth*0.03f) * (roughness*roughness);

    const float F_PI = (float)M_PI;
    float sx = (atan2f(R->z, R->x) + F_PI) / (2.0f * F_PI);
    float sy = acosf(-R->y) / F_PI;
    sx *= img->width;
    sy *= img->height;

    vec_t sampleColor = (vec_t){0.0, 0.0, 0.0, 0.0};

    for(int i=0; i<N_SAMPLES; i++) {

        float sampleX = (randFloat() * 2.0f - 1.0f) * radius;
        float sampleY = (randFloat() * 2.0f - 1.0f) * radius;

        sx += sampleX;
        sy += sampleY;

        pixel_t *pixel = bmGetPixelAt(img, (int)sx, (int)sy, 1);
        if(pixel) {
            sampleColor.r += pixel->r;
            sampleColor.g += pixel->g;
            sampleColor.b += pixel->b;
        }
    }
    vecScale(&sampleColor, &sampleColor, 1.0f/(float)N_SAMPLES);


    float f0  = mix(0.02f, 0.6f, metalness);
    float f90 = mix(0.65f, 1.0f, metalness);

    vec_t M = (vec_t){
        mix(N->x, V->x, roughness),
        mix(N->y, V->y, roughness),
        mix(N->z, V->z, roughness),
        0.0f
    };
    vecNormalize(&M, &M);


    float MdotV = vecDot(&M,V);

    float envBRDF = f0 + (f90-f0) * (1.0f-MdotV);

    vec_t specColor = (vec_t){
        mix(1.0f, baseColor->r, metalness),
        mix(1.0f, baseColor->g, metalness),
        mix(1.0f, baseColor->b, metalness),
    };


    outColor->r = envBRDF * sampleColor.r * specColor.r;
    outColor->g = envBRDF * sampleColor.g * specColor.g;
    outColor->b = envBRDF * sampleColor.b * specColor.b;


}
