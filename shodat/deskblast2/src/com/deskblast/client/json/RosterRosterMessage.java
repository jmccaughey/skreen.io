package com.deskblast.client.json;

import com.google.gwt.core.client.JavaScriptObject;

public class RosterRosterMessage extends JavaScriptObject {

	protected RosterRosterMessage() {
	}

	public final native ParticipantInfo[] getRoster() /*-{
	 return this.roster;
	}-*/;
	
	public static final native RosterRosterMessage fromJson(String input) /*-{ 
	     return eval('(' + input + ')') 
	}-*/; 
	
}
