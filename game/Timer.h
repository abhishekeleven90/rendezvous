#ifndef TIMER_H
#define TIMER_H

#define TIMER_ITEM_GLOBAL 8000
#define TIMER_MAGIC_SPELL 12000
#define TIMER_CURSE 5000

void timerRefresh(int value) {

	glutPostRedisplay();
	glutTimerFunc(currPlayer.speedMove, timerRefresh, 0);
}

void timerItemGlobal(int value) {

	if (!currPlayer.isTimerItemGlobalRunning) {
		cout << "starting timerItemGlobal" << endl;
		currPlayer.isTimerItemGlobalRunning = true;
		glutTimerFunc(TIMER_ITEM_GLOBAL, timerItemGlobal, 0);
	} else {
		cout << "stopping timerItemGlobal" << endl;
		currPlayer.isTimerItemGlobalRunning = false;
	}
}

//TODO : to be called when actually using magic power on hero (not implemented yet)
void timerMagicSpell(int value) {

	if (!currPlayer.isTimerMagicSpellRunning) {
		cout << "starting timerMagicSpell" << endl;
		currPlayer.isTimerMagicSpellRunning = true;
		currPlayer.currentPowerMode = POWER_MODE_BASIC;
		glutTimerFunc(TIMER_MAGIC_SPELL, timerMagicSpell, 0);
	} else {
		cout << "stopping timerMagicSpell" << endl;
		currPlayer.isTimerMagicSpellRunning = false;
	}
}

void timerCurse(int value) {

	if (!currPlayer.isTimerCurseRunning) {
		cout << "starting timerCurse" << endl;

		switch (value) {
		case CURSE_STUN:
			//currentPlayer.s
			break;
		case CURSE_DISABLE:
			//Nothing to be done over here
			break;
		case CURSE_SLOW:
			currPlayer.strength -= CURSE_AMT_SLOW_STRENGTH;
			break;
		case CURSE_BURST:
			//Nothing to be done over here
			break;
		}

		currPlayer.isTimerCurseRunning = true;
		glutTimerFunc(TIMER_CURSE, timerCurse, value);
	}

	else {
		cout << "stopping timerCurse" << endl;
		currPlayer.isTimerCurseRunning = false;

		switch (value) {
		case CURSE_STUN:
			break;
		case CURSE_DISABLE:
			//Nothing to be done over here
			break;
		case CURSE_SLOW:
			currPlayer.strength += CURSE_AMT_SLOW_STRENGTH;
			break;
		case CURSE_BURST:
			//Nothing to be done over here
			break;
		}

		currPlayer.curseType = CURSE_NONE;
	}
}

#endif
