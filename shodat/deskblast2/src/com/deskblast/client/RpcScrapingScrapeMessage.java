package com.deskblast.client;

import java.util.ArrayList;
import java.util.List;


public class RpcScrapingScrapeMessage extends RpcScrapeMessage
implements MergeableMessage{

	private RpcScreenScrapeData rpcScreenScrapeData;
	
	public RpcScrapingScrapeMessage() {	
	}

	public RpcScrapingScrapeMessage(RpcScreenScrapeData rpcScreenScrapeData) {
		super();
		this.rpcScreenScrapeData = rpcScreenScrapeData;
	}

	public RpcScreenScrapeData getRpcScreenScrapeData() {
		return rpcScreenScrapeData;
	}

	@Override
	public MergeableMessage merge(MergeableMessage oldMessage) {
		return oldMessage;
/*		
		if(!(oldMessage instanceof RpcScrapingScrapeMessage)){
			throw new RuntimeException("unexpected mergeable: " + oldMessage);
		}
		RpcScrapingScrapeMessage oldScrapeMessage = (RpcScrapingScrapeMessage)oldMessage;
		if(this.getRpcScreenScrapeData() instanceof RpcScreenScrapeMetaData){
			if(((RpcScreenScrapeMetaData)this.getRpcScreenScrapeData()).isKeyFrame()){
				System.out.println("merging to key frame (dropping other)");
				return this;
			}			
		}
		List<RpcAbstractTileInfo> mergedTiles = new ArrayList<RpcAbstractTileInfo>();
		RpcAbstractTileInfo[] newTiles = this.getRpcScreenScrapeData().getRects();
		RpcAbstractTileInfo[] oldTiles = oldScrapeMessage.getRpcScreenScrapeData().getRects();
		
		for(RpcAbstractTileInfo tiOld : oldTiles){
			boolean found = false;
			for(RpcAbstractTileInfo siNew : newTiles){
				if(tiOld.getX() == siNew.getX() && tiOld.getY() == siNew.getY()){
					found = true;
					break;
				}
			}
			if( ! found){
				System.out.println("carrying over " + tiOld);
				mergedTiles.add(tiOld);
			}else{
				System.out.println("dropping " + tiOld);
			}
		}
		for(RpcAbstractTileInfo tiNew : newTiles){
		    mergedTiles.add(tiNew);
		}
		RpcScreenScrapeData newData = null;
		boolean bKeyFrame = (oldMessage instanceof RpcScreenScrapeMetaData)
			&& ((RpcScreenScrapeMetaData)oldMessage).isKeyFrame();
		if(this.getRpcScreenScrapeData() instanceof RpcScreenScrapeMetaData){
			RpcScreenScrapeMetaData ssmd = (RpcScreenScrapeMetaData)this.getRpcScreenScrapeData();
			newData = new RpcScreenScrapeMetaData(mergedTiles.toArray(new RpcAbstractTileInfo[0]), 
					ssmd.getWidth(), ssmd.getHeight(), ssmd.getMouseX(), ssmd.getMouseY(), bKeyFrame);
		}else if(oldMessage instanceof RpcScreenScrapeMetaData){
			RpcScreenScrapeMetaData ssmd = (RpcScreenScrapeMetaData)oldScrapeMessage.getRpcScreenScrapeData();
			newData = new RpcScreenScrapeMetaData(mergedTiles.toArray(new RpcAbstractTileInfo[0]), 
					ssmd.getWidth(), ssmd.getHeight(), ssmd.getMouseX(), ssmd.getMouseY(), bKeyFrame);
		}else{
			newData = new RpcScreenScrapeData(mergedTiles.toArray(new RpcAbstractTileInfo[0]));
		}
		return new RpcScrapingScrapeMessage(newData);
*/		
	}

//	public void setRpcScreenScrapeData(RpcScreenScrapeData rpcScreenScrapeData) {		
//		this.rpcScreenScrapeData = rpcScreenScrapeData;
//	}
	
	public static void main(String[] args){
		
		
	}
}
