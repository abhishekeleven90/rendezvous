#include <SFML/Audio.hpp>

using namespace sf;

//have to be global
Sound mainSound;
SoundBuffer mainBuffer;

//whenever an event happens, can use these
Sound eventSound;
SoundBuffer eventBuffer;

Music mainMusic;

void startMainSound() {
	int loaded = 1;

	//TODO gives error(writes on cmd line) if no file
	if (!mainBuffer.loadFromFile("data/sounds/bg_sound.wav"))
		loaded = -1;

	if (loaded) {
		//play now
		mainSound.setBuffer(mainBuffer);
		mainSound.play();
		mainSound.setLoop(true);

		//no need of while loop as glutMainLoop doesn't return
		//while(1);
	}
}
