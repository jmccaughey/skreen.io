/*
 *  main.cpp
 *  CoreScraper
 *
 *  Created by John McCaughey on 8/27/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */

#include <PlatformScreen.h>

int main (int argc, const char * argv[]) {
	PlatformScreen* screen = new PlatformScreen();
	screen->doScreenShot();
	int mouseX; 
	int mouseY; 
	screen->updateMousePosition(&mouseX, &mouseY);
	delete(screen);
	printf("x: %d y: %d\n", mouseX, mouseY);

}

