#include "PlatformScreen.h"

int main (int argc, const char * argv[]) {
	PlatformScreen* screen = new PlatformScreen();
	screen->doScreenShot();
	int mouseX; 
	int mouseY; 
	screen->updateMousePosition(&mouseX, &mouseY);
	delete(screen);
	printf("x: %d y: %d\n", mouseX, mouseY);

}

