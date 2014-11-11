#ifndef AI_H
#define AI_H

class AI {
	int id;
	pthread_t threadId;

public:
	AI(int id) {
		this->id = id;
	}

	int getId() {
		return this->id;
	}

	void attackEnemyTemple() {
		if (getId() == 1 && players[getId()].team->name == TEAM_ANGELS) {
			processGeneral("t:16,18?1");
		}

		else if (getId() == 1 && players[getId()].team->name == TEAM_DEMONS) {
			processGeneral("t:4,6?1");
		}

		if (getId() == 2 && players[getId()].team->name == TEAM_ANGELS) {
			processGeneral("t:16,18?2");
		}

		else if (getId() == 2 && players[getId()].team->name == TEAM_DEMONS) {
			processGeneral("t:4,6?2");
		}

		if (getId() == 3 && players[getId()].team->name == TEAM_ANGELS) {
			processGeneral("t:17,19?3");
		}

		else if (getId() == 3 && players[getId()].team->name == TEAM_DEMONS) {
			processGeneral("t:5,7?3");
		}
	}

	void running() {
		while (!gameDetails.isGameOver) {

			attackEnemyTemple();
			sleep(1);
		}
	}

};

pthread_t aiThreadId1;
pthread_t aiThreadId2;
pthread_t aiThreadId3;

AI* ai1;
AI* ai2;
AI* ai3;

void* startMe1(void*) {
	ai1->running();
	return NULL;
}

void* startMe2(void*) {
	ai2->running();
	return NULL;
}

void* startMe3(void*) {
	ai3->running();
	return NULL;
}

void createAIThread1() {
	createThread(&aiThreadId1, startMe1);
}

void createAIThread2() {
	createThread(&aiThreadId2, startMe2);
}

void createAIThread3() {
	createThread(&aiThreadId3, startMe3);
}

#endif
