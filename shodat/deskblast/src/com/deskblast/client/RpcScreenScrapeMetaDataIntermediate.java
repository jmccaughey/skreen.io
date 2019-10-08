package com.deskblast.client;

import java.io.Serializable;

public class RpcScreenScrapeMetaDataIntermediate 
extends	RpcScreenScrapeMetaData 
implements Serializable{

	public RpcScreenScrapeMetaDataIntermediate(
			RpcAbstractTileInfoSprite[] array, int width, int height,
			int mouseX, int mouseY, boolean bKeyFrame) {
		super(array, width, height, mouseX, mouseY, bKeyFrame);
	}

}
