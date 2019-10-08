package com.deskblast.scraper;

import java.awt.Rectangle;

import com.deskblast.scrape.protocol.ScreenScrapeData;

public interface ScreenScraperContext{

	public ScrapeSource getScrapeSource();
	
	public void dataReady(ScreenScrapeData data);//byte[] ba, width);

    public int getScrapeInterval();
        
    public boolean isKeyFrameDue();

    public Rectangle getScrapeRegion();	
    
    public void userStopped();
	
}
