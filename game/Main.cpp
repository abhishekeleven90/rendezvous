#include "Headers.h"
#include "MapNGrid.h"
#include "GeneralHelper.h"
#include "OpenGlHelper.h"
#include "Constants.h"
#include "Objects.h"
#include "SoundAll.h"
#include "Timer.h"
#include "Window_Main.h"
#include "Window_First.h"

int main(int argc, char** argv) {
	//Initialize GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(windowWidth, windowHeight); //Set the window size
	glutInitWindowPosition(10, 10);

	create_window_first();
	return 0; //This line is never reached
}
