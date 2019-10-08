package com.deskblast.server;

import com.deskblast.client.RpcMessage;
import com.deskblast.client.RpcScreenScrapeData;

public interface BlastContext {

	//public void sendToAll(BlastMessage blastMessage);
	
	public void sendRpcToAll(RpcMessage rpcMessage);
	
	//public void sendToAllButSelf(BlastMessage blastMessage);
	
	//public void sendToSelf(BlastMessage blastMessage, Long selfId);
	
	public void setShouldContinueScraping(boolean continueScraping, int blasterId);
	
	public boolean getShouldContinueScraping(int blasterId);
	
	public void addScrapesToCache(RpcScreenScrapeData rpcScreenScrapeData);
	
	public byte[] getScrapeFromCache(String name, int blasterId, String key);

	public void sendRpcToAllButSelf(RpcMessage rpcMessage);

	public void resetChunkState();
	
	
}
