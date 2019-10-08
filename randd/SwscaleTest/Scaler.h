//
//  Scaler.h
//  SwscaleTest
//
//  Created by John McCaughey on 1/1/14.
//  Copyright (c) 2014 John McCaughey. All rights reserved.
//

//#include <iostream>
extern "C" {
#include "libavutil/mathematics.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
}

#define PIXEL_FORMAT AV_PIX_FMT_BGRA

class Scaler{
public:
    Scaler(int beforeWidth, int beforeHeight, bool smooth, AVPixelFormat pixelFormat = PIXEL_FORMAT, bool grayscale = false);
    ~Scaler();
    unsigned int getScaledBufferSize();
    void scale(uint8_t* before, uint8_t* after);
    static const int BYTES_PER_PIXEL = 4;
private:
    struct SwsContext *_resize;
    AVPixelFormat _pixelFormat;
    int _beforeWidth;
    int _beforeHeight;
    int _afterWidth;
    int _afterHeight;
    AVFrame* _frameBefore;
    AVFrame* _frameAfter;
    bool _grayscale;
};