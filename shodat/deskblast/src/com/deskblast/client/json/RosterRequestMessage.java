package com.deskblast.client.json;

import com.google.gwt.core.client.JavaScriptObject;

public class RosterRequestMessage extends JavaScriptObject {

	protected RosterRequestMessage() {
	}
	
	public final native void setSender(ParticipantInfo participantInfo) /*-{ this.sender = participantInfo; }-*/;

	public final native String setJavaClass(String javaClass) /*-{ 
		this.javaClass = javaClass; 
	}-*/;

	public static final native RosterRequestMessage fromJson(String input) /*-{ 
	     return eval('(' + input + ')') 
	}-*/; 
	
}
