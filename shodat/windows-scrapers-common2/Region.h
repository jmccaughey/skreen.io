#ifndef REGION_H
#define REGION_H

#include <string.h>

#include <vector>

//#include "windows.h"

namespace deskblast{

class Region
{
public:
	const static int TILE_WIDTH = 256;
	const static int TILE_HEIGHT = 180;
	const static int MAX_MERGED_TILES = 5;
	const static int BYTES_PER_PIXEL =
#if defined(_WINDOWS)
    4
#else
    4
#endif
; // TODO: fixme bytes per pixel was 3
	//const static int BYTES_PER_SIDE = TILE_SIDE * BYTES_PER_PIXEL;
	Region(void);
	Region(int X, int Y, int width, int height, int desktopWidth, int start, int bytesPerPixel = BYTES_PER_PIXEL)
		: mX(X), mY(Y), mWidth(width), mHeight(height), mDesktopWidth(desktopWidth), mStart(start), changed(true),
        mBytesPerPixel(bytesPerPixel){};
	//Region(const Region& r1, const Region& r2);
	~Region(void);
	//Region(const Region &other);
	//Region & operator=(const Region &rhs);
	int getX(void);
	int getY(void);
	int getWidth(void);
	int getHeight(void);
	int getArea(void);
	unsigned char* getScanline(int row, void * lpvpxldata);
	bool isChanged(void);
	bool checkForChange(void * lpvpxldataA, void * lpvpxldataB);
	bool canMerge(Region & other, int maxArea);
	void grow(const Region & other);
	static void doMergeRegions(std::vector<Region> & dest, int maxArea);
	bool operator <(const Region& rhs) const;
	bool operator ==(const Region& rhs) const;
	bool contains(const Region & other);
	static Region** allocateRegionsForBounds(int width, int height, int* count,
                                             bool grayscale = false);
private:
	int mX;
	int mY;
	int mWidth;
	int mHeight;
	int mDesktopWidth;
	int mStart;
	bool changed;
    int mBytesPerPixel;
	//static void doMergeRegions(std::vector<Region> & dest);
};
}
#endif /* REGION_H */
