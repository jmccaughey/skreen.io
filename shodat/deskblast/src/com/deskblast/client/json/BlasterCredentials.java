package com.deskblast.client.json;

import com.google.gwt.core.client.JavaScriptObject;

public class BlasterCredentials extends JavaScriptObject {

	protected BlasterCredentials() {
	}

	public final native String getKey() /*-{ return this.key; }-*/;

	public final native int getBlastId() /*-{ return this.blastId; }-*/;
	
	public final native BlasterInfo getBlasterInfo() /*-{ return this.blasterInfo; }-*/;

	public final native boolean isOwner() /*-{ return this.owner; }-*/;
	
	public static final native BlasterCredentials fromJson(String input) /*-{ 
	 return eval('(' + input + ')') 
	 }-*/;

	public final native String setJavaClass(String javaClass) /*-{ 
	 this.javaClass = javaClass; 
	 }-*/;

}
