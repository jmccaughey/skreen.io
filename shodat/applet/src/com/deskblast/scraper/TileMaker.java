package com.deskblast.scraper;

import java.awt.Dimension;
import java.awt.Rectangle;

public class TileMaker {
	
	public static Rectangle[] makeTiles(Dimension dim, int tileWidth, int tileHeight){
		int iWidth = dim.width;//300;//rect.width / 2;
		int iHeight = dim.height;//300;//rect.width / 2;
		
		int iRightRun = iWidth / tileWidth;
		int iExtraRight = iWidth % tileWidth;
		iRightRun += iExtraRight > 0 ? 1 : 0;
		
		int iDownRun = iHeight / tileHeight;
		int iExtraDown = iHeight % tileHeight;
		iDownRun += iExtraDown > 0 ? 1 : 0;
		
		Rectangle[] rects = new Rectangle[iRightRun * iDownRun];
		
		int iRectCount = 0;
		
		for(int iRow = 0; iRow < iDownRun; iRow++){
			for(int iColumn = 0; iColumn < iRightRun; iColumn++){
				rects[iRectCount] = new Rectangle(
						iColumn*tileWidth,
						iRow*tileHeight,
						tileWidth,
						tileHeight
				);
				if(iColumn+1 == iRightRun && iExtraRight > 0){
					// this is an extra, filler rect on right
					//System.out.println("doing filler rect on right");
					rects[iRectCount].width = iExtraRight;
				}
				if(iRow+1 == iDownRun && iExtraDown > 0){
					// this is an extra, filler rect on bottom
					//System.out.println("doing filler rect on bottom");
					rects[iRectCount].height = iExtraDown;
				}
				iRectCount++;
			}
		}
		return rects;
	}
	
	public static void main(String[] args){
		int iSide = 150;
		int iWidth = 800;
		int iHeight = 600;
		
		if(args.length > 0){
			iWidth = Integer.parseInt(args[0]);
			iHeight = Integer.parseInt(args[1]);
			iSide = Integer.parseInt(args[2]);
		}		
		
		Rectangle[] rects = TileMaker.makeTiles(
				new Dimension(iWidth, iHeight), 
				256, 128
		);
		System.out.println(rects.length + " rectangles.");
		for(int i = 0; i < rects.length; i++){
			System.out.println(rects[i]);
		}
	}
	
}
