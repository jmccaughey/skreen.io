package com.deskblast.client;

public class RpcCompressedScreenScrape extends RpcAbstractTileInfo{

    private static final long serialVersionUID = 1L;

    public transient byte[] bytes;
    
    public RpcCompressedScreenScrape(){}
    
    public RpcCompressedScreenScrape(byte[] ba, int x, int y, String hash){
        super(x, y, hash);
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