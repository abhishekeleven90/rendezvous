#ifndef TIMER_H
#define TIMER_H

#define TIMER_ITEM_GLOBAL 5000
#define TIMER_MAGIC_SPELL 3
#define REFRESH_RATE 100

void timerRefresh(int value) {

	glutPostRedisplay();
	glutTimerFunc(currentPlayer.speedMove, timerRefresh, 0);
}

void timerItemGlobal(int value) {

	if (!currentPlayer.isTimerItemGlobalRunning) {
		cout << "starting timerItemGlobal" << endl;
		currentPlayer.isTimerItemGlobalRunning = true;
		glutTimerFunc(TIMER_ITEM_GLOBAL, timerItemGlobal, 0);
	} else {
		cout << "stopping timerItemGlobal" << endl;
		currentPlayer.isTimerItemGlobalRunning = false;
	}
}

void timerMagicSpell(int value) {

	if (!currentPlayer.isTimerMagicSpellRunning) {
		cout << "starting timerMagicSpell" << endl;
		currentPlayer.isTimerMagicSpellRunning = true;
		glutTimerFunc(TIMER_MAGIC_SPELL, timerMagicSpell, 0);
	} else {
		cout << "stopping timerMagicSpell" << endl;
		currentPlayer.isTimerMagicSpellRunning = false;
	}
}

#endif
