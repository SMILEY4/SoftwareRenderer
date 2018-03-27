#include "stopwatch.h"
#include <windows.h>
#include <string.h>
#include <time.h>
#include <stdio.h>

#define MAX_SAMPLES 50000

typedef struct {
    char *name;
    clock_t start, end;
    int *samples;
    int nSamples;
    int size;
    int level;
} data_t;


data_t *dataList;
int datasize = 0;
int currentLevel = 0;



data_t *getData(char *name) {
    data_t *data = NULL;
    for(int i=0; i<datasize; i++) {
        data_t *d = dataList+i;
        if(strcmp(name, d->name) == 0) {
            data = d;
            break;
        }
    }

    if(data == NULL) {
        datasize += 1;
        dataList = realloc(dataList, datasize * sizeof(data_t));
        data = dataList + (datasize - 1);
        data->name = name;
        data->nSamples = 0;
        data->size = 0;
    }

    return data;
}




void samplesReset() {
    for(int i=0; i<datasize; i++) {
        data_t *data = dataList + i;
        data->nSamples = 0;
    }
    currentLevel = 0;
}




void sampleStart(char *name) {
    data_t *data = getData(name);
    if(data) {
        data->start = clock();
        data->level = currentLevel;
        currentLevel++;
    }
}




void sampleEnd(char *name) {
    data_t *data = getData(name);
    if(data) {
        data->end = clock();
        int ms = data->end - data->start;
        data->nSamples += 1;

        if(data->nSamples > MAX_SAMPLES) {
            data->nSamples = MAX_SAMPLES;
            data->samples[data->nSamples-1] = (data->samples[data->nSamples-1] + ms) / 2;

        } else {
            if(data->size == 0) {
                data->size = 2;
                data->samples = calloc((size_t)data->size, sizeof(int));

            } else if(data->nSamples >= data->size) {
                data->size += 2;
                data->samples = realloc(data->samples, data->size * sizeof(int) );
            }

            data->samples[data->nSamples-1] = ms;
        }
        currentLevel--;
    } else {
        printf("WATCHEND_ERROR: COULD NOT FIND %s\n", name);
    }
}




void sampelsPrintData() {
    if(datasize == 0) {
        return;
    }
    printf("============\nWATCHDATA\n");
    for(int i=0; i<datasize; i++) {
        data_t *data = dataList + i;
        printf("%*c %s (%d): ", data->level*2+1, ' ', data->name, data->nSamples);

        if(data->nSamples > 8) {
            int sum = 0;
            for(int j=0; j<data->nSamples; j++) {
                sum += data->samples[j];
            }
            float avg = (float)sum / (float)data->nSamples;
            printf(" avg: %fms", avg);

        } else {
            for(int j=0; j<data->nSamples; j++) {
                printf(" %dms", data->samples[j]);
            }
        }

        printf("\n");
    }
    printf("============\n\n");
}



void samplesFreeData() {
    for(int i=0; i<datasize; i++) {
        data_t *data = dataList+i;
        if(data->nSamples > 0) {
            free(data->samples);
        }
    }
    free(dataList);
    datasize = 0;
}