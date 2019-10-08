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
import java.net.MalformedURLException;
import java.net.URL;

import com.deskblast.scrape.protocol.ScreenScrapeData;

public class ScrapeSender implements ScreenScraperContext{
	
	static final int SCRAPE_INTERVAL = 333;
//	static final String DEFAULT_CONTEXT = "/deskblast";
	
    int scrapes = 0;
    AbstractScreenScraper scraper;
    String baseUrl;
    boolean keyframeDue = true;
    private ServerInfo serverInfo;
    private ScrapeSource scrapeSource;
    
    public ScrapeSender(ServerInfo serverInfo, Frame owner, 
    		ScrapeSource scrapeSource){
    	this.scrapeSource = scrapeSource;
    	this.serverInfo = serverInfo;
    	if(serverInfo != null){
    		baseUrl = buildServletUrl("scrapes");
    	}
        try {
            scraper = AbstractScreenScraper.getInstance();   
            scraper.startScraping(this, owner);
        } catch (Exception e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
    }
    protected String buildServletUrl(String document){
    	String url;
		String portPart = serverInfo.port != null ? (":" + serverInfo.port) : "";
		String contextPart = (serverInfo.context != null && serverInfo.context.length() > 0) 
						? ("/" + serverInfo.context) : "";
		url = (serverInfo.ssl ? "https://" : "http://") 
            + serverInfo.host + portPart + contextPart + "/" + document + "?"            
            + "blastId=" + serverInfo.blastId 
            + "&blasterId=" + serverInfo.blasterId
            + "&key=" + serverInfo.key;
        System.out.println(url);
    	return url;
    }
    public void stopScraping(){    	
    	// TODO: write hash cache to disk, id'ed by the blastId
    	// at startup, load this from disk so that scrapes that
    	// are already on the server don't need to be re-sent
        if(this.scraper != null){
            this.scraper.stopScraping();            
        }
    }    
    
    protected HttpURLConnection openPostConnection() throws IOException{
    	URL url = new URL(baseUrl);
        HttpURLConnection connection;
        connection = (HttpURLConnection) url.openConnection();
        connection.setDoOutput(true);
        connection.setRequestProperty("Content-type", "application/octet-stream");
        connection.connect();
        return connection;
    }
    
    public void dataReady(ScreenScrapeData data) {
        System.out.println(data);
        if(baseUrl != null){
	        try {
	        	//System.out.println
	            long start = System.currentTimeMillis();
	            HttpURLConnection connection = openPostConnection();
	            DataOutputStream oos = new DataOutputStream(connection.getOutputStream());
	            data.writeToDataOututStream(oos);
	            oos.flush();
	            oos.close();
	            int response = connection.getInputStream().read();
	            if(response > 0){
	                if(response == 2){
	                    this.keyframeDue = true;
	                }
	                scraper.scrapeRequested();                
	                scrapes++;
	            }else{
	                stopScraping();
	            }
	            System.out.println("upload took " 
	            		+ (System.currentTimeMillis() - start) + "ms");
	        } catch (Exception e) {
	            // TODO Auto-generated catch block
	            e.printStackTrace();
	        }
        }else{
        	File file = new File("stream.bin");            
            try {
            	DataOutputStream oos = new DataOutputStream(new FileOutputStream(file, true));
				data.writeToDataOututStream(oos);
				oos.flush();
	            oos.close();
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
        	System.out.println("not sending (url is null).");
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
		try{
			URL url = new URL(buildServletUrl("stoppedShowing"));
			HttpURLConnection connection = (HttpURLConnection) url.openConnection();
			connection.setDoOutput(true);
			connection.connect();
			OutputStream os = connection.getOutputStream();
			os.write(13);
			InputStream is = connection.getInputStream();
			is.read();
			os.close();
			is.close();
			System.out.println("opened " + url);
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
	@Override
	public ScrapeSource getScrapeSource() {
		return scrapeSource;
	}
}
