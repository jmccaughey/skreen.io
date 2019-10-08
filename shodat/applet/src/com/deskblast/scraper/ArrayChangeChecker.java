package com.deskblast.scraper;

import java.awt.Rectangle;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;

import javax.imageio.ImageIO;

public class ArrayChangeChecker {
		
	public static boolean dataChanged(Rectangle rect, int[] iaOld, int[] iaNew, int scansize) {
		if(iaNew.length != iaOld.length) return true;
		int start = rect.x + (rect.y * scansize);
		for(int y = 0; y < rect.height; y++){
			for(int x = 0; x < rect.width; x++){
				if(iaOld[start+x] != iaNew[start+x]){
					return true;
				}
			}
			start += scansize;
		}
		return false;
	}

	public static boolean dataChanged(ScanRectangle rect, int[] ia, 
			int[] ib) {
		if(ia.length != ib.length) return true;
		for(int i = 0; i < rect.height; i++){
			int index = rect.getScanline(i);
			for(int j = 0; j < rect.width; j++){
				if(ia[index+j] != ib[index+j]){
					return true;
				}
			}
		}
		return false;
	}
	
	public static void main(String[] args) throws IOException{
		BufferedImage biA = ImageIO.read(new File("../reference_desktops/Screenshot_1.png"));
		BufferedImage biB = ImageIO.read(new File("../reference_desktops/Screenshot_2.png"));
		int width = biA.getWidth();
		int height = biA.getHeight();
		int[] a = new int[width * height];
		int[] b = new int[biB.getWidth() * biB.getHeight()];
		biA.getRGB(0, 0, width, height, a, 0, width);
		biB.getRGB(0, 0, biB.getWidth(), biB.getHeight(), b, 0, biB.getWidth());
		
		Rectangle r1 = new Rectangle(0, 0, 38, 38);
		Rectangle r2 = new Rectangle(1000, 350, 36, 36);
		Rectangle r3 = new Rectangle(0, 0, width, height);
		System.out.println(dataChanged(r1, a, b, width));
		System.out.println(dataChanged(r2, a, b, biA.getWidth()));
		System.out.println(dataChanged(r3, a, b, biA.getWidth()));
		
		ScanRectangle rectA = new ScanRectangle(r1, biA.getWidth());
		ScanRectangle rectB = new ScanRectangle(r2, biA.getWidth());
		ScanRectangle rectC = new ScanRectangle(r3, biA.getWidth());
		System.out.println(dataChanged(rectA, a, b));
		System.out.println(dataChanged(rectB, a, b));
		System.out.println(dataChanged(rectC, a, b));
	}
}
