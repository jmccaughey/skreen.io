package com.deskblast.client;


public class ScraperParms {
	
	private String containerId;
	private String scraperId; 
	private String host; 
	private String port;
	private String context;
	private String blastId; 
	private String blasterId;
	private String key;
	private boolean ssl;
	
	public ScraperParms(String containerId, String scraperId, String host,
			String port, String context, String blastId, String blasterId,
			String key, boolean ssl) {
		super();
		this.containerId = containerId;
		this.scraperId = scraperId;
		this.host = host;
		this.port = port;
		this.context = context;
		this.blastId = blastId;
		this.blasterId = blasterId;
		this.key = key;
		this.ssl = ssl;
	}
	
	/**
	 * @return the absolute URL including context and trailing slash ('http://host:port/context/')
	 */
	public String getBaseUrl(){
		String schemePart = ssl ? "https://" : "http://";
		String portPart = (port != null && port.trim().length() > 0)
			? (":" + port) : "";
		String contextPart = (context != null && context.length() > 0) ? ("/" + context + "/") : "/";
		String baseUrl = schemePart + host + portPart + contextPart;	
		return baseUrl;
	}

	public String getContainerId() {
		return containerId;
	}

	public String getScraperId() {
		return scraperId;
	}

	public String getBlastId() {
		return blastId;
	}

	public String getBlasterId() {
		return blasterId;
	}

	public String getKey() {
		return key;
	}	
	public String getHost() {
		return host;
	}

	public String getPort() {
		return port;
	}

	public String getContext() {
		return context;
	}
	
	//	
//	public static void main(String[] args){
//		ScraperParms scraperParms = new ScraperParms("containerId", "scraper", 
//				"localhost", 
//				null
//				//"9999"
//				, 
//				//"context"
//				null
//				, 
//				"34534", "3457876567", "DFG456457457DFGF", true);
//		System.out.println(scraperParms.getBaseUrl());
//		
//	}
}
