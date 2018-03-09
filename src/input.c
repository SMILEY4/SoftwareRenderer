#include "input.h"
#include <stdio.h>


typedef struct {
    unsigned char key;
    unsigned char state;
    int x;
    int y;
} keystate_t;

keystate_t keystates[256];



void inCreate() {
    for(int i=0; i<256; i++) {
        keystate_t state;
        state.key = (unsigned char)i;
        state.x = 0;
        state.y = 0;
        state.state = IN_UP;
        keystates[i] = state;
    }
}




void inKeyPressedFunc(unsigned char key, int x, int y) {
    keystates[key].state = IN_PRESSED;
    keystates[key].x = x;
    keystates[key].y = y;
}




void inKeyReleasedFunc(unsigned char key, int x, int y) {
    keystates[key].state = IN_RELEASED;
    keystates[key].x = x;
    keystates[key].y = y;
}




void inUpdate() {
    for (int i=0; i<256; i++) {
        if(keystates[i].state == IN_PRESSED) {
            keystates[i].state = IN_DOWN;
        }
        if(keystates[i].state == IN_RELEASED) {
            keystates[i].state = IN_UP;
        }
    }
}




int inGetKeyState(unsigned char key) {
    return keystates[key].state;
}




int inGetKeyX(unsigned char key) {
    return keystates[key].x;
}




int inGetKeyY(unsigned char key) {
    return keystates[key].y;
}




int inIsAnyInState(int state) {
    for (int i=0; i<256; i++) {
        if(keystates[i].state == state) {
            return 1;
        }
    }
    return 0;
}












