package com.deskblast.scraper;

import java.awt.Rectangle;

public interface ScrapeSource {
	
	public boolean hasMoreScrapes();
	public Rectangle getScrapeRegion();
	public void getNextScrape(int[] pixels, Rectangle rectangle);
	
}
