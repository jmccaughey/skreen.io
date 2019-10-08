package com.deskblast.scraper;

import java.awt.Rectangle;

public class ScanRectangle extends Rectangle {

	private int scanline;

	public ScanRectangle(int x, int y, int width, int height, int scanline) {
		super(x, y, width, height);
		this.scanline = scanline;
	}
	
	public ScanRectangle(Rectangle rect, int scanline){
		this(rect.x, rect.y, rect.width, rect.height, scanline);
	}
	
	public int getScanline(int index){
		int start = x + ((index + y) * scanline);
		return start;
	}
	
	
	
}
