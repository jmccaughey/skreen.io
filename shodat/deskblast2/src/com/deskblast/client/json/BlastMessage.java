package com.deskblast.client.json;

import com.google.gwt.core.client.JavaScriptObject;

public class BlastMessage extends JavaScriptObject {

	protected BlastMessage() {
	}

	public final native BlasterInfo getSender() /*-{ return this.sender; }-*/;
	
	public static final native BlastMessage fromJson(String input) /*-{ 
	 return eval('(' + input + ')') 
	 }-*/;

	public final native void setJavaClass(String javaClass) /*-{ 
	 this.javaClass = javaClass; 
	 }-*/;
	
	public final native String getJavaClass() /*-{ 
	 return this.javaClass; 
	 }-*/;

}
