package com.deskblast.client.json;

import com.google.gwt.core.client.JavaScriptObject;

public class StartScrapingMessage extends JavaScriptObject {

	protected StartScrapingMessage() {
	}

	public final native BlasterInfo getSender() /*-{ return this.sender; }-*/;

	public final native void setSender(BlasterInfo blasterInfo) /*-{ this.sender = blasterInfo; }-*/;
	
	public static final native StartScrapingMessage fromJson(String input) /*-{ 
	 return eval('(' + input + ')') 
	 }-*/;

	public final native String setJavaClass(String javaClass) /*-{ 
	 this.javaClass = javaClass; 
	 }-*/;

}
