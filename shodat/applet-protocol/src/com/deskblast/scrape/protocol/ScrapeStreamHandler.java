package com.deskblast.scrape.protocol;

public interface ScrapeStreamHandler {

	//public void handleMetadata(ScreenScrapeMetaData screenScrapeMetaData);
	public void handleScrape(ScreenScrapeData screenScrapeData);
	
}
