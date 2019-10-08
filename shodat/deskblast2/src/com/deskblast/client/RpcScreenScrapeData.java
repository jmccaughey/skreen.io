package com.deskblast.client;

import java.io.Serializable;

public class RpcScreenScrapeData implements Serializable{

	private RpcAbstractTileInfoSprite[] rects;
    
	public RpcScreenScrapeData(){}
	public RpcScreenScrapeData(RpcAbstractTileInfoSprite[] rectsToSend) {
		this.rects = rectsToSend;
	}
	public RpcAbstractTileInfoSprite[] getRects() {
		return this.rects;
	}
}
