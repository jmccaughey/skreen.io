package com.deskblast.client.json;

import com.google.gwt.core.client.JavaScriptObject;

public class RosterJoinAcceptMessage extends JavaScriptObject {

	protected RosterJoinAcceptMessage() {
	}
	
	public final native ParticipantInfo getRecipient() /*-{ 
		return this.recipient; 
	}-*/;

	public final native String setJavaClass(String javaClass) /*-{ 
		this.javaClass = javaClass; 
	}-*/;

	public static final native RosterJoinAcceptMessage fromJson(String input) /*-{ 
	     return eval('(' + input + ')') 
	}-*/; 
	
}
