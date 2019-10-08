package com.deskblast.client;

import java.util.logging.Logger;

import com.google.gwt.dom.client.Style.Display;
import com.google.gwt.user.client.ui.AbsolutePanel;
import com.google.gwt.user.client.ui.HTML;
import com.google.gwt.user.client.ui.Image;

public class SpriteTile {
	
	private HTML div;
	private String background;
	private AbsolutePanel parentPanel;
	private static Logger logger = Logger.getLogger(SpriteTile.class + "");
	private static String BASE_URL;
	private Image image;
	private String pendingUrl;
	private String pendingBackground;
	private String divId;

	public SpriteTile(RpcBlasterCredentials rpcBlasterCredentials, AbsolutePanel absolutePanel){
		this.parentPanel = absolutePanel;
		BASE_URL = Deskblast.buildServletUrl(Deskblast.DOC_SCRAPES)	    	 
	    	+ "blastId=" + rpcBlasterCredentials.getBlastId()
	    	+ "&key=" + rpcBlasterCredentials.getKey()
	    	+ "&blasterId=" + rpcBlasterCredentials.getRpcBlasterInfo().getId()
	    	+ "&name=";
	}
	public void handleTile(RpcAbstractTileInfoSprite info){
		divId = info.getX() + "_" + info.getY() + "_a";
		if(div == null){
			String imageId = divId + "_image";
			div = new HTML();
			parentPanel.add(div, info.getX(), info.getY());			
			div.getElement().setId(divId);
			image = new Image();
			div.getElement().appendChild(image.getElement());
			image.getElement().getStyle().setDisplay(Display.NONE);
			image.getElement().setId(imageId);
			div.setWidth(info.getWidth() + "px");
			div.setHeight(info.getHeight() + "px");			
		}
		String url = BASE_URL + info.getHash();
		String background = "url('" + url + "') -" + info.getSpriteX() + "px -" 
			+ info.getSpriteY() + "px";
		loadImageAndSetBackground(url, background);
	}
	
	public void checkForPending(String bogo){
		if(pendingUrl != null && pendingBackground != null){
			String newUrl = pendingUrl;
			String newBackground = pendingBackground;
			pendingUrl = null;
			pendingBackground = null;
			loadImageAndSetBackground(newUrl, newBackground);
		}		
	}
	
	public native void loadImageAndSetBackground(String url, String background) /*-{
		var image = $doc.getElementById(this.@com.deskblast.client.SpriteTile::divId + "_image");
		if(image.complete == true || image.src.length == 0){		
			var divId = this.@com.deskblast.client.SpriteTile::divId;
			var instance = this;
			var callback = function(){
				instance.@com.deskblast.client.SpriteTile::checkForPending(Ljava/lang/String;)("hi");
			}
			image.onload = function(){ 
				var div = $doc.getElementById(divId);
				div.style.background = background;
				callback();
			}			
			image.src = url;
		}else{
			// image is being loaded
//			if(this.@com.deskblast.client.SpriteTile::pendingUrl != null){
//				console.log("skipping image");
//			}
			this.@com.deskblast.client.SpriteTile::pendingUrl = url;
			this.@com.deskblast.client.SpriteTile::pendingBackground = background;			
		}
	}-*/;
	public void reset() {
		if(div != null){
			div.removeFromParent();
			div = null;
		}
	}
}