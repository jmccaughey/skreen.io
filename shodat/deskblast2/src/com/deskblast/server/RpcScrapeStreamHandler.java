package com.deskblast.server;

import com.deskblast.client.RpcScreenScrapeData;

public interface RpcScrapeStreamHandler {

	public void handleScrape(RpcScreenScrapeData screenScrapeData);
	
}
