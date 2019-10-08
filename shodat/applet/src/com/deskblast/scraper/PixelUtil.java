package com.deskblast.scraper;

import java.awt.Rectangle;
import java.awt.image.ImageProducer;
import java.awt.image.PixelGrabber;

public class PixelUtil{

    public static final int MAX_WIDTH   = 2000;//1024;
    public static final int MAX_HEIGHT  = 1500;//768;

    public static final int MIN_WIDTH   = 20;
    public static final int MIN_HEIGHT  = 20;

    public static final int DEFAULT_WIDTH   = 800;
    public static final int DEFAULT_HEIGHT  = 600;
    
    // 480x640 updated every 2000ms is 153.6 pixels per ms
    // 600x800 updated every 2000ms is 240 pixels per ms
    // 1024x768 "       "    2000ms is 393 "       "  "    
    public static final float f640_480_2   = (640  *   480)    / 2000;
    public static final float f800_600_2   = (800  *   600)    / 2000;
    public static final float f1024_768_2  = (1024 *   768)    / 2000;
           
    public static final float DEFAULT_PPMS = f1024_768_2; 
   

    public static final int SCRAPE_CHUNK_SIZE = 150;
    
	public static int[] getSubtilePixels(Rectangle rect, int[] pixels, int scansize){
		int[] rgb = new int[rect.width * rect.height];
		// TODO: compress directly via scanlines instead
		int start = rect.x + (rect.y * scansize);
		for(int i = 0; i < rect.height; i++){
			for(int j = 0; j < rect.width; j++){
				rgb[j+(i*rect.width)] = pixels[start+j]; 
			}
			start += scansize;
		}
		return rgb;
	}
    
    public static void getSubImagePixels(
    		ImageProducer mis,
    		int[] pixelsDest,
    		Rectangle rectDest
    )throws Exception{
    	
    	PixelGrabber grabber = new PixelGrabber(
    			mis, 
    			rectDest.x, 
    			rectDest.y,
    			rectDest.width,
    			rectDest.height,
    			pixelsDest,
    			0,
    			rectDest.width
    	);
    	grabber.grabPixels();    	
    }
    
    // 0xaarrggbb
//    public static byte[] downscalePixels(int[] pixels){//, boolean bRedFirst){
//        byte[] ba = new byte[pixels.length];
//        int red = 0;
//        int green = 0;
//        int blue = 0;
//        for(int i = 0; i < pixels.length; i++){
//            red = cm.getRed(pixels[i]);
//            green = cm.getGreen(pixels[i]);
//            blue = cm.getBlue(pixels[i]);
//            if(Math.abs(red-green) < 13
//                && Math.abs(red-blue) < 13
//                && Math.abs(green-blue) < 13){//((red == green) && (red == blue))
//                //  || 0 < almostGray(red, green, blue)){
//                ba[i] = (byte)128;
//                ba[i] |= green / 2;
//            }else{
//                ba[i] |= (/*bRedFirst ? red :*/ blue) / 64;
//                ba[i] |= (green / 64/*32*/)  << 2;
//                ba[i] |= ((/*bRedFirst ? blue :*/ red) / /*#64#*/32) << 4;
//            }
//        }
//        return ba;
//    }

//    public static void upscalePixels(byte[] pixels, int[] ia){
//    	upscalePixels(pixels, ia, 0);
////        for(int i = 0; i < pixels.length; i++){
////            ia[i] = 0xff000000;
////            if((pixels[i] & 128) != 0){
////                int grayVal = (pixels[i] & 127) * 2;
////                ia[i] |= grayVal;
////                ia[i] |= grayVal << 8;
////                ia[i] |= grayVal << 16;
////            }else{
////                int blue    = pixels[i] & 3;
////                int green   = (pixels[i] >> 2) & 3;
////                int red     = (pixels[i] >> 4) & /*#3#*/7;
////                ia[i] |= TABLE_TWO[blue];
////                ia[i] |= TABLE_TWO[green] << 8;
////                ia[i] |= TABLE_THREE/*#TABLE_TWO#*/[red] << 16;
////            }
////        }
//        //return ia;
//    }
//    
//    public static void upscalePixels(byte[] pixels, int[] ia, int iOffset){
//        for(int i = 0; i < pixels.length; i++){
//            ia[i+iOffset] = 0xff000000;
//            if((pixels[i] & 128) != 0){
//                int grayVal = (pixels[i] & 127) * 2;
//                ia[i+iOffset] |= grayVal;
//                ia[i+iOffset] |= grayVal << 8;
//                ia[i+iOffset] |= grayVal << 16;
//            }else{
//                int blue    = pixels[i] & 3;
//                int green   = (pixels[i] >> 2) & 3;
//                int red     = (pixels[i] >> 4) & /*#3#*/7;
//                ia[i+iOffset] |= TABLE_TWO[blue];
//                ia[i+iOffset] |= TABLE_TWO[green] << 8;
//                ia[i+iOffset] |= TABLE_THREE/*#TABLE_TWO#*/[red] << 16;
//            }
//        }
//        //return ia;
//    }

/*
    private static boolean almostGray(int iRed, int iGreen, int iBlue){
        return (Math.abs(iRed-iGreen) < 13
            && Math.abs(iRed-iBlue) < 13
            && Math.abs(iGreen-iBlue) < 13);


        /*
        int iMin = Math.min(iRed, Math.min(iGreen, iBlue));
        int iMax = Math.max(iRed, Math.max(iGreen, iBlue));
        if((iMax-iMin) > 12){
            return 0;
        }else{
            return iGreen;
        }
        */
//  }

/*
    public static byte[] downscalePixels(int[] pixels){
        byte[] ba = new byte[pixels.length];
        for(int i = 0; i < pixels.length; i++){
            int red = cm.getRed(pixels[i]);
            int green = cm.getGreen(pixels[i]);
            int blue = cm.getBlue(pixels[i]);
            if((red == green) && (red == blue)){
                ba[i] = (byte)128;
                ba[i] |= red / 2;
            }else{
                ba[i] |= blue / 64;
                ba[i] |= (green / 64)  << 2;
                ba[i] |= (red / 32) << 4;
            }
        }
        return ba;
    }

*/

/*
    private static int two2eight(int i){
        return TABLE_TWO[i];
    }

    private static int three2eight(int i){
        return TABLE_THREE[i];
    }
*/

