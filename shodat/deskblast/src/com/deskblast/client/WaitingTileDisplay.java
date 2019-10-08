package com.deskblast.client;

import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Set;

import com.google.gwt.core.client.GWT;
import com.google.gwt.user.client.ui.AbsolutePanel;
import com.google.gwt.user.client.ui.Image;

public class WaitingTileDisplay extends AbsolutePanel 
//implements LoadHandler 
{
	// active download: when this is done, set background on all, start "on deck" if present 
	// "on deck": if another message comes in, merge them
	
	private static final String MOUSE_GIF_STYLE = "mouse-gif";
	public static final String MOUSE_POINTER = "/images/mouse_pointer.gif";
	
	private Image mouseGif = null;
	private int width;
	private int height;
	private Map<String, SimpleSpriteTile> tiles = new HashMap<String, SimpleSpriteTile>();
	//private Map<String, Image> images = new HashMap<String, Image>();
	private Map<String, RpcAbstractTileInfoSprite> pendingUpdates = new HashMap<String, RpcAbstractTileInfoSprite>();
	private RpcBlasterCredentials rpcBlasterCredentials;
	private boolean downloading = false;
	private int pendingImageCount;
	private static String BASE_URL;
	// TODO: on-deck does not capture mouse location or screen size
	private RpcAbstractTileInfoSprite[] onDeckRects;
	
	public WaitingTileDisplay(RpcBlasterCredentials rpcBlasterCredentials){
		this.rpcBlasterCredentials = rpcBlasterCredentials;
		BASE_URL = Deskblast.buildServletUrl(Deskblast.DOC_SCRAPES)	    	 
    	+ "blastId=" + rpcBlasterCredentials.getBlastId()
    	+ "&key=" + rpcBlasterCredentials.getKey()
    	+ "&blasterId=" + rpcBlasterCredentials.getRpcBlasterInfo().getId()
    	+ "&name=";

	}
	public void imageLoaded(String token){
		pendingImageCount--;
		if(pendingImageCount <= 0){
			allImagesDownloaded();
		}
	}
	public native void preloadImage(String url) /*-{
		var image = new Image();
		var instance = this;
		var callback = function(){
			instance.@com.deskblast.client.WaitingTileDisplay::imageLoaded(Ljava/lang/String;)("hi");
		}
		image.onload = function(){
			console.info('in load handler: ' + image.src);
			callback();
			//console.info('returned from callback');
		};		
		$wnd.console.info("preloading image url is " + url); 
		image.src = url;
//		if(image.width > 0){
//			callback();
//		}
	}-*/;
	public native void logToConsole(String text) /*-{
		$wnd.console.log(text);
	}-*/;
	public void mergeWith(RpcScrapingScrapeMessage rpcScrapingScrapeMessage){
		Map<String, RpcAbstractTileInfoSprite> merged = new HashMap<String, RpcAbstractTileInfoSprite>();
		for(RpcAbstractTileInfoSprite newRect : rpcScrapingScrapeMessage.getRpcScreenScrapeData().getRects()){
			merged.put(newRect.getX() + "_" + newRect.getY(), newRect); 
		}		
		for(RpcAbstractTileInfoSprite oldRect : onDeckRects){
			String key = oldRect.getX() + "_" + oldRect.getY();
			if(merged.containsKey(key)){
				logToConsole("dropping old tile");
			}else{
				merged.put(key, oldRect);
				logToConsole("carried over");
			}
		}
		onDeckRects = merged.values().toArray(new RpcAbstractTileInfoSprite[0]);
	}
	public void handleScrape(RpcScrapingScrapeMessage rpcScrapingScrapeMessage){
		GWT.log("handleScrape");		
		if(downloading){
			logToConsole("download still in progress");
			if(onDeckRects != null && onDeckRects.length > 0){
				logToConsole("merging to on deck");
				mergeWith(rpcScrapingScrapeMessage);
			}else{
				onDeckRects = rpcScrapingScrapeMessage.getRpcScreenScrapeData().getRects();
			}
			return;
		}
		downloading = true;
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
			return;  // wait for meta data
		}
		showRects(rpcScreenScrapeData.getRects());
	}
	private void showRects(RpcAbstractTileInfoSprite[] rects){
		//Set<String> urlSet = new HashSet<String>();
		pendingImageCount = rects.length;
		for(RpcAbstractTileInfoSprite info : rects){
			String name = info.getX() + "_" + info.getY();
			SimpleSpriteTile tile = tiles.get(name);
			if(tile == null){
				insertTile(name);
			}
			final String url = BASE_URL + info.getHash();
			pendingUpdates.put(name, info);
			//if(urlSet.add(url)){
				//pendingImageCount++;
				preloadImage(url);
			//}
		}
	}
	private void allImagesDownloaded() {
		Set<Entry<String, RpcAbstractTileInfoSprite>> entrySet = pendingUpdates.entrySet();
		for(Entry<String, RpcAbstractTileInfoSprite> entry : entrySet){
			tiles.get(entry.getKey()).handleTile(entry.getValue());//.setBackground(""+entry.getValue());
		}
		downloading = false;
		pendingUpdates.clear();
		pendingImageCount = 0;
		if(onDeckRects != null && onDeckRects.length > 0){
			RpcAbstractTileInfoSprite[] rectsCopy = new RpcAbstractTileInfoSprite[onDeckRects.length];
			for(int i = 0; i < onDeckRects.length; i++){
				rectsCopy[i] = onDeckRects[i];
			}
			onDeckRects = null;
			showRects(rectsCopy);
		}
	}
	private void insertTile(String name){
		tiles.put(name, new SimpleSpriteTile(rpcBlasterCredentials, this));
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
		for(SimpleSpriteTile tile : this.tiles.values()){
			tile.reset();
		}
		this.tiles.clear();
		setWidth("");
		setHeight("");
	}
}
