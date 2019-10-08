package com.deskblast.server;

import com.deskblast.client.RpcMessage;
import com.deskblast.client.RpcScrapeMessage;
import com.deskblast.client.RpcScrapingScrapeMessage;
import com.deskblast.client.RpcScreenScrapeData;
import com.deskblast.client.RpcStartScrapingMessage;
import com.deskblast.client.RpcStopScrapingMessage;
import com.deskblast.server.json.BlastMessage;

public class ScrapeContentHandler implements ContentHandler {

	public String getName() {
		return "scrape";	
	}
	public void handleContent(RpcMessage rpcMessage,
			BlastContext blastContext) {
		if(rpcMessage instanceof RpcScrapingScrapeMessage){
		    RpcScrapingScrapeMessage rpcInboundScrapeMessage = 
		            (RpcScrapingScrapeMessage)rpcMessage;
		    blastContext.addScrapesToCache(rpcInboundScrapeMessage
		            .getRpcScreenScrapeData());
		    if(blastContext.getShouldContinueScraping(
		    				rpcMessage.getSender().getId())){
		    	sendScrapeDataMessage(rpcInboundScrapeMessage, blastContext);
		    }else{
		    	System.out.println("ignoring scrape");
		    }
		}else if(rpcMessage instanceof RpcStopScrapingMessage){
			System.out.println("got stop scrape message");
		    blastContext.setShouldContinueScraping(
		            false, rpcMessage.getSender().getId());
		    blastContext.sendRpcToAll(rpcMessage);
		}else if(rpcMessage instanceof RpcStartScrapingMessage){
		    System.out.println("got start scrape message");
		    blastContext.resetChunkState();
		    blastContext.setShouldContinueScraping(
                   true, rpcMessage.getSender().getId());
		    blastContext.sendRpcToAll(rpcMessage);
		}else{
		    System.out.println("unexpected scrape message: " + rpcMessage);
		}
	}

    private void sendScrapeDataMessage(
            RpcScrapingScrapeMessage inboundScrapeMessage, 
            BlastContext blastContext) {
        RpcScreenScrapeData screenScrapeData = 
            inboundScrapeMessage.getRpcScreenScrapeData(); 
        RpcScrapeMessage rpcScrapeMessage = new RpcScrapingScrapeMessage(screenScrapeData);
        rpcScrapeMessage.setSender(inboundScrapeMessage.getSender());
        blastContext.sendRpcToAllButSelf(rpcScrapeMessage);
    }
	@Override
	public void handleJsonContent(BlastMessage message,
			BlastContext messageSender) {
		// TODO Auto-generated method stub
		
	}

}
