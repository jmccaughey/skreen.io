package com.deskblast.scraper;

import java.awt.Frame;
import java.awt.Rectangle;
import java.util.ArrayList;
import java.util.List;

import com.deskblast.scrape.protocol.AbstractTileInfo;
import com.deskblast.scrape.protocol.CompressedScreenScrape;
import com.deskblast.scrape.protocol.ScreenScrapeData;
import com.deskblast.scrape.protocol.ScreenScrapeDataChunk;
import com.deskblast.scrape.protocol.Streamable;

public class ChunkedScrapeSender extends ScrapeSender {
	
	private int sequence;
	private Uploader uploader = new ParallelUploader();//SerialUploader();
	
	public ChunkedScrapeSender(ServerInfo serverInfo, Frame owner, 
    		ScrapeSource scrapeSource){
		super(serverInfo, owner, scrapeSource);
		baseUrl = buildServletUrl("chunkedscrapes");
	}

	@Override
	public void dataReady(ScreenScrapeData data) {
		if(baseUrl != null){
			// the sub-scrapes will likely be sent up in different threads
			// so ordering is lost. avoid sending up a backreference to 
			// a jpeg that might not be up yet
			// for now, re-order or batch on server?
			List<ScreenScrapeDataChunk> chunks = new ArrayList<ScreenScrapeDataChunk>();
			AbstractTileInfo[] atis =  data.getRects();
			sequence++;
			ScreenScrapeDataChunk ssdc = null;
			for(int i = 0; i < atis.length; i++){
				if(ssdc == null || atis[i] instanceof CompressedScreenScrape){
					ssdc = new ScreenScrapeDataChunk(sequence);
					chunks.add(ssdc);
				}
				ssdc.addTile(atis[i]);
				if(i == (atis.length - 1)){
					ssdc.setLastData(data.isKeyFrame(), chunks.size(), 
							data.getMousePosition(), new Rectangle(data.getWidth(), data.getHeight()));
				}
			}
			if(chunks.size() == 0){
				// mouse-only update
				System.out.println("sending mouse-only update");
				ssdc = new ScreenScrapeDataChunk(sequence);
				ssdc.setLastData(false, 0, data.getMousePosition(), new Rectangle(data.getWidth(), data.getHeight()));
				chunks.add(ssdc);
			}
			try{
				boolean bStopRequested = false;
				List<byte[]> responses = uploader.upload(baseUrl, chunks.toArray(new Streamable[0]));
				for(byte[] response : responses){
					int responseInt = response[0];
					if(responseInt > 0){
		                if(responseInt == 2){
		                    this.keyframeDue = true;
		                }
		            }else{
		            	bStopRequested = true;
		                break;
		            }
				}
				if(bStopRequested){
					stopScraping();
				}else{
					scraper.scrapeRequested();
				}
			}catch (Exception e) {
				e.printStackTrace();
				throw new RuntimeException(e);
			}
		}else{
			throw new RuntimeException("baseUrl not set!");
		}
	
	}
	

}
