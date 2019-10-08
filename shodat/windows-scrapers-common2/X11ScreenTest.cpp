#include "X11Screen.h"
#include "../GifEncoder2/GifEncoder2.h"

#include <iostream>
#include <fstream>

using namespace std;

int main(int argc, char** argv) {
    X11Screen* screen = new X11Screen();
    screen->doScreenShot();
    int subWidth = deskblast::Region::TILE_WIDTH;
    int subHeight = deskblast::Region::TILE_HEIGHT;

    unsigned char * data = (unsigned char*) screen->getCurrentBuffer();
    GifEncoder2 gifEncoder(screen->getWidth(), screen->getHeight());
    unsigned char* out = (unsigned char*) malloc(1024 * 1024);
    int regionCount = 0;
    deskblast::Region** regions = deskblast::Region::allocateRegionsForBounds(
            screen->getWidth(), screen->getHeight(), &regionCount);
    printf("%d regions made\n", regionCount);

    for (int i = 0; i < regionCount; i++) {
        long result = gifEncoder.encode(regions[i], data, out, 1024 * 1024);
        delete regions[i];
        char index[256];
        sprintf(index, "%d", i);
        string path("out/");
        path += "image_";
        path += index;
        path += ".gif";
        printf("result: %d\n", result);
        if (result > 0) {
            ofstream file;
            file.open(path.c_str(), ios::out | ios::binary);
            if (file.is_open()) {
                file.write((const char*) out, result);
                file.close();
            }
        }
    }
    free(out);
    delete[] regions;
    delete screen;
    return 0;
}
