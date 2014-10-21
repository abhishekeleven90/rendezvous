#include "Headers.h"

void timer(int value) {
	glutPostRedisplay();
	glutTimerFunc(REFRESH_RATE, timer, 0); //TODO: ensure this is not commented
}
