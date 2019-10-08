#include "Region.h"

namespace deskblast{

int Region::getX(){	return mX; }
int Region::getY() { return mY; }
int Region::getWidth() { return mWidth; }
int Region::getHeight() { return mHeight; }

unsigned char* Region::getScanline(int row, void *lpvpxldata){
	//int bytesPerPixel = BYTES_PER_PIXEL;
//#ifdef BOTTOM_UP_SCRAPES
//	row = getHeight() - (1 + row);
//#endif	
	int rowStartIndex = (row * mDesktopWidth * mBytesPerPixel)
		+ mStart;
	unsigned char *bytes = (unsigned char*)lpvpxldata;
	return &bytes[rowStartIndex];
}

bool Region::checkForChange(void* lpvpxldataA, void* lpvpxldataB){
    if(lpvpxldataA == NULL || lpvpxldataB == NULL){
    	this->changed = true;
        return true;
    }
	for(int i = 0; i < mHeight; i++){
		// TODO: should alpha channed be ignored?
		int result = memcmp(getScanline(i, lpvpxldataA),
			getScanline(i, lpvpxldataB),
			mBytesPerPixel * mWidth);
		if(result != 0){
			this->changed = true;
			return true;
		}
	}
	this->changed = false;
	return false;
}
bool Region::isChanged(void){
	return this->changed;
}
Region** Region::allocateRegionsForBounds(int screenWidth, int screenHeight, int* count, bool grayscale){
    int xTiles = screenWidth / Region::TILE_WIDTH;
    int yTiles = screenHeight / Region::TILE_HEIGHT;
    bool xXtra = false;
    bool yXtra = false;
    int bytesPerPixel = grayscale ? 1 : BYTES_PER_PIXEL;
    if (screenWidth % Region::TILE_WIDTH != 0) {
        xTiles++;
        xXtra = true;
    }
    if (screenHeight % Region::TILE_HEIGHT != 0) {
        yTiles++;
        yXtra = true;
    }
    int regionCount = xTiles * yTiles;
    printf("region count %d x:%d y:%d\n", regionCount, xTiles, yTiles);
    Region** regions = new Region*[regionCount];
    printf("returned from new Region*\n");
    *count = regionCount;
    int regionCounter = 0;
//#ifdef BOTTOM_UP_SCRAPES
//    for (int yIndex = yTiles-1; yIndex >= 0; yIndex--) {
//#elif
    for (int yIndex = 0; yIndex < yTiles; yIndex++) {
//#endif        
		int height = Region::TILE_HEIGHT;
        if (yXtra && yIndex == (yTiles - 1)) {
            height = screenHeight % Region::TILE_HEIGHT;
        }
        for (int xIndex = 0; xIndex < xTiles; xIndex++) {
            int width = Region::TILE_WIDTH;
            if (xXtra && xIndex == (xTiles - 1)) {
                width = screenWidth % Region::TILE_WIDTH;
            }
            regions[regionCounter] = new Region(Region::TILE_WIDTH
                    * xIndex, Region::TILE_HEIGHT * yIndex, width, height,
                    screenWidth, (((bytesPerPixel*Region::TILE_HEIGHT) * screenWidth)
                                  * yIndex) + (xIndex * (bytesPerPixel*Region::TILE_WIDTH)),
                                                bytesPerPixel);
            printf("returned from new Region\n");
            regionCounter++;
        }
    }
    return regions;
}
/*
Region::Region(const Region& r1, const Region& r2){
	// merge two regions together
	this->mStart = r1.mStart < r2.mStart ? r1.mStart : r2.mStart;
	this->mWidth = r1.mX == r2.mX ? r1.mWidth : r1.mWidth + r2.mWidth;
	this->mHeight = r1.mX == r2.mX ? r1.mHeight + r2.mHeight : r1.mHeight; 
	this->mX = r1.mX < r2.mX ? r1.mX : r2.mX;
	this->mY = r1.mY < r2.mY ? r1.mY : r2.mY;
	this->changed = r1.changed || r2.changed;
	this->mDesktopWidth = r1.mDesktopWidth;
}
*/
Region::Region(void)
{
	this->changed = true;
}
/*
Region::Region(const Region &other){
	this->mStart = other.mStart;
	this->mWidth = other.mWidth;
	this->mHeight = other.mHeight; 
	this->mX = other.mX;
	this->mY = other.mY;
	this->changed = other.changed;
	this->mDesktopWidth = other.mDesktopWidth;
}
*/
Region::~Region(void)
{
}
/*
Region& Region::operator=(const Region & rhs){
	this->mStart = rhs.mStart;
	this->mWidth = rhs.mWidth;
	this->mHeight = rhs.mHeight; 
	this->mX = rhs.mX;
	this->mY = rhs.mY;
	this->changed = rhs.changed;
	this->mDesktopWidth = rhs.mDesktopWidth;
	return *this;
}
*/
bool Region::canMerge(Region & other, int maxArea){
	if(this->mX == other.mX){
		if(this->mY + this->mHeight == other.mY
				|| other.mY + other.mHeight == this->mY){
			if(this->mWidth == other.mWidth){
				if(this->getArea() + other.getArea() <= maxArea){
					return true;
				}	
			}
		}
	}else if(this->mY == other.mY){
		if(this->mX + this->mWidth == other.mX
				|| other.mX + other.mWidth == this->mX){
			if(this->mHeight == other.mHeight){
				if(this->getArea() + other.getArea() <= maxArea){
					return true;
				}	
			}
		}
	}
	return false;
}
void Region::grow(const Region & other){
	this->mStart = this->mStart < other.mStart ? this->mStart : other.mStart;
	this->mWidth = this->mX == other.mX ? this->mWidth : this->mWidth + other.mWidth;
	this->mHeight = this->mX == other.mX ? this->mHeight + other.mHeight : this->mHeight; 
	this->mX = this->mX < other.mX ? this->mX : other.mX;
	this->mY = this->mY < other.mY ? this->mY : other.mY;
	this->changed = this->changed || other.changed;
}
/*
void Region::mergeRegions(Region * regions, std::vector<Region> & dest, int count, bool force){
	for(int i = 0; i < count; i++){
		if(force || (regions+i)->isChanged()){
			dest.push_back((const Region &)(regions[i]));
		}	
	} 
	doMergeRegions(dest);
}
*/
int Region::getArea(void){
	return mWidth * mHeight;
}
void Region::doMergeRegions(std::vector<Region> & dest, int maxArea){
	// if any can be merged, merge and call back into this
	for(unsigned int i = 0; i < dest.size(); i++){			
		for(unsigned int j = i+1; j < dest.size(); j++){
			if(dest[i].canMerge(dest[j], maxArea)){
				//std::cout << "merging" << std::endl;					
				//boost::geometry::expand(dest[i], dest[j]);
				//boost::polygon::intersect(dest[i], dest[j]);
				dest[i].grow(dest[j]);
				dest.erase(dest.begin()+j);
				doMergeRegions(dest, maxArea);
			}
		}
	}
}
bool Region::operator <(const Region& rhs) const{
	if(this->mY < rhs.mY) return true;
	if(this->mY > rhs.mY) return false;
	// mY == mY
	if(this->mX < rhs.mX) return true;
	if(this->mX > rhs.mX) return false;
	// mX == mX
	return false;
}
bool Region::operator ==(const Region& rhs) const{
    if(     this->mY == rhs.mY
        &&  this->mX == rhs.mX
        &&  this->mWidth == rhs.mWidth
        &&  this->mHeight == rhs.mHeight){
        return true;
    }
    return false;
}

bool Region::contains(const Region & other){
	return other.mX >= this->mX
		&& other.mY >= this->mY
		&& (other.mX + other.mWidth) <= (this->mX + this->mWidth)
		&& (other.mY + other.mHeight) <= (this->mY + this->mHeight);
}
}
