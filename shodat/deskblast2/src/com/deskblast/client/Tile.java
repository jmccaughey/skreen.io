package com.deskblast.client;

import java.util.logging.Level;
import java.util.logging.Logger;

import com.google.gwt.event.dom.client.ErrorEvent;
import com.google.gwt.event.dom.client.ErrorHandler;
import com.google.gwt.event.dom.client.LoadEvent;
import com.google.gwt.event.dom.client.LoadHandler;
import com.google.gwt.event.shared.HandlerRegistration;
import com.google.gwt.user.client.ui.AbsolutePanel;
import com.google.gwt.user.client.ui.Image;

public class Tile 
//implements LoadHandler, ErrorHandler
{
	
	private Image image;
	private boolean fetching;
	private HandlerRegistration handlerRegistration;
	private RpcBlasterCredentials rpcBlasterCredentials;
	private AbsolutePanel absolutePanel;
	private static Logger logger = Logger.getLogger(Tile.class + "");
	private static String BASE_URL;
	//private static String PARM_NAME = "name=";
	
	private RpcAbstractTileInfoSprite pending;

	public Tile(RpcBlasterCredentials rpcBlasterCredentials, AbsolutePanel absolutePanel){
		this.rpcBlasterCredentials = rpcBlasterCredentials;
		this.absolutePanel = absolutePanel;
		BASE_URL = Deskblast.buildServletUrl(Deskblast.DOC_SCRAPES)	    	 
	    	+ "blastId=" + rpcBlasterCredentials.getBlastId()
	    	+ "&key=" + rpcBlasterCredentials.getKey()
	    	+ "&blasterId=" + rpcBlasterCredentials.getRpcBlasterInfo().getId()
	    	+ "&name=";
	}
	public void handleTile(RpcAbstractTileInfoSprite info){
		if(image == null){
			image = new Image();
			image.getElement().setId(info.getX() + "_" + info.getY() + "_a");
			image.getElement().addClassName("tile");
			handlerRegistration = image.addLoadHandler(
					new LoadHandler() {
						@Override
						public void onLoad(LoadEvent event) {
							if(pending != null){
								logger.log(Level.INFO, "image loaded. getting pending image");
								RpcAbstractTileInfoSprite info = pending;
								pending = null;
								String source = BASE_URL + info.getHash(); 
								image.setUrl(source);		
							}
						}
			});
			image.addErrorHandler(new ErrorHandler() {
				@Override
				public void onError(ErrorEvent event) {
					logger.log(Level.SEVERE, "error loading image " + event.getSource());
					if(pending != null){
						//logger.log(Level.INFO, "image loaded. getting pending image");
						RpcAbstractTileInfoSprite info = pending;
						pending = null;
						String source = BASE_URL + info.getHash(); 
						//logger.log(Level.INFO, "fetching " + source);
						image.setUrl(source);		
					}					
				}
			});
			absolutePanel.add(image, info.getX(), info.getY());
		}
		if(image.getUrl() == null || image.getUrl().length() < 1 
				|| image.getElement().getPropertyBoolean("complete")){
			String source = BASE_URL + info.getHash(); 
			//logger.log(Level.INFO, "fetching " + source);
			image.setUrl(source);		
		}else{
			if(pending != null){
				logger.log(Level.INFO, "dropping pending tile");
			}else{
				logger.log(Level.INFO, "queueing tile");
			}
			pending = info;
		}
	}
//	@Override
//	public void onLoad(LoadEvent event) {
//		startPendingIfNeeded();
//	}
//	@Override
//	public void onError(ErrorEvent event) {
//		logger.log(Level.SEVERE, "error loading image");
//		startPendingIfNeeded();
//	}
	public void reset() {
		if(image != null){
			image.removeFromParent();
			image = null;
		}
	}
//	private void startPendingIfNeeded(){
//		if(pending != null){
//			//logger.log(Level.INFO, "image loaded. getting pending image");
//			RpcAbstractTileInfo info = pending;
//			pending = null;
//			String source = BASE_URL + info.getHash(); 
//			//logger.log(Level.INFO, "fetching " + source);
//			image.setUrl(source);		
//		}
//	}
}
