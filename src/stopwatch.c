#include "stopwatch.h"
#include <windows.h>
#include <string.h>
#include <time.h>
#include <stdio.h>



typedef struct {
    char *name;
    clock_t start, end;
    int *samples;
    int nSamples;
    int size;
} data_t;




data_t *dataList;
int datasize = 0;




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


}




void watchReset() {
    for(int i=0; i<datasize; i++) {
        data_t *data = dataList + i;
        data->nSamples = 0;
    }
}




void watchStart(char *name) {
    data_t *data = getData(name);
    if(data) {
        data->start = clock();
    }
}




void watchEnd(char *name) {
    data_t *data = getData(name);
    if(data) {
        data->end = clock();
        int ms = data->end - data->start;
        data->nSamples += 1;
        if(data->size == 0) {
            data->size = 2;
            data->samples = calloc(data->size, sizeof(int));
        } else if(data->nSamples >= data->size) {
            data->size += 2;
            data->samples = realloc(data->samples, data->size * sizeof(int) );
        }
        data->samples[data->nSamples-1] = ms;
    }
}




void watchPrintData() {
    if(datasize == 0) {
        return;
    }
    printf("============\nWATCHDATA\n");
    for(int i=0; i<datasize; i++) {
        data_t *data = dataList + i;
        printf("%s (%d): ", data->name, data->nSamples);

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



void watchFreeData() {
    free(dataList);
    datasize = 0;
}