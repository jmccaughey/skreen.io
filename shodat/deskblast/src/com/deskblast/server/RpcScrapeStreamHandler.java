package com.deskblast.server;

import com.deskblast.client.RpcBlasterInfo;
import com.deskblast.client.RpcScreenScrapeData;

public interface RpcScrapeStreamHandler {

	public void handleScrape(RpcBlasterInfo rpcBlasterInfo, RpcScreenScrapeData screenScrapeData);
	
}
