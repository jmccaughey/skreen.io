package com.deskblast.client;

public class RpcCompressedScreenScrapeSprite extends RpcAbstractTileInfoSprite{

    private static final long serialVersionUID = 1L;

    public transient byte[] bytes;
    
    public RpcCompressedScreenScrapeSprite(){}
    
    public RpcCompressedScreenScrapeSprite(byte[] ba, int x, int y, 
    		int width, int height, String hash, int spriteX, int spriteY){
        super(x, y, width, height, hash, spriteX, spriteY);
    	this.bytes = ba;
    }
//    public byte[] getBytes(){
//        return ba;
//    }
    public int getByteCount(){
    	return bytes.length;
    }
    public String toString(){
    	String strRet = super.toString();
    	strRet += " byte count: " + this.getByteCount();
    	return strRet;
    }
}