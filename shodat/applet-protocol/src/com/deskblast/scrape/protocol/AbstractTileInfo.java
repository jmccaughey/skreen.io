package com.deskblast.scrape.protocol;


public abstract class AbstractTileInfo {

    private static final long serialVersionUID = 1L;

    private String hash;
    
    public int x;
    public int y;
    public int width;
    public int height;
    public int spriteX;
    public int spriteY;
    
    public AbstractTileInfo(int x, int y, int width, int height, 
    		String hash, int spriteX, int spriteY){
        this.x = x;
        this.y = y;
        this.width = width;
        this.height = height;
        this.hash = hash;
        this.spriteX = spriteX;
        this.spriteY = spriteY;
    }
    
    public String getHash(){
        return this.hash;
    }
    
}
