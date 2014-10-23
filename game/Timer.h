#ifndef TIMER_H
#define TIMER_H

#define TIMER_ITEM_GLOBAL 5000
#define TIMER_MAGIC_SPELL 3
#define REFRESH_RATE 300

bool isTimerItemGlobalRunning = false;
bool isTimerMagicSpellRunning = false;

void timerRefresh(int value) {

	glutPostRedisplay();
	glutTimerFunc(REFRESH_RATE, timerRefresh, 0);
}

void timerItemGlobal(int value) {

	if (!isTimerItemGlobalRunning) {
		cout << "starting timerItemGlobal" << endl;
		isTimerItemGlobalRunning = true;
		glutTimerFunc(TIMER_ITEM_GLOBAL, timerItemGlobal, 0);
	} else {
		cout << "stopping timerItemGlobal" << endl;
		isTimerItemGlobalRunning = false;
	}
}

void timerMagicSpell(int value) {

	if (!isTimerMagicSpellRunning) {
		cout << "starting timerMagicSpell" << endl;
		isTimerMagicSpellRunning = true;
		glutTimerFunc(TIMER_MAGIC_SPELL, timerMagicSpell, 0);
	} else {
		cout << "stopping timerMagicSpell" << endl;
		isTimerMagicSpellRunning = false;
	}
}

#endif
