package com.deskblast.client;

import java.util.logging.Logger;

import com.google.gwt.user.client.ui.AbsolutePanel;
import com.google.gwt.user.client.ui.HTML;

public class SimpleSpriteTile {
	
	private HTML div;
	private String background;
	private AbsolutePanel parentPanel;
	private static Logger logger = Logger.getLogger(SimpleSpriteTile.class + "");
	private static String BASE_URL;
	private String divId;

	public SimpleSpriteTile(RpcBlasterCredentials rpcBlasterCredentials, AbsolutePanel absolutePanel){
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
			div = new HTML();
			parentPanel.add(div, info.getX(), info.getY());			
			div.getElement().setId(divId);
			div.setWidth(info.getWidth() + "px");
			div.setHeight(info.getHeight() + "px");			
		}
		String url = BASE_URL + info.getHash();
		String background = "url('" + url + "') -" + info.getSpriteX() + "px -" 
			+ info.getSpriteY() + "px";
		setBackground(background);
	}
	
	public native void setBackground(String background) /*-{
		var divId = this.@com.deskblast.client.SimpleSpriteTile::divId;
		var div = $doc.getElementById(divId);
		div.style.background = background;
		div.style.display = "block";
	}-*/;
	public void reset() {
		if(div != null){
			div.removeFromParent();
			div = null;
		}
	}
}