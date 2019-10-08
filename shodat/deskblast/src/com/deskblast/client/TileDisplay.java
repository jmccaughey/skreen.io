package com.deskblast.client;

import java.util.HashMap;
import java.util.Map;

import com.google.gwt.user.client.ui.AbsolutePanel;
import com.google.gwt.user.client.ui.Image;

public class TileDisplay extends AbsolutePanel {
		
	private static final String MOUSE_GIF_STYLE = "mouse-gif";
	public static final String MOUSE_POINTER = "/images/mouse_pointer.gif";
	
	private Image mouseGif = null;
	private int width;
	private int height;
//	private Map<String, Tile> tiles = new HashMap<String, Tile>();
	private Map<String, SpriteTile> tiles = new HashMap<String, SpriteTile>();
	private RpcBlasterCredentials rpcBlasterCredentials;
		
	public TileDisplay(RpcBlasterCredentials rpcBlasterCredentials){
		this.rpcBlasterCredentials = rpcBlasterCredentials;
	}
	public void handleScrape(RpcScrapingScrapeMessage rpcScrapingScrapeMessage){
		RpcScreenScrapeData rpcScreenScrapeData = rpcScrapingScrapeMessage.getRpcScreenScrapeData();
		boolean handlingKeyFrame = false;
		if(rpcScreenScrapeData instanceof RpcScreenScrapeMetaData){			 
			RpcScreenScrapeMetaData ssmd = (RpcScreenScrapeMetaData)rpcScreenScrapeData;
			if((width > 0 && width != ssmd.getWidth())
					||(height > 0 && height != ssmd.getHeight())){
				// TODO: screen size has changed
			}
			width = ssmd.getWidth();
			height = ssmd.getHeight();
			handlingKeyFrame = ssmd.isKeyFrame();
			setWidth(ssmd.getWidth() + "px");
			setHeight(ssmd.getHeight() + "px");
			updateMouse(ssmd.getMouseX(), ssmd.getMouseY(), 
					ssmd.getWidth(), ssmd.getHeight());
			if(handlingKeyFrame){
				//keyframeSetNumber++;
			}
		}else if(width == 0 || height == 0){
			// wait for meta data 
			return;
		}
		for(RpcAbstractTileInfoSprite info : rpcScreenScrapeData.getRects()){
			String name = info.getX() + "_" + info.getY();
			SpriteTile tile = tiles.get(name);
			if(tile == null){
				tile = new SpriteTile(rpcBlasterCredentials, this);
				tiles.put(name, tile);				
			}
			tile.handleTile(info);
		}
	}
	private void updateMouse(int x, int y, int width, int height){
		if(null == mouseGif){
		    mouseGif = new Image();
		    mouseGif.setVisible(false);
		    mouseGif.addStyleName(MOUSE_GIF_STYLE);
		    mouseGif.setUrl(Deskblast.buildServletUrl(MOUSE_POINTER));
		    add(mouseGif, 100, 100);
		    mouseGif.setVisible(true);
		}
		setWidgetPosition(mouseGif, 
		            Math.min(x, width), 
		            Math.min(y, height)
		);
		mouseGif.getElement().scrollIntoView();			
	}
	public void showBlank() {
		if(null != mouseGif){
			mouseGif.setVisible(false);
			mouseGif.removeFromParent();
			mouseGif = null;
		}
		for(SpriteTile tile : this.tiles.values()){
			tile.reset();
		}
		this.tiles.clear();
		setWidth("");
		setHeight("");
	}
	
}
