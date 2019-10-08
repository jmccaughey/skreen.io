package com.deskblast.server.json;

public class StopScrapingMessage extends BlastMessage {
	
	public StopScrapingMessage() {}

	public String getType() {
		return "scrape";
	}
	
}
