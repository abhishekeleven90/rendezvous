#include <SFML/Audio.hpp>

using namespace sf;

Music music;
Music eventMusic;

void startMainSound() {
	int loaded = 1;

	if (!music.openFromFile("data/sounds/mario_theme.ogg"))
		loaded = -1; // error
	if (loaded) {
		music.setLoop(true);
		music.play();
		//while(1);
	}
}

void playEventSound() {

	int loaded = 1;
	if (!eventMusic.openFromFile("data/sounds/event.wav"))
		loaded = -1; // error
	if (loaded) {
		eventMusic.play();
	}
}
