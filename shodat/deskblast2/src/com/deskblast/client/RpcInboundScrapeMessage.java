package com.deskblast.client;


public class RpcInboundScrapeMessage extends RpcMessage{

	private RpcScreenScrapeData rpcScreenScrapeData;
	
	public RpcInboundScrapeMessage(RpcScreenScrapeData screenScrapeData){
		this.rpcScreenScrapeData = screenScrapeData;
	}
	
	public RpcScreenScrapeData getScreenScrapeData(){
		return this.rpcScreenScrapeData;
	}

}
