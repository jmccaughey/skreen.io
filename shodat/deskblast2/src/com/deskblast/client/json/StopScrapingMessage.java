package com.deskblast.client.json;

import com.google.gwt.core.client.JavaScriptObject;

public class StopScrapingMessage extends JavaScriptObject {

	protected StopScrapingMessage() {
	}

	public final native BlasterInfo getSender() /*-{ return this.sender; }-*/;

	public final native void setSender(BlasterInfo blasterInfo) /*-{ this.sender = blasterInfo; }-*/;
	
	public static final native StopScrapingMessage fromJson(String input) /*-{ 
	 return eval('(' + input + ')') 
	 }-*/;

	public final native String setJavaClass(String javaClass) /*-{ 
	 this.javaClass = javaClass; 
	 }-*/;

}
