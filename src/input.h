#ifndef SOFTWARERENDERERV2_INPUT_H
#define SOFTWARERENDERERV2_INPUT_H


#define IN_UP       0
#define IN_PRESSED  1
#define IN_DOWN     2
#define IN_RELEASED 3

void inCreate();

void inKeyReleasedFunc(unsigned char key, int x, int y);

void inKeyPressedFunc(unsigned char key, int x, int y);

void inUpdate();

int inGetKeyState(unsigned char key);

int inGetKeyY(unsigned char key);

int inGetKeyY(unsigned char key);

int inIsAnyInState(int state);

#endif
