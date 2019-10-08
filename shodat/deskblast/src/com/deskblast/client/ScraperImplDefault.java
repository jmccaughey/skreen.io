package com.deskblast.client;

import com.google.gwt.user.client.DOM;

public class ScraperImplDefault {
	
	protected ScraperParms scraperParms;
	public static final String SCRAPE_START_PENDING = "SCRAPE_START_PENDING";
	
	public boolean installPlugin(ScraperParms scraperParms){
		this.scraperParms = scraperParms;
		// default plugin is Java applet
		return true;
	}
	public void startScraping(){
		if(null == DOM.getElementById(scraperParms.getScraperId())){
			insertApplet(scraperParms.getBaseUrl(), 
					scraperParms.getHost(), 
					scraperParms.getPort(), 
					scraperParms.getContext(), 
					scraperParms.getContainerId(), 
					scraperParms.getScraperId(), 
					scraperParms.getBlastId(), 
					scraperParms.getBlasterId(), 
					scraperParms.getKey());
		}
	}
	public void stopScraping(){		
		//Window.alert("DefaultScraperImpl: remove applet here");
		removeApplet(scraperParms.getContainerId());
	}
	public String buildBaseUrl(String host, String port, String context, boolean ssl){
		String contextPart = (context != null && context.length() > 0) ? ("/" + context) : "";
		String schemePart = ssl ? "https://" : "http://";
		String result = schemePart + host + ":" + port + contextPart; 
		return result;				
	}
	private final native void removeApplet(String containerId) /*-{
		var container = $wnd.document.getElementById(containerId);
		container.innerHTML = "";
	}-*/;
	private final native void insertApplet(String baseUrl, String host, String port, 
			String context, String containerId, String scraperId, String blastId, String blasterId, String key) /*-{
		var container = $wnd.document.getElementById(containerId);
		var content = '<applet id="' + scraperId + '" name="shodat" width="1" height="1"' 
		 + ' code="com.deskblast.scraper.ScraperApplet"'
		 + ' archive="' + baseUrl + 'shodat-1.0.jar' + '">'
		 + ' <param name="blastId" value="' + blastId + '">'
		 + ' <param name="blasterId" value="' + blasterId + '">'
		 + ' <param name="key" value="' + key + '">'
		 + ' <param name="context" value="' + context + '">'
		 // TODO: discover host & port from codebase
		 + ' <PARAM name="host" value="' + host + '">'
		 + ' <PARAM name="port" value="' + port + '">'
		 + '</applet>';  
		 container.innerHTML = content;
	}-*/;
	
}
