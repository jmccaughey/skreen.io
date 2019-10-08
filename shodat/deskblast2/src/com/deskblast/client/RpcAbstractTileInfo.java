package com.deskblast.client;

import java.io.Serializable;


public abstract class RpcAbstractTileInfo implements Serializable
{

    //private static final long serialVersionUID = 1L;

    private String hash;
    
    private int x;
    private int y;
    
    public RpcAbstractTileInfo(){}
    
    public RpcAbstractTileInfo(int x, int y, String hash){
        this.x = x;
        this.y = y;
        this.hash = hash;
    }
    
    public int getX(){ return this.x; }
    public int getY(){ return this.y; }
    
    public String getHash(){
        return this.hash;
    }
    
}
