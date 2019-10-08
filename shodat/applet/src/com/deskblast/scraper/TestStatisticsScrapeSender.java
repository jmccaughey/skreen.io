package com.deskblast.scraper;

import java.awt.Dimension;
import java.awt.Frame;

import com.deskblast.scrape.protocol.ScreenScrapeData;

public class TestStatisticsScrapeSender extends ScrapeSender {

	private long rectCount;
	private long byteCount;
	private long backreferenceCount;
	private long sends;
	//private Dimension dim;
	
	public TestStatisticsScrapeSender(ServerInfo serverInfo, Frame owner,
			ScrapeSource scrapeSource) {
		super(serverInfo, owner, scrapeSource);
	}

	@Override
	public void dataReady(ScreenScrapeData data) {
		sends++;
		rectCount += data.getRects().length;
		//byteCount += data.getByteCount();
		//backreferenceCount += data.getBackreferenceCount();
//		if(dim == null){
//			dim = new Dimension(data.getCompressedRects()[1].x - data.getCompressedRects()[0].x,
//					data.getCompressedRects()[1].y - data.getCompressedRects()[0].y);
//		}
		printStats();
		super.dataReady(data);
	}

	private void printStats() {
		System.out.println("stats: " + AbstractScreenScraper.TILE_WIDTH + "x" 
				+ AbstractScreenScraper.TILE_HEIGHT 
				+ "\n\tsends: " + sends + "\n\trects: " + rectCount
				//+ "\n\tbytes: " + byteCount + "\n\tback references: " + backreferenceCount
				);
	}

	
	
	
}
