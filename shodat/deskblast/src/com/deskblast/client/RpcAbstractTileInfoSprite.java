package com.deskblast.client;

import java.io.Serializable;


public abstract class RpcAbstractTileInfoSprite implements Serializable
{

    //private static final long serialVersionUID = 1L;

    private String hash;
    
    private int x;
    private int y;
	private int width;
    private int height;
    private int spriteX;
    private int spriteY;
    
    public RpcAbstractTileInfoSprite(){}
    
    public RpcAbstractTileInfoSprite(int x, int y, int width, int height, 
    		String hash, int spriteX, int spriteY){
        this.x = x;
        this.y = y;
        this.width = width;
        this.height = height;
        this.hash = hash;
        this.spriteX = spriteX;
        this.spriteY = spriteY;
    }
    
    public int getX(){ return this.x; }
    public int getY(){ return this.y; }
    
    
    public String getHash(){
        return this.hash;
    }
    public int getWidth() {
		return width;
	}

	public int getHeight() {
		return height;
	}

	public int getSpriteX() {
		return spriteX;
	}

	public int getSpriteY() {
		return spriteY;
	}
    
}
