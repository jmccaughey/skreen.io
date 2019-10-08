package com.deskblast.client.json;

import com.google.gwt.core.client.JavaScriptObject;

public class ChatMessage extends JavaScriptObject {

	protected ChatMessage() {
	}

	public final native String getText() /*-{ return this.text; }-*/;

	public final native void setText(String text) /*-{ this.text = text; }-*/;	
	
	public final native BlasterInfo getSender() /*-{ return this.sender; }-*/;

	public final native void setSender(BlasterInfo blasterInfo) /*-{ this.sender = blasterInfo; }-*/;
	
	public static final native ChatMessage fromJson(String input) /*-{ 
	 return eval('(' + input + ')') 
	 }-*/;

	public final native String setJavaClass(String javaClass) /*-{ 
	 this.javaClass = javaClass; 
	 }-*/;

}
