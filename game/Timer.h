#ifndef TIMER_H
#define TIMER_H

#define TIMER_ITEM_GLOBAL 8000
#define TIMER_MAGIC_SPELL 12000
#define TIMER_CURSE 5000

#define REFRESH_RATE 100

void timerRefresh(int value) {
	glutPostRedisplay();
	glutTimerFunc(REFRESH_RATE, timerRefresh, 0);
}

void timerItemGlobal(int whichPlayer) {
	if (!players[whichPlayer].isTimerItemGlobalRunning) {
		cout << "starting timerItemGlobal for player: " << whichPlayer << endl;
		players[whichPlayer].isTimerItemGlobalRunning = true;
		glutTimerFunc(TIMER_ITEM_GLOBAL, timerItemGlobal, whichPlayer);
	} else {
		cout << "stopping timerItemGlobal for player: " << whichPlayer << endl;
		players[whichPlayer].isTimerItemGlobalRunning = false;
	}
}

//TODO : to be called when actually using magic power on hero (not implemented yet)
void timerMagicSpell(int value) {

	if (!players[currPlayerId].isTimerMagicSpellRunning) {
		cout << "starting timerMagicSpell" << endl;
		players[currPlayerId].isTimerMagicSpellRunning = true;
		players[currPlayerId].currPowerMode = POWER_MODE_BASIC;
		glutTimerFunc(TIMER_MAGIC_SPELL, timerMagicSpell, 0);
	} else {
		cout << "stopping timerMagicSpell" << endl;
		players[currPlayerId].isTimerMagicSpellRunning = false;
	}
}

void timerCurse(int value) {

	if (!players[currPlayerId].isTimerCurseRunning) {
		cout << "starting timerCurse" << endl;

		switch (value) {
		case CURSE_STUN:
			players[currPlayerId].speedMoveTemp
					= players[currPlayerId].speedMove;
			players[currPlayerId].speedMove = 0;
			players[currPlayerId].currPowerMode = POWER_MODE_STUN;
			break;
		case CURSE_DISABLE:
			//Nothing to be done over here
			break;
		case CURSE_SLOW:
			players[currPlayerId].strength -= CURSE_AMT_SLOW_STRENGTH;
			break;
		case CURSE_BURST:
			//Nothing to be done over here
			break;
		}

		players[currPlayerId].isTimerCurseRunning = true;
		glutTimerFunc(TIMER_CURSE, timerCurse, value);
	}

	else {
		cout << "stopping timerCurse" << endl;
		players[currPlayerId].isTimerCurseRunning = false;

		switch (value) {
		case CURSE_STUN:
			players[currPlayerId].currPowerMode = POWER_MODE_BASIC;
			players[currPlayerId].speedMove
					+= players[currPlayerId].speedMoveTemp;
			if (players[currPlayerId].speedMove > SPEED_MAX) {
				players[currPlayerId].speedMove = SPEED_MAX;
			}
			break;
		case CURSE_DISABLE:
			//Nothing to be done over here
			break;
		case CURSE_SLOW:
			players[currPlayerId].strength += CURSE_AMT_SLOW_STRENGTH;
			break;
		case CURSE_BURST:
			//Nothing to be done over here
			break;
		}

		players[currPlayerId].curseType = CURSE_NONE;
	}
}

#endif
