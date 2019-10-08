package com.deskblast.scrape.protocol;


public class CompressedScreenScrape extends AbstractTileInfo{

    private static final long serialVersionUID = 1L;

    private byte[] ba;
    
    public CompressedScreenScrape(byte[] ba, int x, int y, 
    		int width, int height, String hash, int spriteX, int spriteY){
        super(x, y, width, height, hash, spriteX, spriteY);
    	this.ba = ba;
    }
    
    public byte[] getBytes(){
        return ba;
    }
    
    public int getByteCount(){
    	return ba.length;
    }

    public String toString(){
    	String strRet = super.toString();
    	strRet += " byte count: " + this.getByteCount();
    	return strRet;
    }
    
}