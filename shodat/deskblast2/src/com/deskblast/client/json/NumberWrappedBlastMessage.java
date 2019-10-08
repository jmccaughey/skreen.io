package com.deskblast.client.json;

import com.google.gwt.core.client.JavaScriptObject;

public class NumberWrappedBlastMessage extends JavaScriptObject {

	protected NumberWrappedBlastMessage() {
	}

	public final native BlastMessage getMessage() /*-{ return this.message; }-*/;
	
	public final native int getNumber() /*-{ return this.number; }-*/;
	
	public static final native NumberWrappedBlastMessage fromJson(String input) /*-{ 
	 return eval('(' + input + ')') 
	 }-*/;

	public final native void setJavaClass(String javaClass) /*-{ 
	 this.javaClass = javaClass; 
	 }-*/;
	
	public final native String getJavaClass() /*-{ 
	 return this.javaClass; 
	 }-*/;

}
