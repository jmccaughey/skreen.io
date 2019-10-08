package com.deskblast.client.json;

import com.google.gwt.core.client.JavaScriptObject;

public class BlasterInfo extends JavaScriptObject {

	protected BlasterInfo() {
	}

	public final native int getId() /*-{ return this.id; }-*/;

	public final native String getName() /*-{ return this.name; }-*/;
	
	public static final native BlasterInfo fromJson(String input) /*-{ 
	 return eval('(' + input + ')') 
	 }-*/;

	public final native String setJavaClass(String javaClass) /*-{ 
	 this.javaClass = javaClass; 
	 }-*/;

}
