#include <SFML/Audio.hpp>

using namespace sf;

//have to be global
Sound mainSound;
SoundBuffer mainBuffer;

//whenever an event happens, can use these
Sound eventSound;
SoundBuffer eventBuffer;

Music music;


void startMainSound() {
	int loaded = 1;

	//TODO gives error(writes on cmd line) if no file
	/*if (!mainBuffer.loadFromFile("data/sounds/pacman.wav"))
		loaded = -1;

	if (loaded) {
		//play now
		mainSound.setBuffer(mainBuffer);
		mainSound.setLoop(true);
		mainSound.play();

		//no need of while loop as glutMainLoop doesn't return
		while(1);

	}*/

	if (!music.openFromFile("data/sounds/mario_theme.ogg"))
	    loaded=-1; // error
	if(loaded)
	{
		music.setLoop(true);
		music.play();
		music.setLoop(true);
		//while(1);
	}
}