    //private byte int2byte(int color){
    //}




/*    
    public static int scrapeIntervalMillis(int iWidth, int iHeight){
        return scrapeIntervalMillis(iWidth, iHeight, DEFAULT_PPMS);
    }
 */
    
//    public static int scrapeIntervalMillis(int iWidth, int iHeight, float fPPMS){
//        // 240 pixels per ms
//        int iRet = (int)((iWidth * iHeight) / fPPMS); 
//        iRet = Math.min(iRet, INTERVAL_MAX);
//        iRet = Math.max(iRet, INTERVAL_MIN);
//        return iRet;
//    }

/*

    public static byte[] downscalePixels(int[] pixels){
        byte[] ba = new byte[pixels.length];
        for(int i = 0; i < pixels.length; i++){
            int red = cm.getRed(pixels[i]);
            int green = cm.getGreen(pixels[i]);
            int blue = cm.getBlue(pixels[i]);
            ba[i] |= blue / 64;
            ba[i] |= (green / 36)  << 2;
            ba[i] |= (red / 36) << 5;
        }
        return ba;
    }

    public static int[] upscalePixels(byte[] pixels, int[] ia){
        for(int i = 0; i < pixels.length; i++){
            int blue    = pixels[i] & 3;
            int green   = (pixels[i] >> 2) & 7;
            int red     = (pixels[i] >> 5) & 7;
            ia[i] = 0xff000000;
            ia[i] |= TABLE_TWO[blue];
            ia[i] |= TABLE_THREE[green] << 8;
            ia[i] |= TABLE_THREE[red] << 16;
        }
        return ia;
    }




    public static byte[] downscalePixels(int[] pixels){
        byte[] ba = new byte[pixels.length];
        for(int i = 0; i < pixels.length; i++){
            int alpha = (pixels[i] >> 24) & 0xff;
            int red = (pixels[i] >> 16) & 0xff;
            int green = (pixels[i] >> 8) & 0xff;
            int blue = pixels[i] & 0xff;
            ba[i] = (byte)(alpha / 64 << 6);
            ba[i] |= red / 64 << 4;
            ba[i] |= green / 64  << 2;
            ba[i] |= blue / 64;
        }
        return ba;
    }

    public static int[] upscalePixels(byte[] pixels, int[] ia){
        //int[] ia = new int[pixels.length];
        for(int i = 0; i < pixels.length; i++){
            int alpha = (pixels[i] >> 6) & 0xff;
            int red = (pixels[i] >> 4) & 0xff;
            int green = (pixels[i] >> 2) & 0xff;
            int blue = pixels[i] & 0xff;
            ia[i] = (alpha * 64) << 24;
            ia[i] |= (red * 64) << 16;
            ia[i] |= (green * 64) << 8;
            ia[i] |= blue * 64;
        }
        return ia;
    }
*/


}
