package com.deskblast.client;

import java.util.Date;

import com.google.gwt.user.client.Cookies;
import com.google.gwt.user.client.DOM;

public class ScraperImplIE extends ScraperImplDefault{

	public void startScraping(){
		if(isIE32BitOnWindows()){
			if(DOM.getElementById(this.scraperParms.getScraperId()) == null){
				insertActiveX();
				if(null != Cookies.getCookie(SCRAPE_START_PENDING)){					
					Cookies.removeCookie(SCRAPE_START_PENDING);
				}else{
					Cookies.setCookie(SCRAPE_START_PENDING, new Date().getTime()+"");
				}							
			}else{
				resumeScraping(this.scraperParms.getScraperId());
			}
		}else{
			super.startScraping();
		}
	}
	public void stopScraping(){
		if(isIE32BitOnWindows()){
			activexStopScraping(this.scraperParms.getScraperId());
		}else{
			super.stopScraping();
		}
	}
	private void insertActiveX(){
		insertActiveXImpl(
				scraperParms.getBaseUrl(), 
				scraperParms.getHost(), 
				scraperParms.getPort(), 
				scraperParms.getContext(),
				scraperParms.getContainerId(),
				scraperParms.getBlastId(),
				scraperParms.getBlasterId(), 
				scraperParms.getKey()
		);
	}
	private final native void insertActiveXImpl(String baseUrl, String host, String port,
			String context, String containerId,	String blastId, String blasterId, String key) /*-{
		var container = $wnd.document.getElementById(containerId);
		//var unsafeHost = 'ksjhdfksjdhfksjdhfkjshfksjdhfksjhdfkjsdhfkjshdfkjshkfjshkdfjhskdjfhksjdfhksjfhksjdfhksjdfhksjdfhksjdfhksjdfhskdfjhskdjfhskjdfhskjdfhksjdfhksjdfhksdjfhksdjfhksdjfhlkjdfhlkjghlksfgjhdfklghsdfkljghkldsfjghskldfjghdklsfjghldkfjghsldfkjghslkdfjghskldfjghsdklfjghsdlkfjghdsfklghdfklghdklfjghkldfjghdkfjghdklfjksjhdfksjdhfksjdhfkjshfksjdhfksjhdfkjsdhfkjshdfkjshkfjshkdfjhskdjfhksjdfhksjfhksjdfhksjdfhksjdfhksjdfhksjdfhskdfjhskdjfhskjdfhskjdfhksjdfhksjdfhksdjfhksdjfhksdjfhlkjdfhlkjghlksfgjhdfklghsdfkljghkldsfjghskldfjghdklsfjghldkfjghsldfkjghslkdfjghskldfjghsdklfjghsdlkfjghdsfklghdfklghdklfjghkldfjghdkfjghdklfjksjhdfksjdhfksjdhfkjshfksjdhfksjhdfkjsdhfkjshdfkjshkfjshkdfjhskdjfhksjdfhksjfhksjdfhksjdfhksjdfhksjdfhksjdfhskdfjhskdjfhskjdfhskjdfhksjdfhksjdfhksdjfhksdjfhksdjfhlkjdfhlkjghlksfgjhdfklghsdfkljghkldsfjghskldfjghdklsfjghldkfjghsldfkjghslkdfjghskldfjghsdklfjghsdlkfjghdsfklghdfklghdklfjghkldfjghdkfjghdklfjksjhdfksjdhfksjdhfkjshfksjdhfksjhdfkjsdhfkjshdfkjshkfjshkdfjhskdjfhksjdfhksjfhksjdfhksjdfhksjdfhksjdfhksjdfhskdfjhskdjfhskjdfhskjdfhksjdfhksjdfhksdjfhksdjfhksdjfhlkjdfhlkjghlksfgjhdfklghsdfkljghkldsfjghskldfjghdklsfjghldkfjghsldfkjghslkdfjghskldfjghsdklfjghsdlkfjghdsfklghdfklghdklfjghkldfjghdkfjghdklfj';
		//var unsafePort = 'dfgdgd';
		var content = '<OBJECT id="scraper" name="shodat" WIDTH=1 HEIGHT=1 ' 
			+ ' CLASSID="CLSID:CF11AE46-27B0-475D-8187-133D1D47BFA4" '
			+ ' CODEBASE="' + baseUrl + 'shodat-signed.cab#Version=-1,-1,-1,-1">'
			+ ' <PARAM name="blastId" value="' + blastId + '">'
			+ ' <PARAM name="blasterId" value="' + blasterId + '">'
			+ ' <PARAM name="key" value="' + key + '">'
			+ ' <param name="context" value="' + context + '">'
			// TODO: discover host & port from setsite
			+ ' <PARAM name="host" value="' + host + '">'
			+ ' <PARAM name="port" value="' + port + '">'
			+ '</OBJECT>';			  
		//alert(content);
		container.innerHTML = content;		
	}-*/;
	
//	public void handleDoneScraping(String status){
//		Window.alert("handling in java: " + status);
//	}
//	private native void registerScraperDone(ScraperImplIE impl)/*-{
//		$wnd.handleDoneScraping = function(status) {
//			impl.@com.deskblast.client.ScraperImplIE::handleDoneScraping(Ljava/lang/String;)(status);
//		};
//	}-*/;
	private final native void resumeScraping(String scraperId) /*-{
		var scraper = $wnd.document.getElementById(scraperId);
		scraper.resumeScraping();
	}-*/;
	private final native void activexStopScraping(String scraperId) /*-{
    	var scraper = $wnd.document.getElementById(scraperId);
	    scraper.stopScraping();
    }-*/;
	native private boolean isIE32BitOnWindows() /*-{
	  var userAgent = navigator.userAgent.toLowerCase();
	  if(userAgent.indexOf('msie') != -1){
	  	if(userAgent.indexOf('windows') != -1){
	  	  if(userAgent.indexOf('win64') == -1){
	  	  	if(userAgent.indexOf('x64') == -1){
	  	  		return true;
	  	  	}
	  	  }
	  	}
	  }
	  return false;
	}-*/;
}
