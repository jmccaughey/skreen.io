//
//  Scaler.cpp
//  SwscaleTest
//
//  Created by John McCaughey on 1/1/14.
//  Copyright (c) 2014 John McCaughey. All rights reserved.
//

#include "Scaler.h"

Scaler::Scaler(int beforeWidth, int beforeHeight, bool smooth,
               AVPixelFormat pixelFormat, bool grayscale){
        _beforeWidth = beforeWidth;
        _beforeHeight = beforeHeight;
        _afterWidth = beforeWidth / 2;
        _afterHeight = beforeHeight / 2;
        _pixelFormat = pixelFormat;
        _grayscale = grayscale;
        _resize = ::sws_getContext(beforeWidth, beforeHeight, pixelFormat,
                                  _afterWidth, _afterHeight,
                                  _grayscale ? AV_PIX_FMT_GRAY8 : pixelFormat,
                                  smooth ?
                                    SWS_BICUBIC // prettier
                                    :
                                    SWS_POINT // nearest neighbor (faster)
                                  //SWS_FAST_BILINEAR // looks better than SWS_POINT
                                  //SWS_X
                                  //SWS_AREA
                                  //SWS_BITEXACT
                                  // TODO: try SWS_AREA and SWS_BITEXACT
                                  //
                                  //SWS_FAST_BILINEAR// SWS_BILINEAR // faster
                                  ,
                                  NULL, NULL, NULL);
        _frameBefore = av_frame_alloc();//avcodec_alloc_frame();
        _frameAfter = av_frame_alloc();//avcodec_alloc_frame();
    }

Scaler::~Scaler(){
    if(_frameBefore){
        av_frame_free(&_frameBefore);
    }
    if(_frameAfter){
        av_frame_free(&_frameAfter);
    }
    if(_resize){
        sws_freeContext(_resize);
    }
}

unsigned int Scaler::getScaledBufferSize(){
    // TODO: for grayscale the buffer should be smaller
    return _afterWidth * _afterHeight //* Scaler::BYTES_PER_PIXEL;
            * (_grayscale ? 1 : Scaler::BYTES_PER_PIXEL);
}

void Scaler::scale(uint8_t* before, uint8_t* after){
    avpicture_fill((AVPicture*)_frameBefore, before, _pixelFormat, _beforeWidth,
                   _beforeHeight);
    avpicture_fill((AVPicture*)_frameAfter, after,
                   _grayscale ? AV_PIX_FMT_GRAY8 : _pixelFormat,
                   _afterWidth,
                   _afterHeight);
    sws_scale(_resize, _frameBefore->data, _frameBefore->linesize, 0, _beforeHeight,
              _frameAfter->data, _frameAfter->linesize);
}
