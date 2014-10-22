#include <SFML/Audio.hpp>
#include "FilePaths.h"

using namespace sf;

Music bgMusic;
Music eventMusic;

void playBgSound() {
	int loaded = 1;

	if (!bgMusic.openFromFile(PATH_SOUND_MARIO_THEME)) {
		loaded = -1; // error
	}

	if (loaded) {
		bgMusic.setLoop(true);
		bgMusic.play();
	}
}

void playEventSound(std::string eventSoundPath) {
	bgMusic.pause();

	int loaded = 1;

	if (!eventMusic.openFromFile(eventSoundPath)) {
		loaded = -1; // error
	}

	if (loaded) {
		eventMusic.play();
	}

	bgMusic.play();
}
