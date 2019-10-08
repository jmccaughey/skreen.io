package com.deskblast.server.json;

public class StartScrapingMessage extends BlastMessage {
	
	public StartScrapingMessage() {}

	public String getType() {
		return "scrape";
	}
	
}
