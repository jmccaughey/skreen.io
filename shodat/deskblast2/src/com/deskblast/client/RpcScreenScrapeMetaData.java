package com.deskblast.client;


public class RpcScreenScrapeMetaData extends RpcScreenScrapeData {

	private boolean bKeyFrame;
	private int width;
	private int height;
	private int mouseX;
	private int mouseY;
	
	public RpcScreenScrapeMetaData(RpcAbstractTileInfoSprite[] rectsToSend,
			int width, int height, int mouseX, int mouseY, boolean bKeyFrame) {
		super(rectsToSend);
		this.width = width;
		this.height = height;
		this.mouseX = mouseX;
		this.mouseY = mouseY;
		this.bKeyFrame = bKeyFrame;
	}
	public RpcScreenScrapeMetaData(){
		super();
	}
	
	public boolean isKeyFrame() {
		return bKeyFrame;
	}
	public int getWidth() {
		return width;
	}
	public int getHeight() {
		return height;
	}
	public int getMouseX() {
		return mouseX;
	}
	public int getMouseY() {
		return mouseY;
	}

}
