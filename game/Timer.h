#ifndef TIMER_H
#define TIMER_H

#define TIMER_ITEM_GLOBAL 8000
#define TIMER_MAGIC_SPELL 12000
#define TIMER_CURSE 5000

void timerRefresh(int value) {

	glutPostRedisplay();
	glutTimerFunc(SPEED_MAX - players[playerId].speedMove + 100, timerRefresh, 0); //'100' added because we don't want refresh rate to be 0
}

void timerItemGlobal(int value) {

	if (!players[playerId].isTimerItemGlobalRunning) {
		cout << "starting timerItemGlobal" << endl;
		players[playerId].isTimerItemGlobalRunning = true;
		glutTimerFunc(TIMER_ITEM_GLOBAL, timerItemGlobal, 0);
	} else {
		cout << "stopping timerItemGlobal" << endl;
		players[playerId].isTimerItemGlobalRunning = false;
	}
}

//TODO : to be called when actually using magic power on hero (not implemented yet)
void timerMagicSpell(int value) {

	if (!players[playerId].isTimerMagicSpellRunning) {
		cout << "starting timerMagicSpell" << endl;
		players[playerId].isTimerMagicSpellRunning = true;
		players[playerId].currentPowerMode = POWER_MODE_BASIC;
		glutTimerFunc(TIMER_MAGIC_SPELL, timerMagicSpell, 0);
	} else {
		cout << "stopping timerMagicSpell" << endl;
		players[playerId].isTimerMagicSpellRunning = false;
	}
}

void timerCurse(int value) {

	if (!players[playerId].isTimerCurseRunning) {
		cout << "starting timerCurse" << endl;

		switch (value) {
		case CURSE_STUN:
			players[playerId].speedMoveTemp = players[playerId].speedMove;
			players[playerId].speedMove = 0;
			players[playerId].currentPowerMode = POWER_MODE_STUN;
			break;
		case CURSE_DISABLE:
			//Nothing to be done over here
			break;
		case CURSE_SLOW:
			players[playerId].strength -= CURSE_AMT_SLOW_STRENGTH;
			break;
		case CURSE_BURST:
			//Nothing to be done over here
			break;
		}

		players[playerId].isTimerCurseRunning = true;
		glutTimerFunc(TIMER_CURSE, timerCurse, value);
	}

	else {
		cout << "stopping timerCurse" << endl;
		players[playerId].isTimerCurseRunning = false;

		switch (value) {
		case CURSE_STUN:
			players[playerId].currentPowerMode = POWER_MODE_BASIC;
			players[playerId].speedMove += players[playerId].speedMoveTemp;
			if (players[playerId].speedMove > SPEED_MAX) {
				players[playerId].speedMove = SPEED_MAX;
			}
			break;
		case CURSE_DISABLE:
			//Nothing to be done over here
			break;
		case CURSE_SLOW:
			players[playerId].strength += CURSE_AMT_SLOW_STRENGTH;
			break;
		case CURSE_BURST:
			//Nothing to be done over here
			break;
		}

		players[playerId].curseType = CURSE_NONE;
	}
}

#endif
