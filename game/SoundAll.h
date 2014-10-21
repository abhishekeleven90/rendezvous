#include <SFML/Audio.hpp>
#include "FilePaths.h"

using namespace sf;

Music music;
Music eventMusic;

void playSound(std::string soundPath, bool setLoop) {
	int loaded = 1;
	if (!music.openFromFile(soundPath)) {
		loaded = -1; // error
	}
	if (loaded) {
		music.setLoop(setLoop);
		music.play();
	}
}

void startMainSound() {
	playSound(PATH_SOUND_MARIO_THEME, true);
}

void playEventSound() {
	playSound(PATH_SOUND_EVENT, false);
}
