package com.deskblast.client.json;

import com.google.gwt.core.client.JavaScriptObject;

public class HeartbeatMessage extends JavaScriptObject {

	protected HeartbeatMessage() {
	}
	
	public static final native HeartbeatMessage fromJson(String input) /*-{ 
	 return eval('(' + input + ')') 
	 }-*/;

	public final native String setJavaClass(String javaClass) /*-{ 
	 this.javaClass = javaClass; 
	 }-*/;

}
