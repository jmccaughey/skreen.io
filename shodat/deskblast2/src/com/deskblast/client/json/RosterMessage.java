package com.deskblast.client.json;

import com.google.gwt.core.client.JavaScriptObject;
import com.google.gwt.core.client.JsArray;

public class RosterMessage extends JavaScriptObject {

	protected RosterMessage() {
	}

	public final native JsArray<BlasterInfo> getBlasterInfos() /*-{ return this.blasterInfos; }-*/;

//	public final native void setBlasterInfos(BlasterInfo[] blasterInfos) /*-{ this.blasterInfos = blasterInfos; }-*/;
	
	public static final native RosterMessage fromJson(String input) /*-{ 
	 return eval('(' + input + ')') 
	 }-*/;

	public final native String setJavaClass(String javaClass) /*-{ 
	 this.javaClass = javaClass; 
	 }-*/;

}
