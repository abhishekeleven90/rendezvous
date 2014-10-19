#include "Headers.h"

void timer(int value) {
	//TODO - call updatefxn(to write)
	glutPostRedisplay();
	glutTimerFunc(100, timer ,0); //TODO: keep refreshRate in constants
}
