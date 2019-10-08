package com.deskblast.scraper;

import java.awt.Frame;
import java.awt.Rectangle;
import java.awt.Toolkit;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.HttpURLConnection;
import java.net.URL;

import com.deskblast.scrape.protocol.ScreenScrapeData;

public class FileScrapeSender implements ScreenScraperContext{
	
	static final int SCRAPE_INTERVAL = 333;
//	static final String DEFAULT_CONTEXT = "/deskblast";
	
    int scrapes = 0;
    AbstractScreenScraper scraper;
    //String baseUrl;
    boolean keyframeDue = true;
    //private ServerInfo serverInfo;
    private File file;
    private ScrapeSource scrapeSource;
    
    public FileScrapeSender(File file, Frame owner, 
    		ScrapeSource scrapeSource){
    	this.scrapeSource = scrapeSource;
    	//this.serverInfo = serverInfo;
    	this.file = file;
//    	if(file != null){
//    		baseUrl = buildServletUrl("scrapes");
//    	}
        try {
            scraper = AbstractScreenScraper.getInstance();   
            scraper.startScraping(this, owner);
        } catch (Exception e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
    }    
    public void stopScraping(){    	
    	// TODO: write hash cache to disk, id'ed by the blastId
    	// at startup, load this from disk so that scrapes that
    	// are already on the server don't need to be re-sent
        if(this.scraper != null){
            this.scraper.stopScraping();            
        }
    }    
    public void dataReady(ScreenScrapeData data) {
        System.out.println(data);
        if(this.file != null){
	        try {
	            DataOutputStream oos = new DataOutputStream(new FileOutputStream(file));
	            data.writeToDataOututStream(oos);
	            oos.flush();
	            oos.close();
	            this.keyframeDue = false;
	        } catch (Exception e) {
	            // TODO Auto-generated catch block
	            e.printStackTrace();
	        }
        }else{
//        	File file 
        	System.out.println("not writing (file is null).");
        	scraper.scrapeRequested();
        }
    }
    public int getScrapeInterval() {
        return SCRAPE_INTERVAL;
    }
    public Rectangle getScrapeRegion() {
    	if(this.scrapeSource != null){
    		return this.scrapeSource.getScrapeRegion();
    	}
        return new Rectangle(Toolkit.getDefaultToolkit().getScreenSize());
    }
    public boolean isKeyFrameDue() {
        boolean due = this.keyframeDue;
        this.keyframeDue = false;
        return due;
    }
	@Override
	public void userStopped() {
	}
	@Override
	public ScrapeSource getScrapeSource() {
		return scrapeSource;
	}
}
