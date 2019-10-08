//
//  main.cpp
//  PngTest
//
//  Created by John McCaughey on 1/8/14.
//  Copyright (c) 2014 John McCaughey. All rights reserved.
//

//#include <stdio.h>

#include "PngFileReader.h"
#include "PngFileWriter.h"
#include "Scaler.h"


int main(int argc, char **argv)
{
    //if (argc != 3)
    //    abort_("Usage: program_name <file_in> <file_out>");
    // Scaler * scaler = new Scaler();
    
    
    PngFileReader* pngFileReader = new PngFileReader("/Users/jmccaughey/Desktop/Screen Shot 2014-02-12 at 5.19.16 PM.png");
    printf("%d x %d\n", pngFileReader->getWidth(), pngFileReader->getHeight());
    unsigned char * data = pngFileReader->getBuffer();
    Scaler* scaler = new Scaler((int)(pngFileReader->getWidth()),
                (int)(pngFileReader->getHeight()), false,
                                PIX_FMT_RGB24
                                );
    unsigned char* after = (unsigned char *)malloc(scaler->getScaledBufferSize());
    scaler->scale(data, after);
    PngFileWriter* pngFileWriter = new PngFileWriter("/Users/jmccaughey/Desktop/Screen Shot 2014-02-12 at 5.19.16 PM.out.half.new.png",
                                                     after,//pngFileReader->getBuffer(),
                                                     pngFileReader->getWidth()/2,
                                                     pngFileReader->getHeight()/2,
                                                     pngFileReader->getColorType(),
                                                     pngFileReader->getBitDepth()
                                                     );
    
    delete scaler;
    scaler = new Scaler((int)(pngFileReader->getWidth()),
                        (int)(pngFileReader->getHeight()), false,
                        PIX_FMT_RGB24,
                        true
                        );
    
    free(after);
    after = (unsigned char *)malloc(scaler->getScaledBufferSize());
    scaler->scale(data, after);
    pngFileWriter = new PngFileWriter("/Users/jmccaughey/Desktop/Screen Shot 2014-02-12 at 5.19.16 PM.out.half.gray.png",
                                                     after,//pngFileReader->getBuffer(),
                                                     pngFileReader->getWidth()/2,
                                                     pngFileReader->getHeight()/2,
                                                     0,//pngFileReader->getColorType(),
                                                     8//pngFileReader->getBitDepth()
                                                     );
    
    
    //read_png_file("/Users/jmccaughey/Desktop/Screen Shot 2014-01-06 at 7.31.25 AM.png");//argv[1]);
    //process_file();
    //write_png_file("/Users/jmccaughey/Desktop/out_file.png");
    
    return 0;
}
