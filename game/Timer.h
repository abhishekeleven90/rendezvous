#define timer_item_global 5
#define timer_magic_spell 3

void timer(int value) {

	glutPostRedisplay();
	glutTimerFunc(REFRESH_RATE, timer, 0); //TODO: ensure this is not commented
}
