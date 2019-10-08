package com.deskblast.scraper;

import java.awt.Rectangle;

public class RawScrape extends Rectangle{
    
    private static final long serialVersionUID = 1L;
    private int[] pixels;
	private int iIndex;
	
	public RawScrape(Rectangle rect, int[] ia, int iIndex){
		super(rect);
		this.pixels = ia;
		this.iIndex = iIndex;
	}
	
	public int[] getPixels(){
		return this.pixels;
	}
	
	public int getIndex(){
		return this.iIndex;
	}
	
}
