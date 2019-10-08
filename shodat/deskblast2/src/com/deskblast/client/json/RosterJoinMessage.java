package com.deskblast.client.json;

import com.google.gwt.core.client.JavaScriptObject;

public class RosterJoinMessage extends JavaScriptObject {

	protected RosterJoinMessage() {
	}
	
	public final native void setParticipantID(String participantID) /*-{ this.participantID = participantID; }-*/;
	
	public final native void setSender(ParticipantInfo participantInfo) /*-{ this.sender = participantInfo; }-*/;

	public final native String setJavaClass(String javaClass) /*-{ 
		this.javaClass = javaClass; 
	}-*/;

	public static final native RosterJoinMessage fromJson(String input) /*-{ 
	     return eval('(' + input + ')') 
	}-*/; 
	
}
